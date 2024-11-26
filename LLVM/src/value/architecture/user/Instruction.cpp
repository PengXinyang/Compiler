//
// Created by PengXinyang on 24-11-23.
//

#include "../../../../include/value/architecture/user/Instruction.h"

#include "type/IRName.h"

Instruction::Instruction(IRType* ir_type, const string& name, const string &instructionType):User(ir_type,name) {
    this->instructionType = instructionType;
    IRName::addInstruction(this);
}
