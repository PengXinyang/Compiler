//
// Created by PengXinyang on 24-11-28.
//

#ifndef OPTIMIZERINIT_H
#define OPTIMIZERINIT_H


// 决定是否优化的类
class LLVMOptimizerInit {
private:
    static bool is_optimize;//表示是否优化
    static bool is_const_fold;//是否常量折叠
public:
    LLVMOptimizerInit();
    ~LLVMOptimizerInit();
    static bool isOptimize();
    static void setIsOptimize(bool optimize);
    static bool isConstFold();
    static void setIsConstFold(bool constFold);
};



#endif //OPTIMIZERINIT_H
