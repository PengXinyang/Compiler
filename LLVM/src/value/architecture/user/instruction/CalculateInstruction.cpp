//
// Created by PengXinyang on 24-11-24.
//

#include "../../../../../include/value/architecture/user/instruction/CalculateInstruction.h"

#include "value/architecture/ConstValue.h"

CalculateInstruction::CalculateInstruction(IRType* ir_type, const string& name, const string& instruction_type, Value *v1, Value *v2)
    : Instruction(ir_type,name, instruction_type){
    addOpValue(v1);
    addOpValue(v2);
}

string CalculateInstruction::toLLVM() {
    ostringstream oss;
    //%9 = add nsw i32 %5, %8
    oss<<value_name+" = ";//%9
    oss<<instructionType<<" ";//"add nsw"，nsw表示假定不会溢出；如果溢出就是未定义行为
    oss<<value_type->toLLVM()<<" ";//i32
    oss<<opValueChain[0]->value_name<<", "<<opValueChain[1]->value_name;//%5,%8
    return oss.str();
}

int CalculateInstruction::getConstNum() const {
    int is_const0 = 0,is_const1 = 0;
    if(instanceof<ConstValue>(opValueChain[0])) is_const0 = 1;
    if(instanceof<ConstValue>(opValueChain[1])) is_const1 = 1;
    return is_const0 + is_const1;
}
