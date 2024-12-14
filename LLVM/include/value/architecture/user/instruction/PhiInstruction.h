//
// Created by PengXinyang on 24-12-10.
//

#ifndef PHIINSTRUCTION_H
#define PHIINSTRUCTION_H
#include "value/architecture/BasicBlock.h"
#include "value/architecture/user/Instruction.h"

/**
* ϕ函数，用于静态单赋值Mem2Reg使用
*/
class PhiInstruction :public Instruction{
private:
    vector<BasicBlock*> preBlocks;//数据流分析中的前序基本块集合
public:
    PhiInstruction() = default;
    //由于phi函数可能是i32或者i8，所以需要传type
    PhiInstruction(IRType* ir_type, const string& name,const vector<BasicBlock*>&preBlocks, int cnt = -1);
    vector<BasicBlock*> getPreBlocks();
    string toLLVM() override;
    /**
    * 在构造函数阶段，我们向定义使用链插入的是空指针，这肯定不行
    * 所以需要根据传入的Value，修改使用的数据
    */
    void changeValue(Value* value,BasicBlock* basic_block);
};



#endif //PHIINSTRUCTION_H
