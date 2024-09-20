cmake_minimum_required(VERSION 3.29)
project(Compiler)

set(CMAKE_CXX_STANDARD 17)

add_executable(Compiler main.cpp
        Lexer/src/Lexer.cpp
        Lexer/include/Lexer.h
        Lexer/src/TokenType.cpp
        Lexer/include/TokenType.h)
