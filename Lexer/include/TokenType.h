//
// Created by Dell on 24-9-20.
//

#ifndef TOKENTYPE_H
#define TOKENTYPE_H

#include<map>
#include <string>
using namespace std;

class TokenType {
public:
    //构造函数
    TokenType();
    //根据单词token获取对应的类型
    static string getTokenType(const string& token);
    //获取单例模式TokenType
    static TokenType* getTokenTypeInstance();
    //进程退出时释放单实例
    static void deleteTokenTypeInstance();
    //获取map
    static map<string, string> getTokenTypeMap();
private:
    //记录单词类型的map
    static map<string, string> tokenTypeMap;
    // 唯一单实例对象指针
    static TokenType *g_pTokenType;
};

#endif //TOKENTYPE_H
