cmake_minimum_required(VERSION 3.29)
project(P2)

set(CMAKE_CXX_STANDARD 20)

add_executable(P2
        MsgStream.cpp
        MsgStream.h
        PartitionStream.cpp
        PartitionStream.h
        DurableStream.h
        DurableStream.cpp
        P4.cpp)
