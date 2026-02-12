cmake_minimum_required(VERSION 4.0)
project(PartA2 C)

set(CMAKE_C_STANDARD 17)

find_library(READLINE_LIBRARY NAMES readline)

add_executable(PartA2 main.c)

target_link_libraries(PartA2 PRIVATE ${READLINE_LIBRARY})
