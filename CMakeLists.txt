cmake_minimum_required(VERSION 3.15)
project(order_book_engine VERSION 0.1 LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# C++17 and optimizations
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -g")

# Enable CTest
include(CTest)
enable_testing()

find_package(Boost REQUIRED)
set(BOOST_INC ${Boost_INCLUDE_DIRS})

# Build modules
add_subdirectory(src)
add_subdirectory(tests)
