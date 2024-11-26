//
// Created by PengXinyang on 24-11-24.
//

#include "../../../../../include/value/architecture/user/instruction/TruncInstruction.h"

TruncInstruction::TruncInstruction(const string &name, Value *value, IRType *final_type)
    : Instruction(final_type, name, "trunc"){
    addOpValue(value);
    this->final_type = final_type;
}

string TruncInstruction::toLLVM() {
    //%10 = trunc i32 %9 to i8
    return value_name+" = "+instructionType+" "
    + opValueChain[0]->value_type->toLLVM() + " " + opValueChain[0]->value_name
    + " to " + final_type->toLLVM();
}
