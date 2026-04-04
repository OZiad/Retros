cmake_minimum_required(VERSION 3.5.0)

set(VERSION "0.0.1")
set(PROJECT_NAME "Retros")

project(${PROJECT_NAME} VERSION ${VERSION} LANGUAGES CXX)

add_executable(${PROJECT_NAME} src/main.cpp src/MappedFile.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

include(CTest)
enable_testing()
