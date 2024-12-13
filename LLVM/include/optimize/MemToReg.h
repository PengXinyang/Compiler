//
// Created by PengXinyang on 24-12-10.
//

#ifndef MEMTOREG_H
#define MEMTOREG_H
#include <stack>

#include "value/architecture/Module.h"

/**
* 用于处理静态单赋值
* 目的：将只使用了load，store的alloca转成寄存器，减少内存开销
*/
class MemToReg {
private:
    //生成LLVM IR的顶级模块
    static Module* module;
    //存储了每个函数的第一个基本块
    static BasicBlock* firstBlock;
    //存储了所有使用该alloca指令的load指令，以及phi指令
    static vector<Instruction*> loadInstructions;
    //存储了所有使用该alloca指令的store指令，以及phi指令
    static vector<Instruction*> storeInstructions;
    //存储所有使用该指令的基本块
    static vector<BasicBlock*> useBlocks;
    //存储所有定义该指令的基本块
    static vector<BasicBlock*> defineBlocks;
    //存储phi指令的值的栈
    static stack<Value*> phiValueStack;
    //当前的alloca指令
    static Instruction* nowAllocaInstruction;
public:
    static BasicBlock* getFirstBlock();
    static void setFirstBlock(BasicBlock* bb);

    /**
    * 执行Mem2Reg优化的函数
    * @param module
    */
    static void Mem2Reg(Module* module);

    /**
    * 根据指令，重新配置相关参数
    * @param instruction
    */
    static void setByInstruction(Instruction* instruction);

    /**
    * 插入phi指令，要求在每个基本块最前面
    */
    static void insertPhiInstruction();

    /**
    * 深搜遍历基本块，对其中的变量重命名
    * 首先，删除该基本块的不必要指令
    * 然后，遍历基本块的后继，
    * 将最新的define（stack.peek）填充进每个后继块的第一个phi指令
    * 随后进行深搜
    * 将该次dfs时压入的stack数据全部弹出
    * 这时候我们需要有支配树
    * @param basic_block
    */
    static void renameVar(BasicBlock* basic_block);
};



#endif //MEMTOREG_H
