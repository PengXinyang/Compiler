#include "./Lexer/include/FileProcess.h"
#include "./Lexer/include/Lexer.h"
#include<iostream>

#include "GenerateIR.h"
#include "MipsGenerate.h"
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
    //下面是符号表分析程序
    SymbolHandle* symbol_handle = SymbolHandle::getSymbolHandleInstance();
    symbol_handle->handleSymbol();
    //如果有错误，输出错误
    ErrorPrint::printfError();
    GenerateIR* generate_ir = GenerateIR::getInstance();
    if(!ErrorPrint::isError) {
        symbol_handle->printSymbol();
        generate_ir->generateLLVMIR();
        generate_ir->printLLVMIR();
        MipsGenerate* mips_generate = MipsGenerate::getInstance(generate_ir->getModule());
        //MipsGenerate* mips_generate = MipsGenerate::getInstance(new Module());
        mips_generate->generateMips();
        mips_generate->printMips();
    }
    //删除类
    Lexer::deleteLexerInstance();
    TokenType::deleteTokenTypeInstance();
    return 0;
}
