//
// Created by PengXinyang on 24-12-10.
//

#include "../../../../../include/value/architecture/user/instruction/PhiInstruction.h"

#include <algorithm>

#include "value/architecture/BasicBlock.h"

PhiInstruction::PhiInstruction(IRType* ir_type, const string& name, const vector<BasicBlock *>& preBlocks, int cnt)
    :Instruction(ir_type,name,"phi"){
    this->preBlocks = preBlocks;
    const int size = cnt == -1? static_cast<int>(preBlocks.size()):cnt;
    int num = size - static_cast<int>(preBlocks.size());
    addOpValue(nullptr,size);
    for(int i=0;i<num;i++) {
        this->preBlocks.push_back(nullptr);
    }
}

vector<BasicBlock *> PhiInstruction::getPreBlocks() {
    return preBlocks;
}

string PhiInstruction::toLLVM() {
    stringstream ss;
    /*
    * 示例：
    * %7 = phi i32 [1, %3], [%5, %4]
    */
    ss<<value_name<<" = "<<instructionType<<" "<<value_type<<" ";
    const int size = static_cast<int>(preBlocks.size());
    for(int i=0;i<size;i++) {
        if(opValueChain[i]&&preBlocks[i]) {
            ss<<"["<<opValueChain[i]<<", %"<<preBlocks[i]->value_name<<"]";
        }
        if(i+1<size) {
            ss<<", ";
        }
    }
    return ss.str();
}

void PhiInstruction::changeValue(Value *value, BasicBlock *basic_block) {
    const auto it = find(preBlocks.begin(),preBlocks.end(),basic_block);
    const int index = it - preBlocks.begin();
    opValueChain[index] = value;
    //由于初始化时，添加了null，现在需要将这个value和phi绑定
    value->addUser(this);
}
