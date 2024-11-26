//
// Created by PengXinyang on 24-11-25.
//

#include "generate/LLVMGenerate.h"

#include <iostream>

#include "GenerateIR.h"
#include "generate/LLVMExp.h"
#include "LLVMSymbol/SymbolCalculate.h"
#include "LLVMSymbol/SymbolValue.h"
#include "type/IRName.h"
#include "type/irType/IRArray.h"
#include "type/irType/IRChar.h"
#include "type/irType/IRInt.h"
#include "value/architecture/ConstValue.h"
#include "value/architecture/user/instruction/AllocaInstruction.h"
#include "value/architecture/user/instruction/CalculateInstruction.h"
#include "value/architecture/user/instruction/CallInstruction.h"
#include "value/architecture/user/instruction/GetelementptrInstruction.h"
#include "value/architecture/user/instruction/ReturnInstruction.h"
#include "value/architecture/user/instruction/StoreInstruction.h"
#include "value/architecture/user/instruction/TruncInstruction.h"
#include "value/architecture/user/instruction/ZextInstruction.h"
#include "value/architecture/user/instruction/IOInstruction/IOPutCh.h"
#include "value/architecture/user/instruction/IOInstruction/IOPutInt.h"
#include "value/architecture/user/instruction/IOInstruction/IOPutStr.h"

LLVMGenerate* LLVMGenerate::instance;
LLVMGenerate::LLVMGenerate() {
    symbol_table = SymbolHandle::getSymbolHandleInstance()->getSymbolTable();
    now_symbol_table = symbol_table;//初始时，当前所在的符号表就是全局符号表
    now_symbol_table->flag = true;
    symbol_calculate_ = SymbolCalculate::getInstance();
}
LLVMGenerate* LLVMGenerate::getInstance() {
    if (!instance) {
        instance = new LLVMGenerate();
    }
    return instance;
}

Value *LLVMGenerate::generateLLVMIR(TreeNode *AstRoot) {
    symbol_value_instance = SymbolValue::getInstance();
    generateIR = GenerateIR::getInstance();
    llvmExp = new LLVMExp(this);
    if(AstRoot->word.word == "<CompUnit>") {
        return CompUnitIR(AstRoot);
    }
    if(AstRoot->word.word == "<ConstDef>") {
        return ConstDefIR(AstRoot);
    }
    if(AstRoot->word.word == "<VarDef>") {
        return VarDefIR(AstRoot);
    }
    if(AstRoot->word.word == "<Block>") {
        return BlockIR(AstRoot);
    }
    if(AstRoot->word.word == "return") {
        return ReturnIR(AstRoot);
    }
    if(AstRoot->word.word == "printf") {
        return PrintfIR(AstRoot);
    }
    if(AstRoot->word.word == "<Exp>" || AstRoot->word.word == "<ConstExp>") {
        return ExpIR(AstRoot);
    }
    if(AstRoot->word.word == "<PrimaryExp>") {
        return PrimaryExpIR(AstRoot);
    }
    if(AstRoot->word.word == "<Number>" || AstRoot->word.word == "<Character>") {
        return NumberOrCharIR(AstRoot);
    }
    if(AstRoot->word.word == "<UnaryExp>") {
        return UnaryExpIR(AstRoot);
    }
    if(AstRoot->word.word == "<MulExp>") {
        return MulExpIR(AstRoot);
    }
    if(AstRoot->word.word == "<AddExp>") {
        return AddExpIR(AstRoot);
    }
    if(AstRoot->word.word == "=") {
        return AssignIR(AstRoot);
    }
    if(AstRoot->word.word == "<FuncDef>") {
        return FuncDefIR(AstRoot);
    }
    if(AstRoot->word.word == "<MainFuncDef>") {
        return MainFuncDefIR(AstRoot);
    }
    if(AstRoot->word.word == "<FuncFParam>") {
        return FuncFParamIR(AstRoot);
    }
    generateIR->traverseTreeIR(AstRoot);
    return nullptr;
}

Value *LLVMGenerate::CompUnitIR(TreeNode *AstRoot) {
    generateIR->traverseTreeIR(AstRoot);
    return nullptr;
}

/**
* ConstDef -> Ident [ '[' ConstExp ']' ] '=' ConstInitVal ';'
*/
Value *LLVMGenerate::ConstDefIR(TreeNode *AstRoot) {
    //再次遍历语法树，直接从当前表里找对应的符号Ident
    Symbol* const_symbol = now_symbol_table->get_symbol(AstRoot->sonNode[0]->word.word);
    if(const_symbol == nullptr) {
        return nullptr;
    }
    //首先，将数组长度清空，因为之前的代码可能产生干扰
    int dim_size = 0;
    InitVar* init_var = nullptr;
    vector<int> init_vals;
    string initString;
    for(int i = 1;i<AstRoot->son_num;++i) {
        //开始遍历后续内容
        if(AstRoot->sonNode[i]->word.word == "<ConstExp>") {
            //如果是ConstExp，计算它的值
            dim_size = symbol_calculate_->calculate(AstRoot->sonNode[i]);
        }
        if(AstRoot->sonNode[i]->word.word == "<ConstInitVal>") {
            //说明这里是赋值，要新建InitVar
            for(auto* son: AstRoot->sonNode[i]->sonNode) {
                if(son->word.word_type=="STRCON") {
                    //说明是StringConst赋初值
                    initString = son->word.word;
                }else if(son->word.word=="<ConstExp>") {
                    //是常量初值，计算后加入赋值vector
                    init_vals.push_back(symbol_calculate_->calculate(son));
                }
            }
        }
    }
    //设置符号的个数
    const_symbol->dim = dim_size==0? 1 : dim_size;
    int dim = dim_size==0? 0 : 1;
    //说明数组第0个元素应当是一个变量的初值
    if(!init_vals.empty()) {const_symbol->var_value = init_vals[0];}
    const_symbol->array_values = init_vals;
    const_symbol->const_string = initString;
    if(const_symbol->is_in_global_table()) {
        //如果在全局表中
        if(const_symbol->is_var) {
            //Value应当是全局Value
            /*const_symbol->value = new GlobalVariable(
                const_symbol->get_ir_type(),
                IRName::getGlobalVariableName(const_symbol->token),
                init_var,
                true);*/
            //是全局的Value，但是也是一个常量，所以记录为ConstValue更好
            const_symbol->value = new ConstValue(
                const_symbol->get_ir_type(),
                to_string(init_vals[0])
                );
            init_var = new InitVar(
                const_symbol->get_ir_type(),
                init_vals,
                initString
            );
            const_symbol->value = new GlobalVariable(
                new IRPointer(const_symbol->get_ir_type()),
                IRName::getGlobalVariableName(const_symbol->token),
                init_var,true);
        }else {
            //说明是全局的数组
            init_var = new InitVar(
                new IRArray(const_symbol->get_ir_type(),dim_size),
                init_vals,
                initString
            );
            const_symbol->value = new GlobalVariable(
                new IRArray(const_symbol->get_ir_type(),dim_size),
                IRName::getGlobalVariableName(const_symbol->token),
                init_var,
                true
                );
        }
    }else {
        //否则，是在局部表中
        //实际上，在常数定义部分，局部表和非局部表操作类似
        //注意，这里开始创建指令类型并且要加入到module中。
        if(const_symbol->is_var) {
            //是一个常量，记录为ConstValue
            const_symbol->value = new ConstValue(
                const_symbol->get_ir_type(),
                to_string(init_vals[0])
                );
            auto instruction = new AllocaInstruction(
                const_symbol->get_ir_type(),
                IRName::getLocalVariableName()
                );
            const_symbol->value = instruction;
            Value* value1 = symbol_value_instance->genConstValuesIR(AstRoot->sonNode[AstRoot->son_num-1],0)[0];
            new StoreInstruction(value1,instruction);
        }else {
            //说明是局部常数的数组
            //需要使用指令
            //创建Alloca指令
            Instruction* instruction = new AllocaInstruction(
                new IRArray(const_symbol->get_ir_type(),dim_size),
                IRName::getLocalVariableName()
                );
            const_symbol->value = instruction;
            //子节点最后一位是constInitVal，说明需要赋值，store指令
            Value* pointer = instruction;
            vector<Value*> value_list = symbol_value_instance->genConstValuesIR(AstRoot->sonNode[AstRoot->son_num-1],1);
            int offset = 0;
            for(Value* v: value_list) {
                instruction = new GetelementptrInstruction(
                    const_symbol->get_ir_type(),
                    IRName::getLocalVariableName(),
                    pointer,
                    new ConstValue(new IRInt(),to_string(offset)));
                new StoreInstruction(v,instruction);
                offset++;
            }
        }
    }
    return nullptr;
}

Value *LLVMGenerate::VarDefIR(TreeNode *AstRoot) {
    //找符号ident
    Symbol* var_symbol = now_symbol_table->get_symbol(AstRoot->sonNode[0]->word.word);
    int dim_size = 0;
    InitVar* init_var = nullptr;
    vector<int> init_vals;
    string initString;
    for(int i = 1;i<AstRoot->son_num;++i) {
        //开始遍历后续内容
        //这里是数组的确定
        if(AstRoot->sonNode[i]->word.word == "<ConstExp>") {
            //如果是ConstExp，计算它的值
            dim_size = symbol_calculate_->calculate(AstRoot->sonNode[i]);
        }
        if(AstRoot->sonNode[i]->word.word == "<InitVal>") {
            //说明这里是赋值，要新建InitVar
            TreeNode* init_node = AstRoot->sonNode[i];
            if(init_node->sonNode[0]->word.word_type=="STRCON") {
                //说明是StringConst赋初值
                initString = init_node->sonNode[0]->word.word;
            }else {
                //说明是Exp集合
                int dim = dim_size==0 ? 0 : 1;
                init_vals = symbol_calculate_->calculateInitVal(dim,init_node);
            }
        }
    }
    //设置符号的个数
    var_symbol->dim = dim_size==0? 1 : dim_size;
    //说明数组第0个元素应当是一个变量的初值
    if(!init_vals.empty()) var_symbol->var_value = init_vals[0];
    var_symbol->array_values = init_vals;
    var_symbol->const_string = initString;
    if(var_symbol->is_in_global_table()) {
        //是全局的，全局的变量在赋值中和常量应该一致
        //首先，将数组长度清空，因为之前的代码可能产生干扰
        if(var_symbol->is_var) {
            init_var = new InitVar(var_symbol->get_ir_type(),init_vals,initString);
            //Value应当是全局Value，全局Value应该是指针
            var_symbol->value = new GlobalVariable(
                new IRPointer(var_symbol->get_ir_type()),
                IRName::getGlobalVariableName(var_symbol->token),
                init_var,
                true);
        }else {
            //说明是一个数组
            init_var = new InitVar(
                new IRArray(var_symbol->get_ir_type(),dim_size),
                init_vals,
                initString
            );
            var_symbol->value = new GlobalVariable(
                new IRArray(var_symbol->get_ir_type(),dim_size),
                IRName::getGlobalVariableName(var_symbol->token),
                init_var,
                true
                );
        }
    }else {
        //是局部变量
        Instruction* instruction;
        if(var_symbol->is_var) {
            //说明是局部变量，不是数组
            instruction = new AllocaInstruction(
                var_symbol->get_ir_type(),
                IRName::getLocalVariableName()
                );
            var_symbol->value = instruction;
            //这里需要确定一下最后子节点是不是initVal
            if(AstRoot->sonNode[AstRoot->son_num-1]->word.word=="<InitVal>") {
                Value* value1 = symbol_value_instance->genValuesIR(AstRoot->sonNode[AstRoot->son_num-1],0)[0];
                //如果左侧和右侧类型不同，强制转换
                if(var_symbol->btype==1 && instanceof<IRInt>(value1)) {
                    auto* trunc_value = new TruncInstruction(IRName::getLocalVariableName(),value1,new IRChar());
                    value1 = trunc_value;
                }else if(var_symbol->btype==0 && instanceof<IRChar>(value1)) {
                    auto* zext_value = new ZextInstruction(IRName::getLocalVariableName(),value1,new IRInt());
                    value1 = zext_value;
                }else if(instanceof<ConstValue>(value1)) {
                    if(var_symbol->btype==1 && instanceof<IRInt>(value1->value_type)) {
                        auto* trunc_value = new TruncInstruction(IRName::getLocalVariableName(),value1,new IRChar());
                        value1 = trunc_value;
                    }
                    else if(var_symbol->btype==0 && instanceof<IRChar>(value1->value_type)) {
                        auto* zext_value = new ZextInstruction(IRName::getLocalVariableName(),value1,new IRInt());
                        value1 = zext_value;
                    }
                }
                new StoreInstruction(value1,instruction);
            }
        }else {
            //TODO: 局部数组的处理
            //示例：
            /*
             * int c[3] = {1, 2, 3};
             * %1 = alloca [3 x i32]
             * %2 = getelementptr inbounds [3 x i32], [3 x i32]* %1, i32 0, i32 0
             * store i32 1, i32* %2
             * %3 = getelementptr inbounds i32, i32* %2, i32 1
             * store i32 2, i32* %3
             * %4 = getelementptr inbounds i32, i32* %3, i32 1
             * store i32 3, i32* %4
             */
            instruction = new AllocaInstruction(
                new IRArray(var_symbol->get_ir_type(),dim_size),
                IRName::getLocalVariableName()
                );
            var_symbol->value = instruction;
            //子节点最后一位是constInitVal，说明需要赋值，store指令
            if(AstRoot->sonNode[AstRoot->son_num-1]->word.word=="<InitVal>") {
                Value* pointer = instruction;
                vector<Value*> value_list = symbol_value_instance->genValuesIR(AstRoot->sonNode[AstRoot->son_num-1],1);
                int offset = 0;
                for(Value* v: value_list) {
                    instruction = new GetelementptrInstruction(
                        var_symbol->get_ir_type(),
                        IRName::getLocalVariableName(),
                        pointer,
                        new ConstValue(new IRInt(),to_string(offset)));
                    //cout<<instruction->toLLVM()<<endl;
                    new StoreInstruction(v,instruction);
                    offset++;
                }
            }
        }
    }
    return nullptr;
}

Value *LLVMGenerate::BlockIR(TreeNode *AstRoot) {
    //进入一个块，当前的符号表要前进一位
    //注意，如果Block的父节点是 FuncDef ，则不进入符号表，因为函数已经进去了
    if(!(AstRoot->father->word.word=="<FuncDef>"||
        AstRoot->father->word.word=="<MainFuncDef>")) {
        enterNewSymbolTable();
    }
    generateIR->traverseTreeIR(AstRoot);
    quitNowSymbolTable();
    return nullptr;
}

//注意：return语句和函数的类型有密切的关系。如果两者类型不同，需要强制转换
//另外，错误处理只考虑了有返回值函数有return，并不考虑return后有没有值。
//所以如果函数有类型，且只有return，返回0对应的类型
Value *LLVMGenerate::ReturnIR(TreeNode *AstRoot) {
    TreeNode* father_node = AstRoot->father;
    Value* return_value = nullptr;
    //查找当前函数的符号，讨论两者类型是否一致，不一致要强制转换
    Symbol* func_synmbol = now_symbol_table->get_symbol_in_all_table(now_symbol_table->func_token);
    if(func_synmbol==nullptr) {
        //由于已经进行了错误处理，所以这里的return应当是main函数
        if(father_node->son_num<=2) {
            return_value = new ConstValue(new IRInt(),"0");
            return new ReturnInstruction(return_value);
        }
        return_value = generateLLVMIR(father_node->sonNode[1]);
        if(instanceof<IRChar>(return_value->value_type)) {
            //返回是int，但是计算出的是char，用zext转
            auto* zext_return_value = new ZextInstruction(
                IRName::getLocalVariableName(),
                return_value,
                new IRInt()
                );
            return_value = zext_return_value;
        }
        return new ReturnInstruction(return_value);
    }
    if(father_node->son_num>2) {
        //孩子数大于2，一定返回有值，最后一个是分号
        return_value = generateLLVMIR(father_node->sonNode[1]);
        //查找类型
        if(instanceof<IRInt>(return_value->value_type) && func_synmbol->func_type==1) {
            //函数是char，要将结果用trunc转换
            auto* trunc_return_value = new TruncInstruction(
                IRName::getLocalVariableName(),
                return_value,
                new IRChar()
                );
            return_value = trunc_return_value;
        }else if(instanceof<IRChar>(return_value->value_type) && func_synmbol->func_type==0) {
            //返回是int，但是计算出的是char，用zext转
            auto* zext_return_value = new ZextInstruction(
                IRName::getLocalVariableName(),
                return_value,
                new IRInt()
                );
            return_value = zext_return_value;
        }
    }else {
        //查函数的类型
        if(func_synmbol->func_type==0) {
            //int类型
            return_value = new ConstValue(new IRInt(),"0");
        }else if(func_synmbol->func_type==1) {
            //char 类型
            return_value = new ConstValue(new IRChar(),"0");
        }
        //void类型直接用空指针即可
    }
    //接下来要走出函数，所以IRName的函数需要修改为null
    IRName::setNowFunction(nullptr);
    return new ReturnInstruction(return_value);
}

/*
* 示例：
* printf("Hello: %d, %c", a, b);
* @.str = private unnamed_addr constant [8 x i8] c"Hello: \00", align 1
* @.str.1 = private unnamed_addr constant [3 x i8] c", \00", align 1
* 根据格式符的不同，涉及强制类型转换
*/
Value *LLVMGenerate::PrintfIR(TreeNode *AstRoot) {
    TreeNode* father_node = AstRoot->father;
    ostringstream oss;
    vector<Value*> exp_values;
    //统计语句里的Exp
    for(auto son:father_node->sonNode) {
        if(son->word.word=="<Exp>") {
            exp_values.push_back(generateLLVMIR(son));
        }
    }
    string format_string = father_node->sonNode[2]->word.word;
    int length = static_cast<int>(format_string.length());
    int exp_num = 0;
    for(int i=1;i<length - 1;i++) {
        if(format_string[i] == '%'&& (format_string[i+1] == 'd' ||format_string[i+1] == 'c')) {
            //如果是%d或者%c才需要考虑占位符
            if(!oss.str().empty()) {
                vector<int> str_num;
                //记录字符串长度
                str_num.push_back(oss.str().length()+1);
                auto* const_string = new ConstString(
                    IRName::getConstStringName(),
                    oss.str(),
                    str_num
                    );
                new IOPutStr(const_string);
                oss.str("");
            }
            Value* value = exp_values[exp_num++];
            if(format_string[i+1] == 'd') {
                //%d格式，如果value是char类型需要强制转换
                if(instanceof<IRChar>(value->value_type)) {
                    //说明是char类型，需要zext强转
                    Value* zext_value = new ZextInstruction(
                        IRName::getLocalVariableName(),
                        value,
                        new IRInt());
                    new IOPutInt(zext_value);
                }
                else new IOPutInt(value);
            }else if(format_string[i+1] == 'c') {
                //%c格式，如果value是int类型需要用trunc强转
                if(instanceof<IRInt>(value->value_type)) {
                    Value* trunc_value = new TruncInstruction(
                        IRName::getLocalVariableName(),
                        value,
                        new IRChar());
                    value = trunc_value;
                }
                //注意：putchar接受int，其实应该再把char转int
                Value* zext_value = new ZextInstruction(
                    IRName::getLocalVariableName(),
                    value,
                    new IRInt());
                value = zext_value;
                new IOPutCh(value);
            }
            i++;
        }else if(format_string[i] == '\\' && format_string[i+1] == 'n') {
            //如果是\n，需要换行
            oss<<"\n";i++;
        }else {
            oss<<format_string[i];
        }
    }
    //如果结束后oss还不为空，需要再次输出
    if(!oss.str().empty()) {
        vector<int> str_num;
        //记录字符串长度
        str_num.push_back(oss.str().length() + 1);
        auto* const_string = new ConstString(
            IRName::getConstStringName(),
            oss.str(),
            str_num
            );
        new IOPutStr(const_string);
        oss.str("");
    }
    return nullptr;
}

/**
* Exp -> AddExp; ConstExp -> AddExp
* 直接继承，不涉及强制类型转换
*/
Value *LLVMGenerate::ExpIR(TreeNode *AstRoot) {
    //AddExp
    return generateLLVMIR(AstRoot->sonNode[0]);
}

/**
* PrimaryExp -> '(' Exp ')' | LVal | Number | Character
* 直接继承，不涉及强制转换
*/
Value *LLVMGenerate::PrimaryExpIR(TreeNode *AstRoot) {
    auto son = AstRoot->sonNode[0];
    Value* ans;
    if(son->word.word=="(") {
        //说明是（EXP）
        ans = ExpIR(AstRoot->sonNode[1]);
    }else if(son->word.word=="<LVal>") {
        ans = llvmExp->generateLValIR(son);
    }else {
        //Number | Character
        ans = NumberOrCharIR(son);
    }
    return ans;
}

Value *LLVMGenerate::NumberOrCharIR(TreeNode *AstRoot) {
    //数字或char
    if(AstRoot->word.word=="<Number>")return new ConstValue(new IRInt(),AstRoot->sonNode[0]->word.word);
    if(AstRoot->word.word=="<Character>") {
        int num = AstRoot->sonNode[0]->word.word[1];
        return new ConstValue(new IRChar(),to_string(num));
    }
    return nullptr;
}

/**
* UnaryExp -> PrimaryExp | Ident '(' [FuncRParams] ')' | UnaryOp UnaryExp
* PrimaryExp -> '(' Exp ')' | LVal | Number
* 不涉及二元运算，但是函数调用处涉及强制类型转换
* int x = foo(a[4], a);
* %6 = getelementptr inbounds [6 x i32], [6 x i32]* @a, i32 0, i32 4
* %7 = load i32, i32* %6
* %8 = getelementptr inbounds [6 x i32], [6 x i32]* @a, i32 0, i32 0
* %9 = call i32 @foo(i32 %7, i32* %8)
*/
Value *LLVMGenerate::UnaryExpIR(TreeNode *AstRoot) {
    //都有第一个节点，直接分析
    Value* ans = generateLLVMIR(AstRoot->sonNode[0]);
    if(AstRoot->sonNode[0]->word.word=="<PrimaryExp>") {
        //说明是PrimaryExp
        return ans;
    }if(AstRoot->sonNode[0]->word.word=="<UnaryOp>") {
        //是一个符号
        ans = generateLLVMIR(AstRoot->sonNode[1]);
        string type = AstRoot->sonNode[0]->sonNode[0]->word.word;
        if(type=="+") {
            return ans;
        }if(type=="-") {
            //取负号，根据教程，视为0减ans
            return new CalculateInstruction(
                ans->value_type,
                IRName::getLocalVariableName(),
                "sub",
                new ConstValue(ans->value_type,"0"),
                ans);
        }if(type=="!") {
            //TODO: 涉及条件取值，之后再写
        }
    }else {
        //函数取值，先从符号表获取函数的符号
        Symbol* func_symbol = now_symbol_table->get_symbol_in_all_table(AstRoot->sonNode[0]->word.word);
        if(func_symbol==nullptr) return ans;
        auto* function = dynamic_cast<Function *>(func_symbol->value);
        if(function==nullptr) return ans;
        vector<Value*> params;
        //根据子节点，找FuncRParams
        int param_num = 0;
        if(AstRoot->sonNode[2]->word.word=="<FuncRParams>") {
            //说明有实参
            for(auto son:AstRoot->sonNode[2]->sonNode) {
                if(son->word.word=="<Exp>") {
                    Value* param_value = generateLLVMIR(son);
                    //func_symbol记录了参数的类型，根据类型选取
                    if(func_symbol->param_type[param_num] == 0) {
                        //说明是整数
                        if(instanceof<IRChar>(param_value->value_type)) {
                            //需要将char转为int
                            auto* zext_value = new ZextInstruction(
                                IRName::getLocalVariableName(),
                                param_value,new IRInt());
                            param_value = zext_value;
                        }
                    }else if(func_symbol->param_type[param_num] == 1) {
                        //说明是char
                        if(instanceof<IRInt>(param_value->value_type)) {
                            //需要将int转char
                            auto* trunc_value = new TruncInstruction(
                                IRName::getLocalVariableName(),
                                param_value,new IRChar());
                            param_value = trunc_value;
                        }
                    }
                    param_num++;
                    params.push_back(param_value);
                }
            }
        }
        //根据类型，如果函数有返回类型则赋名字，否则不用赋名
        if(func_symbol->func_type==2) {
            return new CallInstruction("",function,params);
        }
        ans = new CallInstruction(
            IRName::getLocalVariableName(),
            function,
            params);
        return ans;
    }
    return ans;
}

/**
* MulExp ->  UnaryExp | MulExp ('*' | '/' | '%') UnaryExp
* 二元运算，需要考虑类型转换
*/
Value *LLVMGenerate::MulExpIR(TreeNode *AstRoot) {
    //首先分析第一个元素
    Value* ans = generateLLVMIR(AstRoot->sonNode[0]);
    //如果只有一个子节点，直接可以返回
    if(AstRoot->son_num<=1) return ans;
    //否则，分析第三个子节点，看两个谁用强制类型转换
    Value* ans2 = generateLLVMIR(AstRoot->sonNode[2]);
    //两个数强制类型相同，并生成对应的强转指令
    changeType(&ans,&ans2);
    //转换完成，可以处理
    if(AstRoot->sonNode[1]->word.word=="*") {
        return new CalculateInstruction(
            ans->value_type,IRName::getLocalVariableName(),
            "mul", ans,ans2);
    }
    if(AstRoot->sonNode[1]->word.word=="/") {
        return new CalculateInstruction(
            ans->value_type,IRName::getLocalVariableName(),
            "sdiv", ans, ans2);
    }
    if(AstRoot->sonNode[1]->word.word=="%") {
        return new CalculateInstruction(
            ans->value_type,IRName::getLocalVariableName(),
            "srem", ans,ans2);
    }
    printf("Error in MulExpIR\n");
    return ans;
}

/**
* AddExp -> MulExp | AddExp ('+' | '−') MulExp }
*/
Value *LLVMGenerate::AddExpIR(TreeNode *AstRoot) {
    //首先分析第一个元素
    Value* ans = generateLLVMIR(AstRoot->sonNode[0]);
    //如果只有一个子节点，直接可以返回
    if(AstRoot->son_num<=1) return ans;
    //否则，分析第三个子节点，看两个谁用强制类型转换
    Value* ans2 = generateLLVMIR(AstRoot->sonNode[2]);
    //两个数强制类型相同，并生成对应的强转指令
    changeType(&ans,&ans2);
    if(AstRoot->sonNode[1]->word.word=="+") {
        return new CalculateInstruction(
            ans->value_type,IRName::getLocalVariableName(),
            "add", ans,ans2);
    }
    if(AstRoot->sonNode[1]->word.word=="-") {
        return new CalculateInstruction(
            ans->value_type,IRName::getLocalVariableName(),
            "sub", ans, ans2);
    }
    //转换完成，可以处理加减
    printf("Error in AddExpIR\n");
    return ans;
}

//接下来是函数相关
/**
* FuncDef -> FuncType Ident '(' [FuncFParams] ')' Block
*/
Value *LLVMGenerate::FuncDefIR(TreeNode *AstRoot) {
    //在这里，需要找到对应的函数符号。由于函数定义在全局符号表，所以不会出现覆盖的情况
    //找到符号后，建立Function value，与符号内的value相对应
    //同时，Function建立完成后，应当通过IRName更新。
    //获取函数符号表
    Symbol* func_symbol = now_symbol_table->get_symbol_in_all_table(AstRoot->sonNode[1]->word.word);
    if(func_symbol==nullptr) {
        printf("Error in FuncDefIR\n");
        return nullptr;
    }
    IRType* return_type = func_symbol->get_ir_type();
    //组建Function类
    auto* func = new Function(IRName::getFunctionName(func_symbol->token), return_type);
    func_symbol->value = func;
    //给命名系统设置当前的函数
    IRName::setNowFunction(func);
    auto* basic_block = new BasicBlock(IRName::getBlockName());
    //设置当前基本块
    //由于基本块只针对当前块的命名，与局部变量命名无关，所以退出块时不用置为null
    IRName::setNowBlock(basic_block);
    //理论上，这里应当进入基本块了，方便后续参数的处理
    enterNewSymbolTable();
    generateIR->traverseTreeIR(AstRoot);
    //获取最后一块
    BasicBlock* last_block = IRName::getNowBlock();
    //对于无返回值函数，最后一行需要看看有没有return指令。
    //如果没有return指令，需要补充一个 ret void
    //对于有返回值函数，g类错误保证最后一行一定有return，不再考虑
    if(last_block->isEmpty() ||
        !instanceof<ReturnInstruction>(last_block->getLastInstruction())) {
        //说明最后一行要么没有指令，要么没有return
        new ReturnInstruction(nullptr);
    }
    return nullptr;
}

//主函数
Value *LLVMGenerate::MainFuncDefIR(TreeNode *AstRoot) {
    //实际上，主函数和普通函数几乎一致
    //然儿，并不能复用，因为main函数在符号表不存在
    IRType* return_type = new IRInt();
    //组建Function类
    auto* func = new Function("@main", return_type);
    //给命名系统设置当前的函数
    IRName::setNowFunction(func);
    auto* basic_block = new BasicBlock(IRName::getBlockName());
    //设置当前基本块
    //由于基本块只针对当前块的命名，与局部变量命名无关，所以退出块时不用置为null
    IRName::setNowBlock(basic_block);
    //理论上，这里应当进入基本块了，方便后续参数的处理
    enterNewSymbolTable();
    generateIR->traverseTreeIR(AstRoot);
    //main函数肯定有return语句
    return nullptr;
}

/**
* FuncFParam -> BType Ident [ '[' ']' ]
*/
Value *LLVMGenerate::FuncFParamIR(TreeNode *AstRoot) {
    //首先，第1个子节点一定是Ident，可以先获取符号
    //注意，参数必须是局部的，一定只能在当前符号表内
    Symbol* param_symbol = now_symbol_table->get_symbol(AstRoot->sonNode[1]->word.word);
    if(param_symbol==nullptr) {
        //理论上不可能
        printf("Error in FuncFParamIR\n");
        return nullptr;
    }
    IRType* ir_type = param_symbol->get_ir_type();
    if(param_symbol->is_array) {
        //说明是数组类型，在这里应当存为指针类型
        ir_type = new IRPointer(param_symbol->get_ir_type());
    }
    auto* param = new Param(ir_type,IRName::getParamName());
    if(param->value_type->isPointer()) {
        //是指针类型，直接存就行
        param_symbol->value = param;
        Instruction* instruction = new AllocaInstruction(param->value_type,IRName::getLocalVariableName());
        param_symbol->value = instruction;
        new StoreInstruction(param,instruction);
    }else {
        //先分配空间，再store
        Instruction* instruction = new AllocaInstruction(param->value_type,IRName::getLocalVariableName());
        param_symbol->value = instruction;
        new StoreInstruction(param,instruction);
    }
    generateIR->traverseTreeIR(AstRoot);
    return nullptr;
}

/**
* Stmt-> LVal '=' ('getint' | 'getchar' '(' ')' ';'| Exp ';')
* 这里是一系列赋值，左右两边分析后需要考虑强制类型转换
*/
Value *LLVMGenerate::AssignIR(TreeNode *AstRoot) {
    //理论上，进入这个函数的时候，处于等于号“=”的状态
    TreeNode* father = AstRoot->father;
    //如果只有三个孩子，说明是Exp，否则就是两个IO函数
    if(father==nullptr) {
        printf("Error in AssignIR\n");
        return nullptr;
    }
    Value* LVal = llvmExp->generateAddressLLVM(father->sonNode[0]);
    IRType* irType = LVal->value_type;
    if(instanceof<IRPointer>(LVal->value_type)) {
        //如果指令是指针类型，则要看实际的类型，再判定类型转换。
        irType = dynamic_cast<IRPointer*>(LVal->value_type)->ir_type();
    }
    if(father->sonNode[2]->word.word == "<Exp>") {
        //说明确实是Exp
        Value* Exp = generateLLVMIR(father->sonNode[2]);
        //需要强制类型转换，注意，一定是右侧强转成左侧
        if(instanceof<IRInt>(Exp->value_type) && instanceof<IRChar>(irType)) {
            //说明要将int转为char
            Value* trunc_exp = new TruncInstruction(
                IRName::getLocalVariableName(),Exp,new IRChar());
            Exp = trunc_exp;
        }
        else if(instanceof<IRChar>(Exp->value_type) && instanceof<IRInt>(irType)) {
            //左侧是int，要将右侧转为int
            Value* zext_exp = new ZextInstruction(
                IRName::getLocalVariableName(),Exp,new IRInt());
            Exp = zext_exp;
        }
        return new StoreInstruction(Exp,LVal);
    }
    if(father->sonNode[2]->word.word == "getint") {
        //分析getint
        return llvmExp->generateGetIntIR(father);
    }
    if(father->sonNode[2]->word.word == "getchar") {
        return llvmExp->generateGetCharIR(father);
    }
    //出错
    printf("Error in AssignIR\n");
    return nullptr;
}

void LLVMGenerate::enterNewSymbolTable() {
    //进入新的符号表
    for(const auto& child: now_symbol_table->children_ptr) {
        if(!child->flag) {
            child->flag = true;
            now_symbol_table = child;
            break;
        }
    }
}

void LLVMGenerate::quitNowSymbolTable() {
    now_symbol_table = now_symbol_table->father_ptr;
}

void LLVMGenerate::changeType(Value **value1, Value **value2) {
    //说明二者类型不同，用强制转换，低转高
    if(instanceof<IRChar>((*value1)->value_type)&& instanceof<IRInt>((*value2)->value_type)) {
        //说明第一个ans需要强制转换
        Value* zext_ans = new ZextInstruction(
            IRName::getLocalVariableName(),
            *value1,
            new IRInt()
            );
        *value1 = zext_ans;
    }else if(instanceof<IRInt>((*value1)->value_type) && instanceof<IRChar>((*value2)->value_type)){
        //第二个需要强制类型转换，低转高
        Value* zext_ans2 = new ZextInstruction(
            IRName::getLocalVariableName(),
            *value2,
            new IRInt()
            );
        *value2 = zext_ans2;
    }
}
