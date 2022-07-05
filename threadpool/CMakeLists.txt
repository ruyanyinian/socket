message(STATUS "START TO BUILD THREADPOOL")
message(STATUS "CURRENT CMAKE DIR IS ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "PROJECT SOURCE DIR IS ${PROJECT_SOURCE_DIR}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")


#set(ThreadPoolSRCS ${CMAKE_CURRENT_SOURCE_DIR}/ThreadPool.c)

set(TaskQueueSRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/TaskQueue.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/TaskQueueC_API.cpp)


add_library(TaskQueue SHARED ${TaskQueueSRCS})
target_include_directories(TaskQueue PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR})

#add_library(ThreadPool SHARED ${ThreadPoolSRCS})
#
#target_link_libraries(ThreadPool PRIVATE TaskQueue)

#target_link_libraries(${PROJECT_NAME} PRIVATE TaskQueue)
#target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
#target_include_directories(TaskQueue PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
