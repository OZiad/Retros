cmake_minimum_required(VERSION 3.12.0)

set(VERSION "0.0.1")
set(PROJECT_NAME "Retros")

project(${PROJECT_NAME} VERSION ${VERSION} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} src/main.cpp src/MappedFile.cpp)
add_executable(bitmap_test tests/TickBitMapTest.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

set(CTEST_OUTPUT_ON_FAILURE 1)
include(CTest)
enable_testing()
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)


target_link_libraries(bitmap_test GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(bitmap_test)
