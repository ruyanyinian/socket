cmake_minimum_required(VERSION 3.10)
project(ThreadPool)

# 输出一些信息
message(STATUS "START TO BUILD THREADPOOL")
message(STATUS "CURRENT CMAKE DIR IS ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "PROJECT SOURCE DIR IS ${PROJECT_SOURCE_DIR}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../libs)

set(TaskQueueSRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/taskqueue.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/threadpool.cpp)


add_library(TaskQueue SHARED ${TaskQueueSRCS})
message(STATUS "build target taskqueue.so in ${CMAKE_CURRENT_SOURCE_DIR}/../../libs")
target_include_directories(TaskQueue PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
