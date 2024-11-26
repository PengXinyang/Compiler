//
// Created by PengXinyang on 24-11-24.
//

#include "../../../../../include/value/architecture/user/instruction/AllocaInstruction.h"

#include "type/irType/IRPointer.h"

AllocaInstruction::AllocaInstruction(IRType *ir_type, const string& name)
    :Instruction(new IRPointer(ir_type),name,"alloca"){
    this->ir_type = ir_type;
}

string AllocaInstruction::toLLVM() {
    return value_name + " = " + instructionType + " " + ir_type->toLLVM();
}
