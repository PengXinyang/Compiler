//
// Created by PengXinyang on 24-11-23.
//

#include "../../../../include/value/architecture/user/Instruction.h"

#include "optimize/LLVMOptimizerInit.h"
#include "optimize/MemToReg.h"
#include "structure/text/MipsInstruction/Annotation.h"
#include "type/IRName.h"

Instruction::Instruction(IRType* ir_type, const string& name, const string &instructionType):User(ir_type,name) {
    this->instructionType = instructionType;
    if(!LLVMOptimizerInit::isOptimize()) {
        IRName::addInstruction(this);
    }
}

void Instruction::generateMIPS() {
    //将指令作为注释生成
    new Annotation(toLLVM());
}

void Instruction::insertPhiInstruction() {
    MemToReg::resetByInstruction(this);
    MemToReg::insertPhiInstruction();
}
