cmake_minimum_required(VERSION 3.31)
project(Simple_ID_Lottery)

set(CMAKE_CXX_STANDARD 17)
add_compile_definitions(CURL_STATICLIB)

# 通过设置环境变量，为 FindCURL 模块指明路径
set(ENV{CURL_ROOT} "${CMAKE_CURRENT_SOURCE_DIR}/include/curl")
# 使用模块模式，去掉 CONFIG 关键词
find_package(CURL REQUIRED)

# 其他配置保持不变
include_directories(include)
include_directories(libs)
add_executable(Simple_ID_Lottery src/main.cpp include/jsonxx/jsonxx.cc)

target_link_libraries(Simple_ID_Lottery PRIVATE
        CURL::libcurl
        ws2_32 wldap32 crypt32 advapi32
)