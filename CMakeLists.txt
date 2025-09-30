cmake_minimum_required(VERSION 3.22)
project(C_HTTP C)

set(CMAKE_C_STANDARD 11)

add_executable(C_HTTP main.c src/server.c src/client.c)
include_directories(C_HTTP include)
