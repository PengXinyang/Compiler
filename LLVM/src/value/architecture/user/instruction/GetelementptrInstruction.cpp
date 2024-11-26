//
// Created by PengXinyang on 24-11-24.
//

#include "../../../../../include/value/architecture/user/instruction/GetelementptrInstruction.h"

#include "type/irType/IRArray.h"
#include "type/irType/IRPointer.h"

GetelementptrInstruction::GetelementptrInstruction(IRType* ir_type, const string& name, Value *ptr_value, Value *offset_value)
    :Instruction(new IRPointer(ir_type),name, "getelementptr"){
    addOpValue(ptr_value);
    addOpValue(offset_value);
}

string GetelementptrInstruction::toLLVM() {
    //数组形式：
    //%5 = getelementptr inbounds [5 x i32], [5 x i32]* %2, i32 0, i32 1
    //指针形式：（主要用于函数定义中，形参是数组，表现是指针
    //%7 = getelementptr inbounds i32, i32* %6, i32 2
    ostringstream oss;
    //第一个操作数就是%2
    //TODO:NULL
    auto* ptr_value_type = dynamic_cast<IRPointer*>(opValueChain[0]->value_type);
    IRType* ir_type = opValueChain[0]->value_type;
    //TODO：特判，需要修正
    if(!ptr_value_type) ptr_value_type = new IRPointer(ir_type);
    else ir_type = ptr_value_type->ir_type();
    //%5 = getelementptr inbounds
    oss<<value_name<<" = getelementptr inbounds ";
    //[5 x i32] 或 i32
    oss<<ir_type->toLLVM()<<", ";
    //[5 x i32]* 或 i32*
    oss<<ptr_value_type->toLLVM()<<" ";
    //%2,
    oss<<opValueChain[0]->value_name<<", ";
    //如果是数组，要加i32 0；如果是指针，则不加
    if(ir_type->isArray()) {
        oss<<"i32 0, ";
    }
    //TODO:如果中括号内是常量值，比如a[3]，可能需要修改，最后加的不是name，而是值
    //ConstValue的num字段？
    oss<<opValueChain[1]->value_type->toLLVM()<<" "<<opValueChain[1]->value_name;
    return oss.str();
}
