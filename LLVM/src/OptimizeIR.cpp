//
// Created by PengXinyang on 24-12-12.
//

#include "../include/OptimizeIR.h"

#include "GenerateIR.h"
#include "optimize/GVN.h"
#include "optimize/LLVMOptimizerInit.h"
OptimizeIR* OptimizeIR::instance;
OptimizeIR::OptimizeIR() {
    module = GenerateIR::getInstance()->getModule();
}
OptimizeIR* OptimizeIR::getInstance() {
    if (!instance) {
        instance = new OptimizeIR();
    }
    return instance;
}

void OptimizeIR::runOptimizeIR() {
    module->buildCfgGraph();
    module->printCFG();
    module->generateDominantTree();
    module->printDominantTree();
    if(LLVMOptimizerInit::isConstFold()) {
        //说明要运行常量折叠
        GVN::runConstFold(module);
    }
}
