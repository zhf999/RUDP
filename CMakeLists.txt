cmake_minimum_required(VERSION 3.27)
project(RUDP)

set(CMAKE_CXX_STANDARD 17)


include_directories(include)
add_library(libRUDP src/client/RUDP_client.cpp
    src/server/RUDP_server.cpp
    src/RUDP.cpp
    src/utils.cpp)

link_libraries(libRUDP)

add_executable(RUDP src/main.cpp)
add_executable(ClientTest src/client/ClientTest.cpp)
add_executable(ServerTest src/server/ServerTest.cpp)