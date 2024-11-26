//
// Created by PengXinyang on 24-11-23.
//

#include "../../../../../../include/value/architecture/user/instruction/IOInstruction/IOPutCh.h"

#include "type/irType/IRVoid.h"

IOPutCh::IOPutCh(Value* op_value)
    : IOInstruction(new IRVoid(),"IOPutCh", "IOInstruction"){
    addOpValue(op_value);
}

string IOPutCh::getIODefine() {
    return "declare void @putch(i32)";
}

string IOPutCh::toLLVM() {
    return "call void @putch(i32 " + opValueChain[0]->value_name + ")";
}