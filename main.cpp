#include "./Lexer/include/FileProcess.h"
#include "./Lexer/include/Lexer.h"
#include<iostream>

#include "GenerateIR.h"
#include "ErrorHandle/include/ErrorPrint.h"
#include "Lexer/include/TokenType.h"
#include "Parser/include/Parser.h"
#include "Symbol/include/SymbolHandle.h"
using namespace std;
string source;

int main() {
    source = FileProcess::readFileIntoString("testfile.txt");
    //下面是词法分析处理程序
    TokenType* tokenTypeMain = TokenType::getTokenTypeInstance();
    Lexer *lexer = Lexer::getLexerInstance();
    lexer->setCharPtr(const_cast<char *>(source.c_str()));
    lexer->handleSource();
    //下面是语法分析处理程序
    Parser* parser = Parser::getParserInstance();
    parser->ParserHandle();
    SymbolHandle* symbol_handle = SymbolHandle::getSymbolHandleInstance();
    symbol_handle->handleSymbol();
    ErrorPrint::printfError();
    GenerateIR* generate_ir = GenerateIR::getInstance();
    generate_ir->generateLLVMIR();
    if(!ErrorPrint::isError) {
        symbol_handle->printSymbol();
        generate_ir->printLLVMIR();
    }
    //删除类
    Lexer::deleteLexerInstance();
    TokenType::deleteTokenTypeInstance();
    return 0;
}
