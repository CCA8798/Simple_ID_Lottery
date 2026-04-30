cmake_minimum_required(VERSION 3.31)
project(Simple_ID_Lottery)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(Simple_ID_Lottery src/main.cpp
        include/jsonxx/jsonxx.cc)
