cmake_minimum_required(VERSION 3.26)

project(CHAT)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 公共代码
set(COMMON_SRC
    src/Socket.cpp
)

# server
add_executable(server
    src/server.cpp
    ${COMMON_SRC}
)

# client
add_executable(client
    src/client.cpp
)