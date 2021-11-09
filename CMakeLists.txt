cmake_minimum_required(VERSION 3.16)
project(Assignment_1)

set(CMAKE_CXX_STANDARD 14)
link_libraries(ws2_32 wsock32)

add_executable(Assignment_1_server serverMain.cpp Parser.cpp Parser.h Server.cpp Server.h HTTPBuilder.cpp HTTPBuilder.h)
add_executable(Assignment_1_client clientMain.cpp Parser.cpp Parser.h Client.cpp Client.h HTTPBuilder.cpp HTTPBuilder.h)
