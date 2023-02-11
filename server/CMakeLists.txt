# cmake 最低版本需求
cmake_minimum_required(VERSION 3.13)

# 工程名称
project(IMserver)

# 设置编译选项
set(CXX_FLAGS
    -g
    -Wall
    -std=c++17
    )

include_directories(
    ${PROJECT_SOURCE_DIR}/include/base
    ${PROJECT_SOURCE_DIR}/include/http
    ${PROJECT_SOURCE_DIR}/include/mysql
    ${PROJECT_SOURCE_DIR}/include/net
    ${PROJECT_SOURCE_DIR}/include/redis
    ${PROJECT_SOURCE_DIR}/include/timer
    )
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib)

add_executable(IMserver chat_server.cpp)
target_link_libraries(IMserver libmyNetworkLibrary.so)