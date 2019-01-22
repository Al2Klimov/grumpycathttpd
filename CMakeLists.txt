cmake_minimum_required(VERSION 2.6)
project(grumpycathttpd)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

find_package(Boost 1.48.0 COMPONENTS coroutine system REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_executable(grumpycathttpd main.cpp)
target_link_libraries(grumpycathttpd ${Boost_LIBRARIES})
