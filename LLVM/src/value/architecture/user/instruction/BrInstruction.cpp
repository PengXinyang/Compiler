//
// Created by PengXinyang on 24-11-28.
//

#include "../../../../../include/value/architecture/user/instruction/BrInstruction.h"

#include "type/irType/IRVoid.h"

BrInstruction::BrInstruction(Value *condition, BasicBlock *ifTrueBlock, BasicBlock *ifFalseBlock)
    :Instruction(new IRVoid(),"BrInstruction","br"){
    this->condition = condition;
    this->ifTrueBlock = ifTrueBlock;
    this->ifFalseBlock = ifFalseBlock;
    addOpValue(condition);
    addOpValue(ifTrueBlock);
    addOpValue(ifFalseBlock);
}

string BrInstruction::toLLVM() {
    return instructionType + " i1 " + condition->value_name + ", label %" +
                ifTrueBlock->value_name + ", label %" + ifFalseBlock->value_name;
}
