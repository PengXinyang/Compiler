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
#include "value/architecture/user/instruction/BrInstruction.h"
#include "value/architecture/user/instruction/CalculateInstruction.h"
#include "value/architecture/user/instruction/GetelementptrInstruction.h"
#include "value/architecture/user/instruction/IcmpInstruction.h"
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

void LLVMExp::generateCondIR(TreeNode *AstRoot, BasicBlock *thenBlock, BasicBlock *elseBlock) {
    if(AstRoot->sonNode[0]->word.word=="<LOrExp>") {
        generateLOrExpIR(AstRoot->sonNode[0],thenBlock,elseBlock);
    }
}

/**
* 示例：
* if(a!=a || a!= 'b' || a!='c') return 0;
  %3 = load i8, i8* %2, align 1
  %4 = sext i8 %3 to i32
  %5 = load i8, i8* %2, align 1
  %6 = sext i8 %5 to i32
  %7 = icmp ne i32 %4, %6
  br i1 %7, label %16, label %8    ； 先计算第一个条件

8:                                                ; preds = %0 分号是注释
  %9 = load i8, i8* %2, align 1
  %10 = sext i8 %9 to i32
  %11 = icmp ne i32 %10, 98
  br i1 %11, label %16, label %12

12:                                               ; preds = %8
  %13 = load i8, i8* %2, align 1
  %14 = sext i8 %13 to i32
  %15 = icmp ne i32 %14, 99
  br i1 %15, label %16, label %17

16:                                               ; preds = %12, %8, %0
  store i32 0, i32* %1, align 4
  br label %18

17:                                               ; preds = %12
  store i32 1, i32* %1, align 4
  br label %18

18:                                               ; preds = %17, %16
  %19 = load i32, i32* %1, align 4
  ret i32 %19
*
*/
void LLVMExp::generateLOrExpIR(TreeNode *AstRoot, BasicBlock *thenBlock, BasicBlock *elseBlock) {
    for(const auto son:AstRoot->sonNode) {
        if(son->word.word=="<LAndExp>") {
            if(((AstRoot->father && AstRoot->father->son_num>1) || AstRoot->son_num>1) && AstRoot->father->word.word == "<LOrExp>") {
                //这里需要处理短路运算，所以要创建一个新的基本块。
                //如果这里求完后符合短路特性，就不用计算后面
                /*
                *if (a || (b && c)) {
                *    // 处理a为true的情况
                *} else {
                *    // 处理a为false的情况
                *}
                * 如果 a 为 true，我们跳过 b && c 的计算，直接跳转到 thenBlock。
                * 如果 a 为 false，我们需要生成一个新的基本块来处理 (b && c) 的计算，避免不必要的计算。
                */
                //新生成块，处理子节点的LAndExp
                auto* new_else_block = new BasicBlock(IRName::getBlockName());
                generateLAndExpIR(son,thenBlock,new_else_block);
                IRName::setNowBlock(new_else_block);
            }else {
                generateLAndExpIR(son,thenBlock,elseBlock);
            }
        }else if(son->word.word=="<LOrExp>") {
            generateLOrExpIR(son,thenBlock,elseBlock);
        }
    }
}

void LLVMExp::generateLAndExpIR(TreeNode *AstRoot, BasicBlock *thenBlock, BasicBlock *elseBlock) {
    for(const auto son:AstRoot->sonNode) {
        if(son->word.word=="<EqExp>") {
            if(((AstRoot->father && AstRoot->father->son_num>1) || AstRoot->son_num>1) && AstRoot->father->word.word == "<LAndExp>") {
                //新生成块，处理短路求值失败后的条件句
                auto* next_block = new BasicBlock(IRName::getBlockName());
                generateEqExpIR(son,next_block,elseBlock);
                IRName::setNowBlock(next_block);
            }else {
                generateEqExpIR(son,thenBlock,elseBlock);
            }
        }else if(son->word.word=="<LOrExp>") {
            generateLAndExpIR(son,thenBlock,elseBlock);
        }
    }
}

void LLVMExp::generateEqExpIR(TreeNode *AstRoot, BasicBlock *thenBlock, BasicBlock *elseBlock) {
    Value* condition = llvmGenerate->generateLLVMIR(AstRoot);
    if(instanceof<IRInt>(condition->value_type)) {
        const auto icmp = new IcmpInstruction(
            IRName::getLocalVariableName(),
            "ne",condition,
            new ConstValue(new IRInt(),"0"));
        condition = icmp;
    }
    new BrInstruction(condition,thenBlock,elseBlock);
}
