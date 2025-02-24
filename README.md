# 2024北航编译技术实验

本项目是2024年的实验

其中文法的定义参见文法.pdf

各部分设计请参阅设计文档

代码优化部分请参阅优化文档

## 期中测试

添加了如下文法：

```c++
Assign → '+=' | '-=' | '*=' | '/=' | '%=' //以前的单一赋值号变成了多个赋值号
Stmt → LVal Assign Cond '?' Exp ':' Exp ';'//三目运算符
```

同时修改了以下文法：

```c++
Stmt → LVal Assign Exp ';'
Stmt → LVal Assign 'getint' '(' ')' ';'
Stmt → LVal Assign 'getchar' '(' ')' ';'
ForStmt → LVal Assign Exp
```

修改原有的程序，使之能满足上述规则

### 个人思路

首先，在词法分析部分，主要是处理那几个赋值号；本来是+的，现在要读下一位，看是不是=；对于/=，本人的除号在注释部分判断，所以是先判断注释，再判断/=，再判断/

其次，在语法分析部分，添加新的函数Assign()，处理赋值号的树；对于修改的文法，将以前”连接=号“直接改为”连接Assign()节点“

对于三目运算符，首先在LVal部分，由于Cond和Exp是基本重合的，所以必须采用回溯的算法（预读理论上可以卡）。预读Cond，如果下一位是"?"就说明是三目运算符，否则是LVal Assign Exp;

## 期末测试

题目构成：

1-5测试点仅涉及课下的文法，测试点6仅涉及课下的错误处理

之后的点涉及课上，一部分只有++另一部分只修改了if，最后还有一个测试点是完整的错误处理。

第二题是代码竞速，仅涉及线下文法，主要是对竞速封榜之后的排名。但是，pcode和LLVM的同学也需要提交，疑似送分？

第三题是简答题，给一个代码，选取其中1行代码问如何编译；同时还会问有什么优化方案（不管做没做优化都需要写）

添加了如下文法

```c++
Stmt → if '(' Btype Ident '=' InitVal ')' Stmt [else Stmt]
UnaryOp → "+" || "-" || "!" || "++"
```

其中，对于if语句括号内的定义，需要加入下一层符号表

"++"只是增加1，不改变符号原本的值

例如：

```c++
if(int a = b){
    //do sth
}else{
    //do sth
}
//相当于
{
    int a = b;
    if(a){
        //do sth
    }else{
        //do sth
    }
}
```

```c++
a = ++b;
//相当于
a = b + 1;//注意，不更改b本身的值
```

### 个人思路

难点主要在if文法上

对于++而言，需要在词法修改为"++"，然后在语法部分，语义部分和代码生成的UnaryOp分析部分添加”++“的分析，说白了就是一个变量加常数1。
注意 a = ++a这种情况

对于if而言，提示已经简化了，只需要在进入if的时候无脑加一层符号表即可（期末只判断运行和错误处理是否正确，不看语义分析的符号表序号）

对于if括号内的处理，我的最初处理复杂化了，直接去分析里面的内容，导致在代码生成部分处理很复杂（原本是封装好的genCondIR函数）

事实上，因为只看正确性不看语法树长什么样，可以直接把这个节点当作vardef。