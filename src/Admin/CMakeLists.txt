cmake_minimum_required(VERSION 3.22)
project(Admin)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(Admin
        admin.cpp
        admin.h
        admin_process.cpp
        network_data.h
        )

target_link_libraries(Admin wsock32 ws2_32)
