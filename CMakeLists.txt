cmake_minimum_required(VERSION 3.22)
project(C_HTTP CXX)

find_package(fmt)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES main.cpp src/server.cpp src/client.cpp src/udp_yapper.cpp src/udp_listener.cpp src/connection.cpp src/netutils.cpp src/request.cpp
    src/response.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})
add_compile_definitions(LFS_LOG_LEVEL=5)
target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt)
target_include_directories(${PROJECT_NAME} PUBLIC include)
