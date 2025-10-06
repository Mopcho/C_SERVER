cmake_minimum_required(VERSION 3.22)
project(C_HTTP C)

set(CMAKE_C_STANDARD 11)

set(SOURCES main.c src/pollfds_dynamic.c src/server.c src/client.c src/udp_yapper.c src/udp_listener.c src/connection.c src/server_context.c)
add_executable(${PROJECT_NAME} ${SOURCES})
include_directories(${PROJECT_NAME} include)
add_compile_definitions(lfs_LOG_LEVEL=5)
