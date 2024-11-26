//
// Created by PengXinyang on 24-11-23.
//

#include "../../../include/value/architecture/Module.h"

#include <sstream>

#include "instanceof.h"
#include "type/irType/IRBlock.h"
#include "value/architecture/user/instruction/IOInstruction/IOGetChar.h"
#include "value/architecture/user/instruction/IOInstruction/IOGetInt.h"
#include "value/architecture/user/instruction/IOInstruction/IOPutCh.h"
#include "value/architecture/user/instruction/IOInstruction/IOPutInt.h"
#include "value/architecture/user/instruction/IOInstruction/IOPutStr.h"

Module::Module() :Value(new IRBlock("module"),"main_module"){
    globalValues = vector<GlobalValue*>();
    constantStrings = vector<ConstString*>();
}
void Module::addGlobalValue(GlobalValue *globalValue) {
    globalValues.push_back(globalValue);
}

vector<GlobalVariable *> &Module::getGlobalValues() {
    vector<GlobalVariable *> global_variables = vector<GlobalVariable *>();
    for (auto global_value : globalValues) {
        //TODO: 需要检查这个instanceof是否可以正常使用
        if(instanceof<GlobalVariable>(global_value)) {
            global_variables.push_back(dynamic_cast<GlobalVariable *>(global_value));
        }
    }
    return global_variables;
}

vector<Function *> &Module::getFunctions() {
    auto functions = vector<Function *>();
    for (auto global_value : globalValues) {
        //TODO: 需要检查这个instanceof是否可以正常使用
        if(instanceof<Function>(global_value)) {
            functions.push_back(dynamic_cast<Function *>(global_value));
        }
    }
    return functions;
}

void Module::addConstantString(ConstString *constantString) {
    constantStrings.push_back(constantString);
}

string Module::toLLVM() {
    ostringstream os;
    //首先，添加输入输出函数的定义语句
    os<<IOGetInt::getIODefine()<<"\n";
    os<<IOGetChar::getIODefine()<<"\n";
    os<<IOPutInt::getIODefine()<<"\n";
    os<<IOPutCh::getIODefine()<<"\n";
    os<<IOPutStr::getIODefine()<<"\n\n";
    //其次，添加format语句
    for(const auto constantString : constantStrings) {
        os << constantString->toLLVM() << "\n";
    }
    os<<"\n";
    //最后，添加全局符号语句，包括全局变量和函数，理论上是按照出现的先后顺序
    for(const auto global_value : globalValues) {
        os << global_value->toLLVM() << "\n";
    }
    return os.str();
}
