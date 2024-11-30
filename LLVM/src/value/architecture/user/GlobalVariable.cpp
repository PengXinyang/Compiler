//
// Created by PengXinyang on 24-11-22.
//

#include "../../../../include/value/architecture/user/GlobalVariable.h"

#include "optimize/OptimizerInit.h"
#include "type/IRName.h"

GlobalVariable::GlobalVariable(IRType* ir_type, const string& name, InitVar* init_var, bool is_const_array):GlobalValue(ir_type,name) {
    initVar = init_var;
    this->is_const_array = is_const_array;
    if(!OptimizerInit::isOptimize()) {
        //如果不需要优化，直接添加
        IRName::addGlobalVariable(this);
    }
}

string GlobalVariable::toLLVM() {
    if(is_const_array) return value_name + " = dso_local constant " + initVar->toLLVM();
    return value_name + " = dso_local global " + initVar->toLLVM();
}

void GlobalVariable::toMIPS() {

}
