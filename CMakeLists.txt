cmake_minimum_required(VERSION 3.28.3)
project(Compiler)

set(CMAKE_CXX_STANDARD 17)

add_executable(Compiler main.cpp
        Lexer/src/Lexer.cpp
        Lexer/include/Lexer.h
        Lexer/src/TokenType.cpp
        Lexer/include/TokenType.h
        Lexer/src/FileProcess.cpp
        Lexer/include/FileProcess.h
        Parser/src/TokenScanner.cpp
        Parser/include/TokenScanner.h
        Parser/src/Parser.cpp
        Parser/include/Parser.h
        ErrorHandle/src/ErrorCategory.cpp
        ErrorHandle/include/ErrorCategory.h
        ErrorHandle/src/ErrorPrint.cpp
        ErrorHandle/include/ErrorPrint.h
        Parser/src/ParserTree.cpp
        Parser/include/ParserTree.h
)
