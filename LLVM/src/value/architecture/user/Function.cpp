//
// Created by PengXinyang on 24-11-23.
//

#include "value/architecture/user/Function.h"

#include <sstream>

#include "generate/MipsCell.h"
#include "optimize/LLVMOptimizerInit.h"
#include "optimize/data_structure/DominantTree.h"
#include "Register/RegisterController.h"
#include "Register/RegisterTool.h"
#include "structure/text/MipsBlock.h"
#include "type/IRName.h"
#include "type/irType/IRBlock.h"

Function::Function(const string& name, IRType* returnType):GlobalValue(new IRBlock("function"),name){
    this->returnType = returnType;
    basicBlocks = vector<BasicBlock*>();
    params = vector<Param*>();
    valueRegisterMap = unordered_map<Value*, Register*>();
    if(!LLVMOptimizerInit::isOptimize()) {
        IRName::addFunction(this);
    }
    //开启优化，添加控制流图
    cfgGraph = new CfgGraph(this);
    dominantTree = new DominantTree(this);
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

void Function::generateMIPS() {
    new MipsBlock(value_name.substr(1));
    MipsCell::resetFunction(this);
    int num = min(4,static_cast<int>(params.size()));
    for(int i=0;i<num;++i) {
        //前三个参数只需要压栈，由a0-a2寄存器保存
        RegisterController::allocateRegister(
            params[i],
            Register::getRegister(Register::regTransform(static_cast<int>(RegisterName::$a0) + i))
        );
        RegisterTool::moveValue(params[i]);
    }
    for(int i=4;i<params.size();++i) {
        RegisterTool::moveValue(params[i]);
    }
    for(const auto basicBlock:basicBlocks) {
        basicBlock->generateMIPS();
    }
}

void Function::buildCfgGraph() const {
    if(cfgGraph) {
        cfgGraph->buildCFG();
    }
}

CfgGraph *Function::getCfgGraph() const {
    return cfgGraph;
}

DominantTree *Function::getDominantTree() const {
    return dominantTree;
}

void Function::buildDominantTree() const {
    if(dominantTree) {
        //生成支配集
        dominantTree->generateDominateBlocks();
        //生成支配树
        dominantTree->generateDominantTree();
        //生成支配边界
        dominantTree->generateDominateEdge();
    }
}
