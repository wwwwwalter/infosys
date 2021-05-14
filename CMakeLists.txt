cmake_minimum_required(VERSION 3.1.1)
project(CloudServer)

set(CMAKE_CXX_STANDARD 14)
project(CloudServer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall -Wno-unused-variable -Wsign-compare -pthread")

add_executable(CloudServer main.cpp config.h recvmessage.h streamsock.h recvmessage.cpp streamsock.cpp)
