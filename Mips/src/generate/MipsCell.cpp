//
// Created by PengXinyang on 24-12-6.
//

#include "../../include/generate/MipsCell.h"
#include <climits>
#include "Register/RegisterController.h"

int MipsCell::stackOffset = 0;
unordered_map<Value*, int> MipsCell::stackOffsetMap;

void MipsCell::moveNowOffset(int offset) {
    stackOffset += offset;
    if(stackOffset>0) {
        printf("stack offset error: %i\n", stackOffset);
    }
}

void MipsCell::resetFunction(Function *function) {
    stackOffset = 0;
    stackOffsetMap = unordered_map<Value*, int>();
    RegisterController::setValueRegisterMap(function->getValueRegisterMap());
}

int MipsCell::getStackOffset() {
    return stackOffset;
}
void MipsCell::addValueOffset(Value *value, int offset) {
    stackOffsetMap.insert(pair<Value*, int>(value, offset));
}
int MipsCell::getValueOffset(Value *value) {
    if(stackOffsetMap.find(value) == stackOffsetMap.end()) {
        //如果没找到，返回int_max，提醒报错
        return INT_MAX;
    }
    return stackOffsetMap[value];
}

vector<Register *> MipsCell::getRegisterDistribute() {
    vector<Register *> registers;
    unordered_map<Value* , Register *> registerMap = RegisterController::getValueRegisterMap();
    registers.reserve(registerMap.size());
    for(auto &[fst, snd] : registerMap) {
        registers.push_back(snd);
    }
    return registers;
}
