cmake_minimum_required(VERSION 3.10)

project(map_reduce_server LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Network REQUIRED)

set(map_reduce_server_SRCS
    src/main.cpp
    src/server.cpp
    src/worker.cpp
    src/support.cpp
    src/json.hpp
    src/ThreadTcpSocket.cpp
)

add_executable(${PROJECT_NAME} ${map_reduce_server_SRCS})

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Network)
