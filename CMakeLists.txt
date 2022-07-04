cmake_minimum_required(VERSION 3.10)
project(socket)

# Note: 为什么不显式指定bin目录的话就可以直接子目录生成的库就可以链接上了? 干脆生成的库到最后我们install下然后安装到指定目录吧
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY)
add_subdirectory(threadpool)
add_executable(test ${CMAKE_CURRENT_SOURCE_DIR}/unitest.c)
target_link_libraries(test PRIVATE TaskQueue)