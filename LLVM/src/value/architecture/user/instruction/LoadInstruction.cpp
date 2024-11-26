//
// Created by PengXinyang on 24-11-24.
//

#include "../../../../../include/value/architecture/user/instruction/LoadInstruction.h"

#include <sstream>

LoadInstruction::LoadInstruction(const string& name,Value* ptr_value)
    :Instruction(ptr_value->value_type,
        name,"load"){
    //只是用了一个操作数
    if(dynamic_cast<IRPointer*>(ptr_value->value_type)) {
        //说明是指针类型，将该指令改为指针类型
        value_type = dynamic_cast<IRPointer*>(ptr_value->value_type)->ir_type();
    }
    addOpValue(ptr_value);
}

string LoadInstruction::toLLVM() {
    //<result> = load <ty>, ptr <pointer>
    //%8 = load i32, i32* %3
    ostringstream os;
    os<<value_name<<" = ";//result，%8
    os<<instructionType<<" ";//load
    os<<value_type->toLLVM()<<", ";//type，i32
    //使用的操作数只有一个
    if(instanceof<IRPointer>(opValueChain[0]->value_type)) {
        os<<opValueChain[0]->value_type->toLLVM()<<" ";//ptr，i32*
    }
    else  os<<opValueChain[0]->value_type->toLLVM()<<"* ";//ptr，i32*
    os<<opValueChain[0]->value_name;//pointer，%3
    return os.str();
}
