//
// Created by PengXinyang on 24-11-23.
//

#include "../../../include/value/architecture/BasicBlock.h"

#include <sstream>

#include "optimize/LLVMOptimizerInit.h"
#include "structure/text/MipsBlock.h"
#include "type/IRName.h"
#include "type/irType/IRBlock.h"
#include "value/architecture/data_structure/Loop.h"
#include "value/architecture/user/Instruction.h"

BasicBlock::BasicBlock(const string& name) :Value(new IRBlock("BasicBlock"),name){
    instructions = vector<Instruction*>();
    is_exist = true;
    if(!LLVMOptimizerInit::isOptimize()) {
        IRName::addBasicBlock(this);
    }
}
void BasicBlock::setExist(bool exist) {
    is_exist = exist;
}
bool BasicBlock::isExist() const {
    return is_exist;
}
Loop *BasicBlock::getLoopParent() const {
    return loopParent;
}
void BasicBlock::setLoopParent(Loop *loop) {
    loopParent = loop;
}
int BasicBlock::getLoopDepth() const {
    return loopParent->getLoopDepth();
}

void BasicBlock::addInstruction(Instruction *instruction, int index) {
    if(index>=0 && index<instructions.size()) {
        instructions.insert(instructions.begin()+index, instruction);
    }else {
        instructions.push_back(instruction);
    }
    instruction->setBlockParent(this);
}
void BasicBlock::setInstructions(const vector<Instruction *> &instructions) {
    this->instructions = instructions;
}
Function *BasicBlock::getFunctionParent() const {
    return functionParent;
}
void BasicBlock::setFunctionParent(Function *function) {
    functionParent = function;
}

string BasicBlock::toLLVM() {
    //基本块的字符串输出
    ostringstream os;
    //首先，将块的名字输出，尤其是在for循环和if-else部分，块号需要输出
    //至于块的名字，计划采用前缀+块号，优化前块号采用全局符号表号
    os<<value_name<<":\n\t";
    const int size = static_cast<int>(instructions.size());
    for(int i = 0; i < size; i++) {
        os<<instructions[i]->toLLVM();
        if(i!=size-1) os<<"\n\t";
    }
    return os.str();
}

void BasicBlock::generateMIPS() {
    new MipsBlock(value_name);
    for(const auto instruction : instructions) {
        instruction->generateMIPS();
    }
}
