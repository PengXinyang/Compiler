//
// Created by PengXinyang on 24-11-28.
//

#ifndef OPTIMIZERINIT_H
#define OPTIMIZERINIT_H


// 决定是否优化的类
class OptimizerInit {
private:
    static bool is_optimize;//表示是否优化
public:
    OptimizerInit();
    ~OptimizerInit();
    static bool isOptimize() {
        return is_optimize;
    }
    static void setIsOptimize(bool optimize) {
        is_optimize = optimize;
    }
};



#endif //OPTIMIZERINIT_H
