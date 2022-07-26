cmake_minimum_required(VERSION 3.22)
project(Console_Client_Server_Application_master)

set(CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")

add_executable(Admin
        src/Admin/admin.cpp
        src/Admin/admin.h
        src/Admin/admin_process.cpp
        src/Admin/network_data.h
        )

add_executable(Client_Server
        src/Client_Server/client_network.cpp
        src/Client_Server/client_network.h
        src/Client_Server/client_pipe.cpp
        src/Client_Server/client_pipe.h
        src/Client_Server/IPC.h
        src/Client_Server/main.cpp
        src/Client_Server/network_data.h
        src/Client_Server/pipe.h
        src/Client_Server/server_network.cpp
        src/Client_Server/server_network.h
        src/Client_Server/server_pipe.cpp
        src/Client_Server/server_pipe.h)

target_include_directories(Admin PUBLIC src/Admin)
target_include_directories(Client_Server PUBLIC src/Client_Server)
target_link_libraries(Admin wsock32 ws2_32)