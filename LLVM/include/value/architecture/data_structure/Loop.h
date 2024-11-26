//
// Created by PengXinyang on 24-11-23.
//

#ifndef LOOP_H
#define LOOP_H


//处理循环用，没有继承
class Loop {
private:
    int loopDepth = 0;//循环深度
public:
    Loop() = default;
    [[nodiscard]] int getLoopDepth() const;//获取当前循环深度，第几层循环
};



#endif //LOOP_H
