//
// Created by PengXinyang on 24-11-24.
//

#include "../../../../../include/value/architecture/user/instruction/StoreInstruction.h"

#include "type/irType/IRVoid.h"

StoreInstruction::StoreInstruction(Value* value, Value* pointer)
    : Instruction(new IRVoid(),"StoreInstruction","store"){
    //使用的操作数有两个
    addOpValue(value);
    addOpValue(pointer);
}

string StoreInstruction::toLLVM() {
    //store i32 %2, i32* %1
    ostringstream os;
    //store
    os<<instructionType<<" ";
    //i32 %2
    os<<opValueChain[0]->value_type->toLLVM()<<" "<<opValueChain[0]->value_name;
    os<<", ";
    //i32* %1
    os<<opValueChain[1]->value_type->toLLVM()<<" "<<opValueChain[1]->value_name;
    return os.str();
}
