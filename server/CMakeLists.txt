cmake_minimum_required(VERSION 3.10)
project(server)

set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS "-lldap")
SET(CMAKE_CXX_FLAGS "-llber")

add_executable(server myserver.cpp)