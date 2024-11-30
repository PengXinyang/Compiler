//
// Created by PengXinyang on 24-11-23.
//

#include "value/architecture/user/Function.h"

#include <sstream>

#include "optimize/OptimizerInit.h"
#include "type/IRName.h"
#include "type/irType/IRBlock.h"

Function::Function(const string& name, IRType* returnType):GlobalValue(new IRBlock("function"),name){
    this->returnType = returnType;
    basicBlocks = vector<BasicBlock*>();
    params = vector<Param*>();
    if(!OptimizerInit::isOptimize()) {
        IRName::addFunction(this);
    }
}

void Function::addParam(Param *param) {
    this->params.push_back(param);
    param->setFunc(this);
}
void Function::addBasicBlock(BasicBlock *basicBlock, int index) {
    if(index>=0 && index < basicBlocks.size()) {
        basicBlocks.insert(basicBlocks.begin()+index,basicBlock);
    }
    else {
        basicBlocks.push_back(basicBlock);
    }
    basicBlock->setFunctionParent(this);
}

string Function::toLLVM() {
    /**
     * 示例：
     * define dso_local i32 @func() {
     *     store i32 2, i32* @a
     *     ret i32 1
     * }
     */
    ostringstream os;
    //首先添加第一行前缀部分
    //注意，”@“包含在了value_name中
    os<<"define dso_local "<<returnType->toLLVM()<<" "<<value_name<<"(";
    //添加函数的参数
    const int param_num = static_cast<int>(params.size());
    for(int i=0;i<param_num;i++) {
        os<<params[i]->toLLVM();
        if(i<param_num-1) os<<", ";
    }
    //添加参数后到第一行最右侧
    os<<") {\n";
    const int basic_block_num = static_cast<int>(basicBlocks.size());
    for(int i=0;i<basic_block_num;i++) {
        os<<basicBlocks[i]->toLLVM();
        if(i<basic_block_num-1) os<<"\n\n";
    }
    os<<"\n}";
    return os.str();
}
