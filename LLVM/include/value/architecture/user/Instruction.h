//
// Created by PengXinyang on 24-11-23.
//

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "value/user/User.h"

class BasicBlock;
class Instruction :public User{
private:
    BasicBlock* blockParent = nullptr;//标记这条指令所在的基本块
protected:
    string instructionType;//指令的名字，比如是add指令还是sub指令,trunk等
public:
    string& getInstructionType(){
        return instructionType;
    }
    void setBlockParent(BasicBlock* currentBlock) {
        blockParent = currentBlock;
    }
    [[nodiscard]] BasicBlock* getBlockParent() const {
        return blockParent;
    }
    Instruction() = default;
    Instruction(IRType* ir_type,const string& name, const string &instructionType);
};



#endif //INSTRUCTION_H
