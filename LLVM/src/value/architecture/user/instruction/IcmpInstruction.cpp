//
// Created by PengXinyang on 24-11-28.
//

#include "../../../../../include/value/architecture/user/instruction/IcmpInstruction.h"

#include "type/irType/IRBool.h"

IcmpInstruction::IcmpInstruction(const string& name, const string& instruction_type, Value *op1, Value *op2)
    : Instruction(new IRBool(),name, instruction_type){
    addOpValue(op1);
    addOpValue(op2);
}

string IcmpInstruction::toLLVM() {
    /*
     * %4 = icmp ne i8 %3, 0
     * %5 = icmp ne i32 %4, %6
     */
    return value_name + " = icmp " + instructionType + " " + opValueChain[0]->value_type->toLLVM()
    + " "+opValueChain[0]->value_name + ", " + opValueChain[1]->value_name;
}
