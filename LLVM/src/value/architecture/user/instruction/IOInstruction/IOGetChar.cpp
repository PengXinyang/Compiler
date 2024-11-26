//
// Created by PengXinyang on 24-11-23.
//

#include "../../../../../../include/value/architecture/user/instruction/IOInstruction/IOGetChar.h"

#include "type/irType/IRInt.h"
IOGetChar::IOGetChar():IOInstruction(new IRInt(),"IOGetChar", "IOInstruction"){

}
IOGetChar::IOGetChar(const string &name, const string &instruction_type)
    : IOInstruction(new IRInt(),name, instruction_type){

}

string IOGetChar::getIODefine() {
    return "declare i32 @getchar()";
}

string IOGetChar::toLLVM() {
    return value_name + " = call i32 @getchar()";
}
