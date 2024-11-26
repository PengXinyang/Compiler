//
// Created by PengXinyang on 24-11-23.
//

#include "../../../../../../include/value/architecture/user/instruction/IOInstruction/IOGetInt.h"

#include "type/irType/IRInt.h"
IOGetInt::IOGetInt():IOInstruction(new IRInt(),"IOGetInt", "IOInstruction"){

}
IOGetInt::IOGetInt(const string& name, const string& instruction_type)
    : IOInstruction(new IRInt(),name, instruction_type){

}

string IOGetInt::getIODefine() {
    return "declare i32 @getint()";
}

string IOGetInt::toLLVM() {
    return value_name + " = call i32 @getint()";
}
