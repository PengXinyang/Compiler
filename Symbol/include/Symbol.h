//
// Created by PengXinyang on 24-10-12.
//

#ifndef SYMBOL_H
#define SYMBOL_H
#include <memory>
#include<string>
#include <unordered_map>
#include <vector>
#include<cstdio>
struct Word;
using namespace std;

//将类型与字符串对应
inline unordered_map<int,string> int_to_btypeMap = {
    {-1,"Error"},
    {0,"Int"},
    {1,"Char"}
};
inline unordered_map<string,int> btype_to_intMap = {
    {"Error",-1},
    {"Int",0},
    {"Char",1}
};
inline unordered_map<int,string> int_to_typeMap = {
    {-1,"Error"},
    {0,"Var"},
    {1,"Array"},
    {2,"Func"}
};
inline unordered_map<string,int> type_to_intMap = {
    {"Error",-1},
    {"Var",0},
    {"Array",1},
    {"Func",2}
};
inline unordered_map<int,string> int_to_func_typeMap = {
    {-1,"Error"},
    {0,"Int"},
    {1,"Char"},
    {2,"Void"}
};
inline unordered_map<string,int> func_type_to_intMap = {
    {"Error",-1},
    {"Int",0},
    {"Char",1},
    {"Void",2}
};

//符号
class Symbol {
public:
    int symbol_id{};//符号的id
    int table_id{};//符号所在的符号表的id
    string token;//当前单词对应的字符串
    int lineNum{};//当前符号所在的行
    int btype = -1;//是什么数据类型，0是int 1是char
    int type = -1;//是什么类型，0是var，1是数组，2是函数
    bool is_const = false;//是不是常量
    bool is_func = false;//是不是函数
    bool is_func_param = false;//是不是函数的参数
    bool is_var = false;//是不是单元素变量
    bool is_array = false;//是不是数组
    //函数是否有返回值，这个变量是代码运行过程中逐渐改变的，用于判断f,g类错误
    bool is_return = false;
    int dim = 0;//代表数组的元素个数，如果是数组就启用
    vector<int> array_int_values;//如果是数组，用于记录数组的值
    vector<char> array_char_values;//如果是char数组，按照字符串记录即可
    int int_var_value = 0;//如果是变量，记录变量的值
    char char_var_value = 0;//字符型变量，记录变量的值
    int reg = 1;//寄存器是哪个，代码优化的时候再用
    int func_type = -1;//是函数的时候再启用，0 int 1 char 2 void
    int param_num = 0;//如果是函数，那么后面的参数数量
    vector<int> param_type;//函数形参的类型，0是变量，1是int数组，2是char数组
    int func_value = -1;//如果函数有返回值，则设置这个
    Symbol();
};

//符号表
class SymbolTable {
public:
    int table_id = 1;//符号表id，有一个全局的数字统计。初始全局符号表id是1
    int layer = 1;//现在位于第几层。父符号表层数+1，初始时全局符号表位于第1层s
    shared_ptr<SymbolTable> father_ptr = nullptr;//父符号表，默认根节点的父亲为空指针
    vector<shared_ptr<SymbolTable>> children_ptr; // 子符号表
    unordered_map<string,Symbol> symbol_table;//符号表有哪些符号
    vector<string> token_vector;//将插入的符号单词按照插入顺序计入，便于输出时按照顺序输出
    SymbolTable();
    SymbolTable(int table_id, const shared_ptr<SymbolTable> &father_ptr);
    void print_symbol_table(FILE* fp);//输出符号表，fp是文件指针
    void print_all_symbol_table(FILE* fp);//输出符号表和子符号表，递归
    bool is_in_table(const string& token);//判断单词是否在当前符号表中
    bool is_in_all_table(const string& token);//递归判断单词是否在当前或父符号表中
    Symbol* get_symbol(const string& token);//获取当前符号表token对应的符号
    Symbol* get_symbol_in_all_table(const string& token);//在所有符号表遍历，寻找符号
    Symbol* get_last_symbol();//获取最后一个符号
    bool is_in_func_table(const string& token);//判断单词是否在符号表中，且是否为函数
    bool is_in_var_table(const string& token);//判断单词是否在符号表中，且是否为变量
    bool is_in_array_table(const string& token);//判断单词是否在符号表中，且是否为数组
    bool is_const(const string& token);//判断符号是不是常量，这个是涉及多层的
    void add_symbol(const string& token, const Symbol &symbol);//将一个构造好的符号表添加入表中
    void add_var_symbol(const Word& word,int btype,int var_value,bool is_const);//将变量/常量符号添加入符号表中
    void add_func_symbol(const Word& word,int func_type,int param_num, const vector<int> &param_type);//将函数名添加入符号表
    void add_func_param_symbol(const Word& word,int btype,bool is_array);//添加函数参数
    void add_array_symbol(const Word& word,int type,int dim, bool is_const);//添加数组符号
    void set_var_value(const string& token,int var_value);
    void set_int_array_value(const string& token, const vector<int> &array_value);
    void set_int_array_value(const string& token, int array_value);//添加一个数字
    void set_char_array_value(const string& token, const vector<char> &array_value);
    void set_char_array_value(const string& token, char array_value);//添加一个字符
    void set_array_dim(const string &token, int dim);//设置数组的维度
    //设置函数是否return
    void set_is_return(const string& token, bool is_return);
    void set_is_return(Symbol* func_symbol,bool is_return);
    void set_func_return(const string& token,int func_value);
    void set_func_return(Symbol* func_symbol,int func_value);
};

#endif //SYMBOL_H
