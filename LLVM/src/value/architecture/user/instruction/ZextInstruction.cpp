//
// Created by PengXinyang on 24-11-24.
//

#include "../../../../../include/value/architecture/user/instruction/ZextInstruction.h"

ZextInstruction::ZextInstruction(const string& name, Value *value, IRType *final_type)
    : Instruction(final_type, name, "zext"){
    this->final_type = final_type;
    addOpValue(value);
}

string ZextInstruction::toLLVM() {
    //%12 = zext i8 %11 to i32
    return value_name+" = "+instructionType+" "
    + opValueChain[0]->value_type->toLLVM() + " " + opValueChain[0]->value_name
    + " to " + final_type->toLLVM();
}
