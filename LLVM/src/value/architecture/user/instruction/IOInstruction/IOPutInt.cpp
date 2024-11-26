//
// Created by PengXinyang on 24-11-23.
//

#include "../../../../../../include/value/architecture/user/instruction/IOInstruction/IOPutInt.h"

#include "type/irType/IRVoid.h"

IOPutInt::IOPutInt(Value* op_value)
    : IOInstruction(new IRVoid(),"IOPutInt", "IOInstructor"){
    addOpValue(op_value);
}

string IOPutInt::getIODefine() {
    return "declare void @putint(i32)";
}

string IOPutInt::toLLVM() {
    return "call void @putint(i32 " + opValueChain[0]->value_name + ")";
}