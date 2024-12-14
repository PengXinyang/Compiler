//
// Created by PengXinyang on 24-11-23.
//

#ifndef FUNCTION_H
#define FUNCTION_H
#include <unordered_map>
#include <unordered_set>

#include "GlobalValue.h"
#include "optimize/data_structure/CfgGraph.h"
#include "structure/Register.h"
#include "value/architecture/BasicBlock.h"
#include "value/architecture/Param.h"
#include "optimize/data_structure/DominantTree.h"

class Param;
class CfgGraph;
class DominantTree;
class Function : public GlobalValue{
private:
    IRType* returnType = nullptr;//函数的返回类型
    vector<BasicBlock*> basicBlocks;//函数内的基本块
    vector<Param*> params;//函数的参数
    //unordered_set<Function*> callFunctions;//这个函数调用了哪些函数
    unordered_map<Value*, Register*> valueRegisterMap;//在函数内部的寄存器和Value的映射表
    CfgGraph* cfgGraph = nullptr;//这个函数对应的控制流图
    DominantTree* dominantTree = nullptr;//这个函数的支配树
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

    //---------------------以下是代码优化部分--------------------------//
    /**
    * 删除无用的代码
    */
    void DCEBlockInstruction();
    /**
    * 生成控制流图
    */
    void buildCfgGraph() const;
    /**
    * 获得控制流图
    */
    CfgGraph* getCfgGraph() const;
    /**
    * 获得支配树
    */
    DominantTree* getDominantTree() const;
    /**
    * 构建支配树中的支配集，支配树，支配边界等
    */
    void buildDominantTree() const;

    /**
    * 插入phi指令
    */
    void insertPhiInstruction() override;
};



#endif //FUNCTION_H
