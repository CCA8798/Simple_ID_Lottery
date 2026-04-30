cmake_minimum_required(VERSION 3.28)
project(Simple_ID_Lottery)

set(CMAKE_CXX_STANDARD 17)

find_package(CURL REQUIRED)

include_directories(include)
include_directories(libs)

add_executable(Simple_ID_Lottery
        src/main.cpp
        include/jsonxx/jsonxx.cc
)

target_link_libraries(Simple_ID_Lottery
        ${CURL_LIBRARIES}
)

if(WIN32)
    target_link_libraries(Simple_ID_Lottery
            ws2_32 wldap32 crypt32 advapi32
    )
endif()