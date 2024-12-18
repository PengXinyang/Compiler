//
// Created by PengXinyang on 24-12-18.
//

#include "../../include/Register/RegisterDistribute.h"

vector<Register*> RegisterDistribute::registers;
unordered_map<Value*, Register*> RegisterDistribute::ValueToRegisterMap;
unordered_map<Register*, Value*> RegisterDistribute::RegisterToValueMap;

void RegisterDistribute::init() {
    for(auto rn = RegisterName::$t0; rn < RegisterName::$t0; rn = rn + 1) {
        registers.push_back(Register::getRegister(rn));
    }
}

void RegisterDistribute::setValueToRegisterMap(unordered_map<Value *, Register *> &valueToRegisterMap) {
    ValueToRegisterMap = valueToRegisterMap;
}
void RegisterDistribute::setRegisterToValueMap(unordered_map<Register *, Value *> &registerToValueMap) {
    RegisterToValueMap = registerToValueMap;
}
unordered_map<Value *, Register *> &RegisterDistribute::getValueToRegisterMap() {
    return ValueToRegisterMap;
}

Register *RegisterDistribute::distributeRegister() {
    for(auto reg:registers) {
        if(RegisterToValueMap.find(reg) != RegisterToValueMap.end() && RegisterToValueMap[reg]) {
            //说明找到了寄存器，直接返回
            return reg;
        }
    }
    //没有找到，返回null
    return nullptr;
}
