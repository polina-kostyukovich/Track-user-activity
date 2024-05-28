cmake_minimum_required(VERSION 3.25)
project(map_reduce2)

set(CMAKE_CXX_STANDARD 20)

add_executable(external_sort external_sort.cpp)
add_executable(map map.cpp)
add_executable(partition_map partition_map.cpp)
add_executable(reduce1 reduce1.cpp)
add_executable(reduce2 reduce2.cpp)
add_executable(mapreduce mapreduce.cpp)

add_executable(generate generate.cpp)
