cmake_minimum_required(VERSION 3.10)

project(Server_part)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SERVER_SOURCES src/server.cpp)

add_executable(server ${SERVER_SOURCES})

target_link_libraries(server ws2_32)

set_target_properties(server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/server
)
