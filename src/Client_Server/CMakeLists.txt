cmake_minimum_required(VERSION 3.22)
project(Client_Server)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(Client_Server
        client_network.cpp
        client_network.h
        client_pipe.cpp
        client_pipe.h
        IPC.h
        main.cpp
        network_data.h
        network_ser.cpp
        network_ser.h
        pipe.h
        server_network.cpp
        server_network.h
        server_pipe.cpp
        server_pipe.h)

target_link_libraries(Client_Server wsock32 ws2_32)

