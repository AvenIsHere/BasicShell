cmake_minimum_required(VERSION 3.10)
project(BShell CXX)

set(CMAKE_CXX_STANDARD 20)

if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/usr/local/opt/readline" "/opt/homebrew/opt/readline")
endif()

find_library(READLINE_LIBRARY NAMES readline REQUIRED)

add_executable(BShell main.cpp
        config.cpp
        config.h)

target_link_libraries(BShell PRIVATE ${READLINE_LIBRARY})
