//
// Created by PengXinyang on 24-11-23.
//

#include "../../../include/value/architecture/ConstString.h"

#include <regex>

#include "type/IRName.h"
#include "type/irType/IRArray.h"
#include "type/irType/IRChar.h"

ConstString::ConstString(const string& name, const string& str, const vector<int>& dims)
    :Value(
        new IRPointer(
            new IRArray(new IRChar(),dims)
        ),name){
    this->str = str;
    IRName::addConstString(this);
}

IRPointer *ConstString::getIRPointer() const {
    return dynamic_cast<IRPointer*>(value_type);
}

string ConstString::getString() const {
    return str;
}

string ConstString::toLLVM() {
    /**
     * 示例：
     * printf("b is \n%d",b);
     * @.str = private unnamed_addr constant [9 x i8] c"b is \0A%d\00", align 1
     */
    string type_st = getIRPointer()->ir_type()->toLLVM();
    string place_str = regex_replace(str,regex("\n"),"\\0A");
    return value_name+ " = private unnamed_addr constant " + type_st + " c\"" + place_str + "\\00\"";
}
