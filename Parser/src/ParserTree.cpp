//
// Created by PengXinyang on 24-9-29.
//

#include "../include/ParserTree.h"
#include<cstdio>
TreeNode *ParserTree::catchTree(TreeNode* root, const Word &word) {
    auto* son = new TreeNode(word);
    root->sonNode.push_back(son);
    root->son_num++;
    return root;
}

TreeNode *ParserTree::createTree(const Word &word) {
    auto* root = new TreeNode(word);
    return root;
}

TreeNode *ParserTree::catchTree(TreeNode *root, TreeNode *son) {
    root->sonNode.push_back(son);
    root->son_num++;
    return root;
}
void ParserTree::printTree(FILE* fp, const TreeNode *root) {
    if (root != nullptr) {
        if(root->son_num==0) {
            //没有孩子，是子节点，要输出类型和名字
            fprintf(fp,"%s %s\n",root->word.word_type.c_str(),root->word.word.c_str());
        }
        else {
            for (int i = 0; i < root->son_num; i++) {
                printTree(fp,root->sonNode[i]);
            }
            if(root->word.word!="<BlockItem>"&&root->word.word!="<Decl>"&&root->word.word!="<Btype>") {
                fprintf(fp,"%s\n", root->word.word.c_str());
            }
        }
    }
}

TreeNode* ParserTree::adjustTree(TreeNode *root, const Word &word) {
    /*语法树的调整
     * 本来，对于左递归文法，是直接转成循环处理
     * 但是循环处理后，构建语法树会发现他们都作为同一类在同一层
     * 比如AddExp = AddExp + MulExp
     * 循环后，树AddExp下面一层有多个MulExp，每两个之间还有运算符
     * 而根据题目，题目希望建的树就是AddExp + MulExp这三部分
     * 所以要调整树，从右往左
     */
    TreeNode *parent = createTree(root->word);
    TreeNode *p = parent;
    for(int i=root->son_num-1; i>=0; i-=2) {
        if(i!=0) {
            auto* sonWord = new TreeNode(word);
            p->sonNode.push_back(sonWord);
            p->sonNode.push_back(root->sonNode[i-1]);
            p->son_num+=2;
        }
        p->sonNode.push_back(root->sonNode[i]);
        p->son_num++;
        p=p->sonNode[0];
    }
    delete root;
    return parent;
}
