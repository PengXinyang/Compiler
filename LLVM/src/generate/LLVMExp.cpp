//
// Created by PengXinyang on 24-11-25.
//

#include "generate/LLVMExp.h"

#include <iostream>

#include "generate/LLVMGenerate.h"
#include "type/IRName.h"
#include "type/irType/IRChar.h"
#include "type/irType/IRInt.h"
#include "value/architecture/ConstValue.h"
#include "value/architecture/user/instruction/AllocaInstruction.h"
#include "value/architecture/user/instruction/CalculateInstruction.h"
#include "value/architecture/user/instruction/GetelementptrInstruction.h"
#include "value/architecture/user/instruction/LoadInstruction.h"
#include "value/architecture/user/instruction/StoreInstruction.h"
#include "value/architecture/user/instruction/TruncInstruction.h"
#include "value/architecture/user/instruction/IOInstruction/IOGetChar.h"
#include "value/architecture/user/instruction/IOInstruction/IOGetInt.h"

class IRArray;

LLVMExp::LLVMExp(LLVMGenerate *llvmGenerate) {
    this->llvmGenerate = llvmGenerate;
    now_symbol_table = llvmGenerate->now_symbol_table;
}

Value *LLVMExp::generateAddressLLVM(TreeNode* AstRoot) {
    vector<Value*> args;
    for(auto son:AstRoot->sonNode) {
        if(son->word.word=="<Exp>") {
            //只需要先分析Exp子节点，如果有，这就是数组
            args.push_back(llvmGenerate->generateLLVMIR(son));
        }
    }
    //获取第一个子节点，LVal的符号Ident
    Symbol* lval_symbol = now_symbol_table->get_symbol_in_all_table(AstRoot->sonNode[0]->word.word, AstRoot->sonNode[0]->word.line_num);
    if(lval_symbol!=nullptr) {
        if(lval_symbol->is_var) {
            //说明是变量
            return lval_symbol->value;
        }
        if(lval_symbol->is_array) {
            //说明是取的数组值
            return new GetelementptrInstruction(
                lval_symbol->get_ir_type(),
                IRName::getLocalVariableName(),
                lval_symbol->value,
                args[0]
                );
        }
    }
    return nullptr;
}

Value *LLVMExp::generateLValIR(TreeNode *AstRoot) {
    vector<Value*> args;
    for(auto son:AstRoot->sonNode) {
        if(son->word.word=="<Exp>") {
            //只需要先分析Exp子节点，如果有，这就是数组
            args.push_back(llvmGenerate->generateLLVMIR(son));
        }
    }
    //获取第一个子节点，LVal的符号Ident
    Symbol* lval_symbol = now_symbol_table->get_symbol_in_all_table(AstRoot->sonNode[0]->word.word, AstRoot->sonNode[0]->word.line_num);
    return generateVarValueIR(args, lval_symbol);
}

Value *LLVMExp::generateVarValueIR(const vector<Value *> &values, Symbol *symbol) {
    if(symbol->is_var) {
        //说明是变量
        return new LoadInstruction(
            IRName::getLocalVariableName(),symbol->value);
    }
    if(values.empty()) {
        //说明没有Exp，应当是函数调用的数组指针，默认从0获取
        auto* array_value = new Value(*symbol->value);
        //将array_value转成指针类型
        return new GetelementptrInstruction(
            symbol->get_ir_type(),
            IRName::getLocalVariableName(),
            symbol->value,
            new ConstValue(new IRInt(),"0")
            );
    }
    if(instanceof<AllocaInstruction>(symbol->value)) {
        //只需要考虑指针指令?
        if(instanceof<IRPointer>(dynamic_cast<AllocaInstruction*>(symbol->value)->get_alloca_ir_type())) {
            //说明是指针类型，先load指针，再get地址，再load值
            auto load_instrction = new LoadInstruction(IRName::getLocalVariableName(),symbol->value);
            return new LoadInstruction(
                IRName::getLocalVariableName(),
                new GetelementptrInstruction(symbol->get_ir_type(),IRName::getLocalVariableName(),load_instrction,values[0])
            );
        }
    }
    //说明这是一维数组
    auto* temp = new LoadInstruction(
        IRName::getLocalVariableName(),
        new GetelementptrInstruction(symbol->get_ir_type(),IRName::getLocalVariableName(),symbol->value,values[0])
    );
    cout<<temp->toLLVM()<<" "<<temp->opValueChain[0]->toLLVM()<<" "<< symbol->value->value_type->toLLVM()<<endl;
    return temp;
}

Value *LLVMExp::generateGetIntIR(const TreeNode *AstRoot) {
    Value* pointer = generateAddressLLVM(AstRoot->sonNode[0]);
    auto* ioGetInt = new IOGetInt(IRName::getLocalVariableName(),"call");
    Value* result = ioGetInt;
    //根据两边的情况，进行类型转换
    IRType* irType = pointer->value_type;
    if(instanceof<IRPointer>(pointer->value_type)) irType = dynamic_cast<IRPointer*>(pointer->value_type)->ir_type();
    if(instanceof<IRChar>(irType)) {
        //如果左值是Char，需要强转
        auto* trunc_ioGetInt = new TruncInstruction(
            IRName::getLocalVariableName(),
            ioGetInt, new IRChar()
            );
        result = trunc_ioGetInt;
    }
    return new StoreInstruction(result,pointer);
}

Value *LLVMExp::generateGetCharIR(const TreeNode *AstRoot) {
    Value* pointer = generateAddressLLVM(AstRoot->sonNode[0]);
    auto* ioGetChar = new IOGetChar(IRName::getLocalVariableName(),"call");
    Value* result = ioGetChar;
    IRType* irType = pointer->value_type;
    if(instanceof<IRPointer>(pointer->value_type)) irType = dynamic_cast<IRPointer*>(pointer->value_type)->ir_type();
    if(instanceof<IRChar>(irType)) {
        //需要强制类型转换
        auto* trunc = new TruncInstruction(
            IRName::getLocalVariableName(),
            ioGetChar,
            new IRChar());
        result = trunc;
    }
    return new StoreInstruction(result,pointer);
}
