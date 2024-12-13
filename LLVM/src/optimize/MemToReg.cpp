//
// Created by PengXinyang on 24-12-10.
//

#include "../../include/optimize/MemToReg.h"
Module* MemToReg::module;
BasicBlock* MemToReg::firstBlock;
vector<Instruction*> MemToReg::loadInstructions;
vector<Instruction*> MemToReg::storeInstructions;
vector<BasicBlock*> MemToReg::useBlocks;
vector<BasicBlock*> MemToReg::defineBlocks;
stack<Value*> MemToReg::phiValueStack;
Instruction* MemToReg::nowAllocaInstruction;