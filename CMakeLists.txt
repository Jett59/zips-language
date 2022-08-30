cmake_minimum_required(VERSION 3.15)

project(zips CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

bison_target(parser src/parser.y "${CMAKE_CURRENT_BINARY_DIR}/parser.cc")

flex_target(lexer src/lexer.l "${CMAKE_CURRENT_BINARY_DIR}/lexer.cc")

add_executable(zips
src/error.cpp
    src/main.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/lexer.cc"
    "${CMAKE_CURRENT_BINARY_DIR}/parser.cc"
)

target_include_directories(zips PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_BINARY_DIR}"
)
