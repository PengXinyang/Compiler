//
// Created by PengXinyang on 24-9-29.
//

#ifndef PARSERTREE_H
#define PARSERTREE_H
#include <cstring>
#include<string>
#include <utility>
#include <vector>

#include "../../Lexer/include/Lexer.h"
using namespace std;
struct TreeNode {
    Word word;
    int son_num = 0;//统计有几个孩子
    vector<TreeNode*> sonNode;
    TreeNode* father{};//父节点
    TreeNode() = default;
    explicit TreeNode(Word word){this->word=std::move(word);}
};

class ParserTree {
public:
    //连接语法树
    static TreeNode* catchTree(TreeNode* root, const Word &word);
    static TreeNode* catchTree(TreeNode* root, TreeNode* son);
    //建立语法树
    static TreeNode* createTree(const Word &word);
    //后序输出语法树
    static void printTree(FILE*fp, const TreeNode* root);
    //对于左递归文法，如果转换成课堂上的循环会导致语法树缺少，这个函数用于加起来
    static TreeNode* adjustTree(TreeNode* root,const Word &word);
    // 递归函数，查找子树中的所有叶子节点
    static void getLeafNodes(TreeNode* root,vector<TreeNode*>& leafNodes);
};



#endif //PARSERTREE_H
