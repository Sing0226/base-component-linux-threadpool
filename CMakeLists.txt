# cmake最低版本
cmake_minimum_required(VERSION 3.14)
# 本项目名
project(base-component-threadpool VERSION 0.1 LANGUAGES CXX)

# 设置C++17标准，需要满足标准，不用C++拓展语法
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CONFIGURATION_TYPES "Debug")
set(CMAKE_C_FLAGS_DEBUG,"O0")
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-std=c++17 -pthread -g -Wall -O0 -fstack-protector -fstack-protector-all)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

set(CMAKE_BUILD_TYPE "Debug")

# 进入test_example文件夹下面的CMakeLists.txt
add_subdirectory(threadpool_example)
add_subdirectory(logger)



