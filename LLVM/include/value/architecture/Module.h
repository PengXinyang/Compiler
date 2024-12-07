//
// Created by PengXinyang on 24-11-23.
//

#ifndef MODULE_H
#define MODULE_H
#include "ConstString.h"
#include "user/Function.h"
#include "user/GlobalValue.h"
#include "user/GlobalVariable.h"
#include "value/Value.h"


class Module :public Value{
private:
    //在模块module中，包含全局变量和函数
    vector<GlobalValue*> globalValues;
    //对于printf的格式字符串，需要单独存
    vector<ConstString*> constantStrings;
public:
    Module();
    //添加一个全局符号
    void addGlobalValue(GlobalValue* globalValue);
    //从全局符号获取所有的全局变量
    vector<GlobalVariable*> getGlobalValues();
    //从全局符号获取所有的函数
    vector<Function*> getFunctions();
    //添加格式字符串
    void addConstantString(ConstString* constantString);
    string toLLVM() override;
    void generateMIPS() override;
};



#endif //MODULE_H
