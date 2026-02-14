cmake_minimum_required(VERSION 4.0)
project(BasicShell C)

set(CMAKE_C_STANDARD 17)

find_library(READLINE_LIBRARY NAMES readline)

add_executable(BasicShell main.c)

target_link_libraries(BasicShell PRIVATE ${READLINE_LIBRARY})
