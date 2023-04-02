cmake_minimum_required(VERSION 3.22)
project(multiThd)

set(CMAKE_CXX_STANDARD 20)
include_directories(${PROJECT_SOURCE_DIR})
#set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(multiThd main.cpp)
target_link_libraries(multiThd -pthread)
