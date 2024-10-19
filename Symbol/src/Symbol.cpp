//
// Created by PengXinyang on 24-10-12.
//

#include <utility>

#include "../include/Symbol.h"

#include "../../Lexer/include/Lexer.h"

Symbol::Symbol() = default;
SymbolTable::SymbolTable() = default;
SymbolTable::SymbolTable(const int table_id, const shared_ptr<SymbolTable> &father_ptr) {
    this->table_id = table_id;
    this->father_ptr = father_ptr;
    this->layer = father_ptr->layer + 1;
}

void SymbolTable::print_symbol_table(FILE* fp) {
    for(const auto &token:token_vector) {
        string identType;
        if(symbol_table[token].is_func) {
            //如果是函数
            string bType;
            if(symbol_table[token].func_type==0) {
                bType = "Int";
            }else if(symbol_table[token].func_type==1) {
                bType = "Char";
            }else if(symbol_table[token].func_type==2) {
                bType = "Void";
            }
            identType = bType+"Func";
        }
        else {
            //如果不是函数，那么就是const，类型，和数组的组合
            string bType = symbol_table[token].btype==1?"Char":"Int";//0是int 1是char
            string Const = symbol_table[token].is_const?"Const":"";
            string Array = symbol_table[token].is_array?"Array":"";
            identType = Const+bType;
            identType.append(Array);
        }
        fprintf(fp,"%d %s %s\n",table_id,token.c_str(),identType.c_str());
    }
}

void SymbolTable::print_all_symbol_table(FILE *fp) {
    print_symbol_table(fp);
    for(const auto& children: children_ptr) {
        children->print_all_symbol_table(fp);
    }
}

bool SymbolTable::is_in_table(const string& token) {
    if(symbol_table.find(token) == symbol_table.end()) {
        return false;
    }
    return true;
}

bool SymbolTable::is_in_all_table(const string &token) {
    if(is_in_table(token)) {return true;}
    if(father_ptr==nullptr){ return false;}//当前节点没有，父节点为空，说明找到头了，确实没有这个符号
    return father_ptr->is_in_all_table(token);//当前表没有，父节点不为空，递归向上找
}

Symbol* SymbolTable::get_symbol(const string &token) {
    if(is_in_table(token)) {
        return &symbol_table[token];
    }
    return nullptr;
}

Symbol *SymbolTable::get_symbol_in_all_table(const string &token) {
    if(is_in_table(token)) {
        return &symbol_table[token];
    }
    if(father_ptr==nullptr) {return nullptr;}
    return father_ptr->get_symbol_in_all_table(token);
}

Symbol *SymbolTable::get_last_symbol() {
    if(token_vector.empty()) {return nullptr;}
    return &symbol_table[token_vector.back()];
}

bool SymbolTable::is_in_var_table(const string &token) {
    if(is_in_table(token)) {
        if(const Symbol* symbol = get_symbol(token); symbol != nullptr && symbol->is_var) {
            return true;
        }
    }
    return false;
}
bool SymbolTable::is_in_func_table(const string &token) {
    if(is_in_table(token)) {
        if(const Symbol* symbol = get_symbol(token); symbol != nullptr && symbol->is_func) {
            return true;
        }
    }
    return false;
}

bool SymbolTable::is_in_array_table(const string &token) {
    if(is_in_table(token)) {
        if(const Symbol* symbol = get_symbol(token); symbol != nullptr && symbol->is_array) {
            return true;
        }
    }
    return false;
}

bool SymbolTable::is_const(const string &token) {
    if(is_in_table(token)) {
        return symbol_table[token].is_const;
    }
    if(father_ptr==nullptr) {
        return false;
    }
    else return father_ptr->is_const(token);
}

void SymbolTable::add_symbol(const string& token, const Symbol &symbol) {
    token_vector.push_back(token);
    symbol_table[token] = symbol;
}

void SymbolTable::add_var_symbol(const Word& word, const int btype, const int var_value, const bool is_const) {
    Symbol symbol;
    symbol.token = word.word;
    symbol.lineNum = word.line_num;
    symbol.btype = btype;
    if(btype == 0) {
        symbol.int_var_value = var_value;
    }else {
        symbol.char_var_value = static_cast<char>(var_value);
    }
    symbol.is_const = is_const;
    symbol.is_var = true;
    symbol.type = 0;
    add_symbol(symbol.token, symbol);
}

void SymbolTable::add_func_symbol(const Word& word, const int func_type, const int param_num, const vector<int> &param_type) {
    Symbol symbol;
    symbol.token = word.word;
    symbol.lineNum = word.line_num;
    symbol.func_type = func_type;
    symbol.param_num = param_num;
    symbol.param_type = param_type;
    symbol.type = 2;
    symbol.is_func = true;
    add_symbol(symbol.token, symbol);
}

void SymbolTable::add_func_param_symbol(const Word &word, const int btype, const bool is_array) {
    Symbol symbol;
    symbol.token = word.word;
    symbol.lineNum = word.line_num;
    symbol.btype = btype;
    symbol.is_array = is_array;
    symbol.is_var = !is_array;//是数组就不是var，是var就不是数组
    symbol.type = is_array?1:0;
    symbol.is_func_param = true;
    add_symbol(symbol.token, symbol);
}

void SymbolTable::add_array_symbol(const Word& word, const int btype, const int dim, const bool is_const) {
    Symbol symbol;
    symbol.token = word.word;
    symbol.lineNum = word.line_num;
    symbol.dim = dim;
    symbol.btype = btype;
    symbol.is_const = is_const;
    symbol.is_array = true;
    symbol.type = 1;
    add_symbol(symbol.token, symbol);
}

//TODO:这里需要判断一下，是在本符号表找符号，还是在所有符号表中找符号
void SymbolTable::set_var_value(const string &token, const int var_value) {
    Symbol* symbol = get_symbol_in_all_table(token);
    if(symbol != nullptr && symbol->is_var) {
        if(symbol->btype ==0) {
            symbol->int_var_value = var_value;
        }
        else {
            symbol->char_var_value = static_cast<char>(var_value);
        }
    }
}

void SymbolTable::set_int_array_value(const string &token, const vector<int> &array_value) {
    Symbol* symbol = get_symbol_in_all_table(token);
    if(symbol != nullptr && symbol->is_array) {
        if(symbol->btype ==0) {
            symbol->array_int_values = array_value;
        }
    }
}

void SymbolTable::set_int_array_value(const string &token, const int array_value) {
    Symbol* symbol = get_symbol_in_all_table(token);
    if(symbol != nullptr && symbol->is_array) {
        if(symbol->btype ==0) {
            symbol->array_int_values.push_back(array_value);
        }
    }
}

void SymbolTable::set_char_array_value(const string &token, const vector<char> &array_value) {
    Symbol* symbol = get_symbol_in_all_table(token);
    if(symbol != nullptr && symbol->is_array) {
        if(symbol->btype ==1) {
            symbol->array_char_values = array_value;
        }
    }
}

void SymbolTable::set_char_array_value(const string &token, const char array_value) {
    Symbol* symbol = get_symbol_in_all_table(token);
    if(symbol != nullptr && symbol->is_array) {
        if(symbol->btype ==1) {
            symbol->array_char_values.push_back(array_value);
        }
    }
}

void SymbolTable::set_array_dim(const string &token, const int dim) {
    Symbol* symbol = get_symbol_in_all_table(token);
    if(symbol != nullptr && symbol->is_array) {
        symbol->dim = dim;
    }
}

void SymbolTable::set_is_return(const string &token, bool is_return) {
    Symbol* symbol = get_symbol_in_all_table(token);
    if(symbol != nullptr && symbol->is_func) {
        symbol->is_return = is_return;
    }
}

void SymbolTable::set_is_return(Symbol *func_symbol, const bool is_return) {
    if(func_symbol != nullptr && func_symbol->is_func) {
        func_symbol->is_return = is_return;
    }
}

void SymbolTable::set_func_return(const string &token, int func_value) {
    Symbol* symbol = get_symbol_in_all_table(token);
    if(symbol != nullptr && symbol->is_func) {
        symbol->func_value = func_value;
    }
}

void SymbolTable::set_func_return(Symbol *func_symbol, int func_value) {
    if(func_symbol != nullptr && func_symbol->is_func) {
        func_symbol->func_value = func_value;
    }
}
