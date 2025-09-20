# 
cmake_minimum_required(VERSION 3.16)
project(OrderMatchingEngine VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

option(BUILD_TESTS "Build Unit Tests" ON)

# Main Library
add_subdirectory(src)

# Unit Tests
if(BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()

