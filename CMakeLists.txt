cmake_minimum_required(VERSION 3.10)
project(socket)

#add_executable(server ${CMAKE_CURRENT_SOURCE_DIR}/server.cpp threadpool/ThreadPool.c threadpool/ThreadPool.h threadpool/TaskQueue.cpp threadpool/TaskQueue.h)
#target_link_libraries(server PRIVATE pthread)
#add_executable(client ${CMAKE_CURRENT_SOURCE_DIR}/client.cpp)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
add_subdirectory(threadpool)