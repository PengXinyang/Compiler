//
// Created by PengXinyang on 24-11-23.
//

#include "../../../../../../include/value/architecture/user/instruction/IOInstruction/IOPutStr.h"

#include "type/irType/IRVoid.h"

IOPutStr::IOPutStr(ConstString* constString)
    : IOInstruction(new IRVoid,"IOPutSt","IOInstruction"){
    this->constString = constString;
}

string IOPutStr::getIODefine() {
    return "declare void @putstr(i8*)";
}

string IOPutStr::toLLVM() {
    /*
     * 示例：
     * call void @putstr(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.1, i64 0, i64 0))
     */
    string const_sting_name = constString->value_name;
    IRPointer* const_string_pointer = constString->getIRPointer();
    return "call void @putstr(i8* getelementptr inbounds ("+
            const_string_pointer->ir_type()->toLLVM() + ", "+
            const_string_pointer->toLLVM() + " " +
            const_sting_name + ", i64 0, i64 0))";
}
