//
// Created by PengXinyang on 24-12-18.
//

#ifndef REGISTERDISTRIBUTE_H
#define REGISTERDISTRIBUTE_H
#include <vector>

#include "structure/Register.h"
#include "value/Value.h"
#include "value/architecture/BasicBlock.h"
#include "value/architecture/user/Instruction.h"

/**
* 处理寄存器分配的问题
*/
class RegisterDistribute {
private:
    //当前分配器可以分配的寄存器，这里使用t0-s7寄存器
    static vector<Register*> registers;
    //保存value和寄存器的映射
    static unordered_map<Value*, Register*> ValueToRegisterMap;
    //保存寄存器和value的映射
    static unordered_map<Register*, Value*> RegisterToValueMap;
public:
    /**
    * 初始化可以使用的寄存器： t0-s7
    * t8和t9用于没有寄存器可分配的时候，进行分配
    */
    static void init();
    static void setValueToRegisterMap(unordered_map<Value*, Register*>& valueToRegisterMap);
    static void setRegisterToValueMap(unordered_map<Register*, Value*>& registerToValueMap);
    static unordered_map<Value*, Register*>& getValueToRegisterMap();

private:
    /**
    * 遍历寄存器列表，找到一个尚未分配的寄存器
    * @return
    */
    static Register* distributeRegister();

    /**
    * 释放寄存器
    * 如果该指令的某个op是该基本块内最后一次引用，并且out不包含这个value
    * 说明作用域已经结束，可以释放
    * 如果是定义语句，那么需要分配一个寄存器
    */
    static void releaseRegister(BasicBlock* block, Instruction* instruction,
        unordered_map<Value*, Value*>& );

};



#endif //REGISTERDISTRIBUTE_H
