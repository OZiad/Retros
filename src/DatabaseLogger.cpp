#include "DatabaseLogger.hpp"
#include <iostream>

DatabaseLogger::DatabaseLogger(const std::string &connectionStr)
    : conn_(connectionStr), running_(true) {
  worker_ = std::thread(&DatabaseLogger::processQueue, this);
}

DatabaseLogger::~DatabaseLogger() {
  running_ = false;
  cv_.notify_one();
  if (worker_.joinable()) {
    worker_.join();
  }
}

void DatabaseLogger::push(const MarketSnapshot &snapshot) {
  {
    std::lock_guard<std::mutex> lock(mtx_);
    queue_.push_back(snapshot);
  }

  cv_.notify_one();
}

void DatabaseLogger::processQueue() {
  while (true) {
    std::vector<MarketSnapshot> batch;
    {
      std::unique_lock<std::mutex> lock(mtx_);
      cv_.wait(lock, [this] { return !queue_.empty() || !running_; });

      if (!running_ && queue_.empty()) {
        break;
      }

      batch.insert(batch.end(), std::make_move_iterator(queue_.begin()),
                   std::make_move_iterator(queue_.end()));
      queue_.clear();
    }

    if (!batch.empty()) {
      writeToDb(batch);
    }
  }
}

void DatabaseLogger::writeToDb(const std::vector<MarketSnapshot> &batch) {
  try {
    pqxx::work tx(conn_);

    for (const auto &s : batch) {
      tx.exec_params(
          "INSERT INTO market_snapshots (event_time, symbol, best_bid_price, "
          "best_bid_size, best_ask_price, best_ask_size) "
          "VALUES (to_timestamp($1 / 1000000000.0), $2, $3, $4, $5, $6)",
          s.timestamp, s.symbol, static_cast<int64_t>(s.bidPrice), s.bidSize,
          static_cast<int64_t>(s.askPrice), s.askSize);
    }

    tx.commit();
  } catch (const std::exception &e) {
    std::cerr << "postgres logging error: " << e.what() << std::endl;
  }
}
