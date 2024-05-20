cmake_minimum_required(VERSION 3.28)
project(Task)

set(CMAKE_CXX_STANDARD 17)

add_executable(Task main.cpp
        ../Task/main.cpp
)

find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})
target_link_libraries(Task ${OpenCL_LIBRARIES})
