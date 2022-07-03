cmake_minimum_required(VERSION 3.10)
project(SOCKET_PRACTICE)

add_executable(server ${CMAKE_CURRENT_SOURCE_DIR}/server.c)
target_link_libraries(server PRIVATE pthread)
#add_executable(client ${CMAKE_CURRENT_SOURCE_DIR}/client.cpp)
