#pragma once

#include "MarketSnapshot.hpp"
#include <atomic>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <pqxx/pqxx>
#include <string>
#include <thread>
#include <vector>

class DatabaseLogger {
public:
  explicit DatabaseLogger(const std::string &connectionStr);
  ~DatabaseLogger();

  DatabaseLogger(const DatabaseLogger &) = delete;
  DatabaseLogger &operator=(const DatabaseLogger &) = delete;

  void push(const MarketSnapshot &snapshot);

private:
  void processQueue();
  void writeToDb(const std::vector<MarketSnapshot> &batch);

  pqxx::connection conn_;
  std::deque<MarketSnapshot> queue_;
  std::mutex mtx_;
  std::condition_variable cv_;

  std::thread worker_;
  std::atomic<bool> running_;
};
