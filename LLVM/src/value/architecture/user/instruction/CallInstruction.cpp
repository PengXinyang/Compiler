//
// Created by PengXinyang on 24-11-24.
//

#include "../../../../../include/value/architecture/user/instruction/CallInstruction.h"

CallInstruction::CallInstruction(const string& name, Function *call_function, vector<Value *> &args)
    : Instruction(call_function->getReturnType(),name,"call"){
    addOpValue(call_function);
    this->call_function = call_function;
    for(const auto arg : args) {
        addOpValue(arg);
    }
}

Function *CallInstruction::getCallFunction() const {
    return call_function;
}

string CallInstruction::toLLVM() {
    //%17 = call i32 @f(i32 %16)
    //call void @g(i32 %19)
    ostringstream os;
    if(value_type->isVoid()) {
        //如果返回的类型是void，那么采用第二行字符串的方式输出
        os<<"call void ";
    }else {
        //%17 = call i32
        os<<value_name<<" = call "<<value_type->toLLVM()<<" ";
    }
    //第一个使用的操作数一定是函数本身
    os<<opValueChain[0]->value_name<<"(";
    //接下来，将实参数填入字符串
    int param_size = static_cast<int>(opValueChain.size());
    for(int i=1;i<param_size;i++) {
        //i32 %16
        os<<opValueChain[i]->value_type->toLLVM()<<" "<<opValueChain[i]->value_name;
        //如果后面还有参数，就需要加一个逗号
        if(i+1<param_size) {
            os<<", ";
        }
    }
    os<<")";
    return os.str();
}
