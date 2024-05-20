cmake_minimum_required(VERSION 3.28)
project(iGPU)

set(CMAKE_CXX_STANDARD 17)

add_executable(iGPU main.cpp)
find_package(OpenCL REQUIRED)

target_include_directories(iGPU PRIVATE ${OpenCL_INCLUDE_DIRS})
target_link_libraries(iGPU PRIVATE ${OpenCL_LIBRARIES})

