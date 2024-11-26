//
// Created by PengXinyang on 24-11-23.
//

#ifndef BASICBLOCK_H
#define BASICBLOCK_H
#include "data_structure/Loop.h"
#include "value/Value.h"

class Function;
class Instruction;
//基本块类，一个函数有多个基本块，至少有一个
class BasicBlock :public Value{
    //一个基本块内有多个指令,且不能包括跳转和返回指令。
    //一个基本块内的指令，要么都执行，要么都不执行
    //先不考虑代码优化
private:
    vector<Instruction*> instructions;//多条指令按照先后顺序存入数组
    Function* functionParent = nullptr;//这个基本块属于哪个函数
    bool is_exist = true;//基本块是否还存在
    Loop* loopParent = nullptr;//这个基本块是不是某个循环下的
public:
    BasicBlock() = default;
    explicit BasicBlock(const string& name);
    //添加一条指令，如果index在当前vector范围内则插入到这个地方；否则添加在最后
    void addInstruction(Instruction* instruction,int index = -1);
    //判断指令集是否为空
    [[nodiscard]] bool isEmpty() const {
        return instructions.empty();
    }
    //获取最后一条指令
    [[nodiscard]] Instruction* getLastInstruction() const {
        return instructions.back();
    }
    //设置指令集
    void setInstructions(const vector<Instruction*>& instructions);
    //设置exist状态
    void setExist(bool exist);
    //获取exist状态
    [[nodiscard]] bool isExist() const;
    //设置属于哪个循环块
    void setLoopParent(Loop* loop);
    //获取当前属于哪个循环块
    [[nodiscard]] Loop* getLoopParent() const;
    //当前在循环第几层深度？
    [[nodiscard]] int getLoopDepth() const;
    //设置当前属于哪个函数
    void setFunctionParent(Function* function);
    //获取当前块属于哪个函数
    [[nodiscard]] Function* getFunctionParent() const;
    //覆写，基本块应当如何输出
    string toLLVM() override;
    //生成汇编，之后再补

};



#endif //BASICBLOCK_H
