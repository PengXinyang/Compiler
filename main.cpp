#include "./Lexer/include/FileProcess.h"
#include "./Lexer/include/Lexer.h"
#include<iostream>

#include "Lexer/include/TokenType.h"
using namespace std;
string source;

int main() {
    source = FileProcess::readFileIntoString("testfile.txt");
    TokenType* tokenTypeMain = TokenType::getTokenTypeInstance();
    Lexer *lexer = Lexer::getLexerInstance();
    lexer->setCharPtr(const_cast<char *>(source.c_str()));
    lexer->handleSource();
    lexer->printWrong();
    lexer->printRight();
    return 0;
}
