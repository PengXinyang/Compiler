//
// Created by PengXinyang on 24-12-10.
//

#ifndef MIPSOPTIMIZEINIT_H
#define MIPSOPTIMIZEINIT_H



class MipsOptimizerInit {
private:
    static bool is_optimize;//表示是否优化
public:
    MipsOptimizerInit();
    ~MipsOptimizerInit();
    static bool isOptimize() {
        return is_optimize;
    }
    static void setIsOptimize(bool optimize) {
        is_optimize = optimize;
    }
};



#endif //MIPSOPTIMIZEINIT_H
