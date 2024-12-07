//
// Created by PengXinyang on 24-11-23.
//

#ifndef FUNCTION_H
#define FUNCTION_H
#include <unordered_map>
#include <unordered_set>

#include "GlobalValue.h"
#include "structure/Register.h"
#include "value/architecture/BasicBlock.h"
#include "value/architecture/Param.h"

class Param;
class Function : public GlobalValue{
private:
    IRType* returnType = nullptr;//函数的返回类型
    vector<BasicBlock*> basicBlocks;//函数内的基本块
    vector<Param*> params;//函数的参数
    //unordered_set<Function*> callFunctions;//这个函数调用了哪些函数
    unordered_map<Value*, Register*> valueRegisterMap;//在函数内部的寄存器和Value的映射表
public:
    Function() = default;
    Function(const string& name, IRType* returnType);
    [[nodiscard]] IRType* getReturnType() const {
        return returnType;
    }
    vector<BasicBlock *>& getBasicBlocks(){
        return basicBlocks;
    }
    vector<Param *>& getParams(){
        return params;
    }
    [[nodiscard]] unordered_map<Value *, Register *>& getValueRegisterMap() {
        return valueRegisterMap;
    }
    //添加函数使用的参数
    void addParam(Param *param);
    //添加函数包括的基本块，index表示添加基本块到哪个位置
    void addBasicBlock(BasicBlock *basicBlock,int index = -1);
    string toLLVM() override;
    void generateMIPS() override;
};



#endif //FUNCTION_H
