//
// Created by PengXinyang on 24-11-28.
//

#include "../../../../../include/value/architecture/user/instruction/BrInstruction.h"

#include "type/irType/IRVoid.h"

BrInstruction::BrInstruction(Value *condition, BasicBlock *thenBlock, BasicBlock *elseBlock)
    :Instruction(new IRVoid(),"BrInstruction","br"){
    this->condition = condition;
    this->thenBlock = thenBlock;
    this->elseBlock = elseBlock;
    addOpValue(condition);
    addOpValue(thenBlock);
    addOpValue(elseBlock);
}

string BrInstruction::toLLVM() {
    return instructionType + " i1 " + condition->value_name + ", label %" +
                thenBlock->value_name + ", label %" + elseBlock->value_name;
}
