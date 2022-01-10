cmake_minimum_required(VERSION 3.10)

project(WebSocketCalc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED False)

add_executable(srv main.cpp)

target_include_directories(srv PRIVATE ext/include)

target_compile_definitions(srv PRIVATE ASIO_STANDALONE)

target_compile_options(srv PRIVATE -Wall -Werror)

target_link_libraries(srv pthread ssl crypto)