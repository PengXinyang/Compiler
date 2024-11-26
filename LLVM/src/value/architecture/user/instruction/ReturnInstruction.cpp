//
// Created by PengXinyang on 24-11-24.
//

#include "../../../../../include/value/architecture/user/instruction/ReturnInstruction.h"

#include "type/irType/IRVoid.h"

ReturnInstruction::ReturnInstruction(Value* return_value)
    :Instruction(new IRVoid(),"ReturnInstruction","ret"){
    if(return_value != nullptr) {
        addOpValue(return_value);
    }
}

Value *ReturnInstruction::getReturnValue() const {
    if(opValueChain.empty()) {
        return nullptr;
    }
    return opValueChain.front();
}

string ReturnInstruction::toLLVM() {
    const Value *return_value = getReturnValue();
    if(return_value == nullptr) {
        return instructionType+" void";
    }
    return instructionType+" "+return_value->value_type->toLLVM() + " "+return_value->value_name;
}
