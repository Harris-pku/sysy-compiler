#pragma once
#include <cassert>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <stack>
#include <vector>
#include <map>

// 所有 AST 的基类
class BaseAST {
  public:
    virtual ~BaseAST() = default;
    // virtual int Cal() const = 0;
    virtual void Dump(char *str, int & cnt_block, std::stack<int>* value_st) const = 0;
};

// CompUnit ::= FuncDef;
class CompUnitAST : public BaseAST {
  public:
    // 用智能指针管理对象
    std::unique_ptr<BaseAST> func_def;

    void Dump(char *str, int & cnt_block, std::stack<int>* value_st) const override {
      func_def->Dump(str, cnt_block, value_st);
    }
};

// Decl ::= ConstDecl | VarDecl;
class DeclAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> const_decl;

    void Dump(char *str, int & cnt_block, std::stack<int>* value_st) const override {
      const_decl->Dump(str, cnt_block, value_st);
    }
};

// ConstDecl ::= CONST INT ConstDefArr ";";
class ConstDeclAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> const_def_arr;

    void Dump(char *str, int & cnt_block, std::stack<int>* value_st) const override {
      const_def_arr->Dump(str, cnt_block, value_st);
    }
};

// ConstDefArr ::= ConstDefArr "," ConstDef | ConstDef; 
class ConstDefArrAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> const_def_arr;
    std::unique_ptr<BaseAST> const_def;
    int mode;

    void Dump(char *str, int & cnt_block, std::stack<int>* value_st) const override {
      switch (mode){
        case 1:
          const_def_arr->Dump(str, cnt_block, value_st);
          const_def->Dump(str, cnt_block, value_st);
          break;
        case 2:
          const_def->Dump(str, cnt_block, value_st);
          break;
        default:
          break;
      }
    }
};

// ConstDef ::= IDENT "=" ConstInitVal;
class ConstDefAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> const_init_val;
    std::string ident;

    void Dump(char *str, int & cnt_block, std::stack<int>* value_st) const override {
      const_init_val->Dump(str, cnt_block, value_st);
    }
};

// ConstInitVal ::= ConstExp;
class ConstInitValAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> const_exp;

    void Dump(char *str, int & cnt_block, std::stack<int>* value_st) const override {
      const_exp->Dump(str, cnt_block, value_st);
    }
};

// FuncDef ::= FuncType IDENT "(" ")" Block;
class FuncDefAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> func_type;
    std::string ident;
    std::unique_ptr<BaseAST> block;

    void Dump(char *str, int & cnt_block, std::stack<int>* value_st) const override {
      char stmp[20] = "fun @";
      strcat(stmp, ident.c_str());
      char stmp1[10] = "(): ";
      strcat(stmp, stmp1);
      strcat(str, stmp);
      func_type->Dump(str, cnt_block, value_st);
      block->Dump(str, cnt_block, value_st);
    }
};

// FuncType ::= "int";
class FuncTypeAST : public BaseAST {
  public:
    std::string type;

    void Dump(char *str, int & cnt_block, std::stack<int>* value_st) const override {
      if (type == "int"){
        char stmp[10] = "i32";
        strcat(str, stmp);
      }
    }
};

// Block ::= "{" BlockItemArr "}";
class BlockAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> block_item_arr;

    void Dump(char *str, int & cnt_block, std::stack<int>* value_st) const override {
      char stmp[10] = " {\n";
      strcat(str, stmp);
      block_item_arr->Dump(str, cnt_block, value_st);
      char stmp1[10] = "}\n";
      strcat(str, stmp1);
    }
};

// BlockItemArr ::= BlockItemArr Decl | BlockItemArr Stmt | ;
class BlockItemArrAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> block_item_arr;
    std::unique_ptr<BaseAST> decl;
    std::unique_ptr<BaseAST> stmt;
    int mode;

    void Dump(char *str, int & cnt_block, std::stack<int>* value_st) const override {
      // std::cout << "blockitemarr dump mode = " << mode << std::endl;
      switch (mode){
        case 1:
          block_item_arr->Dump(str, cnt_block, value_st);
          decl->Dump(str, cnt_block, value_st);
          break;
        case 2:
          block_item_arr->Dump(str, cnt_block, value_st);
          stmt->Dump(str, cnt_block, value_st);
          break;
        case 3:
          break;
        default:
          break;
      }
    }
};

// Stmt ::= "return" Exp ";";
class StmtAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> exp;

    void Dump(char *str, int & cnt_block, std::stack<int>* value_st) const override {
      char stmp[20] = "%entry:\n";
      strcat(str, stmp);
      // int result = exp->Cal();
      exp->Dump(str, cnt_block, value_st);
      char stmp1[20];
      if (cnt_block == 0) {
        int ret_value = (*value_st).top();
        (*value_st).pop();
        sprintf(stmp1, "  ret %d\n", ret_value);
      }
      else{
        sprintf(stmp1, "  ret %%%d\n", cnt_block);
      }
      strcat(str, stmp1);
    }
};

// Exp ::= LorExp;
class ExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> lor_exp;

    void Dump(char *str, int & cnt_block, std::stack<int>* value_st) const override {
      lor_exp->Dump(str, cnt_block, value_st);
    }
};

// LVal ::= IDENT;
class LValAST : public BaseAST {
  public:
    std::string ident;

    void Dump(char *str, int & cnt_block, std::stack<int>* value_st) const override {
      return;
    }
};

// PrimaryExp ::= "(" Exp ")" | LVal | Number;
class PrimaryExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> exp;
    std::unique_ptr<BaseAST> lval;
    int number, mode;

    void Dump(char *str, int & cnt_block, std::stack<int>* value_st) const override {
      switch (mode){
        case 1:
          exp->Dump(str, cnt_block, value_st);
          break;
        case 2:
          lval->Dump(str, cnt_block, value_st);
          break;
        case 3:
          (*value_st).push(number);
          break;
        default:
          break;
      }
    }
};

// Number ::= INT_CONST;
class NumberAST : public BaseAST {
  public:
    int num;

    void Dump(char *str, int & cnt_block, std::stack<int>* value_st) const override {
      (*value_st).push(num);
    }
};

// UnaryExp ::= PrimaryExp | ("+" | "-" | "!") UnaryExp;
class UnaryExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> primary_exp;
    std::unique_ptr<BaseAST> unary_exp;
    int mode; char op;

    void Dump(char *str, int & cnt_block, std::stack<int>* value_st) const override {
      if (mode == 1) {
        primary_exp->Dump(str, cnt_block, value_st);
      }
      else{
        unary_exp->Dump(str, cnt_block, value_st);
        char stmp[20]; int ret_value;
        switch (mode){
          case 2: 
            break;
          case 3: 
            if (cnt_block == 0){
              assert(!(*value_st).empty());
              ret_value = (*value_st).top();
              (*value_st).pop();
              sprintf(stmp, "  %%1 = sub 0, %d\n", ret_value);
            }
            else{
              sprintf(stmp, "  %%%d = sub 0, %%%d\n", cnt_block+1, cnt_block);
            }
            // std::cout << "unary_exp dump3" << std::endl;
            cnt_block++;
            strcat(str, stmp);
            break;
          case 4: 
            if (cnt_block == 0){
              assert(!(*value_st).empty());
              ret_value = (*value_st).top();
              (*value_st).pop();
              sprintf(stmp, "  %%1 = eq %d, 0\n", ret_value);
            }
            else{
              sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt_block+1, cnt_block);
            }
            // std::cout << "unary_exp dump4" << std::endl;
            cnt_block++;
            strcat(str, stmp);
          default: 
            break;
        }
      }
    }
};

// MulExp ::= UnaryExp | MulExp ("*" | "/" | "%") UnaryExp;
class MulExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> unary_exp;
    std::unique_ptr<BaseAST> mul_exp;
    int mode; char op;

    void Dump(char *str, int & cnt_block, std::stack<int>* value_st) const override {
      int value1, value2;
      char stmp[20];
      switch (mode){
        case 1:
          // std::cout << "mul_exp dump1" << std::endl;
          unary_exp->Dump(str, cnt_block, value_st);
          break;
        case 2:
          // std::cout << "mul_exp dump2" << std::endl;
          mul_exp->Dump(str, cnt_block, value_st);
          unary_exp->Dump(str, cnt_block, value_st);
          if ((*value_st).size() >= 2){
            value1 = (*value_st).top();
            (*value_st).pop();
            value2 = (*value_st).top();
            (*value_st).pop();
            sprintf(stmp, "  %%%d = mul %d, %d\n", cnt_block+1, value2, value1);
          }
          else if ((*value_st).size() == 1){
            value1 = (*value_st).top();
            (*value_st).pop();
            sprintf(stmp, "  %%%d = mul %d, %%%d\n", cnt_block+1, value1, cnt_block);
          }
          else{
            sprintf(stmp, "  %%%d = mul %%%d, %%%d\n", cnt_block+1, cnt_block, cnt_block-1);
          }
          strcat(str, stmp);
          cnt_block++;
          break;
        case 3:
          // std::cout << "mul_exp dump3" << std::endl;
          mul_exp->Dump(str, cnt_block, value_st);
          unary_exp->Dump(str, cnt_block, value_st);
          if ((*value_st).size() >= 2){
            value1 = (*value_st).top();
            (*value_st).pop();
            value2 = (*value_st).top();
            (*value_st).pop();
            sprintf(stmp, "  %%%d = div %d, %d\n", cnt_block+1, value2, value1);
          }
          else if ((*value_st).size() == 1){
            value1 = (*value_st).top();
            (*value_st).pop();
            sprintf(stmp, "  %%%d = div %d, %%%d\n", cnt_block+1, value1, cnt_block);
          }
          else{
            sprintf(stmp, "  %%%d = div %%%d, %%%d\n", cnt_block+1, cnt_block, cnt_block-1);
          }
          strcat(str, stmp);
          cnt_block++;
          break;
        case 4:
          // std::cout << "mul_exp dump4" << std::endl;
          mul_exp->Dump(str, cnt_block, value_st);
          unary_exp->Dump(str, cnt_block, value_st);
          if ((*value_st).size() >= 2){
            value1 = (*value_st).top();
            (*value_st).pop();
            value2 = (*value_st).top();
            (*value_st).pop();
            sprintf(stmp, "  %%%d = mod %d, %d\n", cnt_block+1, value2, value1);
          }
          else if ((*value_st).size() == 1){
            value1 = (*value_st).top();
            (*value_st).pop();
            sprintf(stmp, "  %%%d = mod %d, %%%d\n", cnt_block+1, value1, cnt_block);
          }
          else{
            sprintf(stmp, "  %%%d = mod %%%d, %%%d\n", cnt_block+1, cnt_block, cnt_block-1);
          }
          strcat(str, stmp);
          cnt_block++;
          break;
        default:
          break;
      }
    }
};

// AddExp ::= MulExp | AddExp ("+" | "-") MulExp;
class AddExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> mul_exp;
    std::unique_ptr<BaseAST> add_exp;
    int mode; char op;

    void Dump(char *str, int & cnt_block, std::stack<int>* value_st) const override {
      int value1, value2;
      char stmp[20];
      switch (mode){
        case 1:
          // std::cout << "add_exp dump1" << std::endl;
          mul_exp->Dump(str, cnt_block, value_st);
          break;
        case 2:
          // std::cout << "add_exp dump2" << std::endl;
          add_exp->Dump(str, cnt_block, value_st);
          mul_exp->Dump(str, cnt_block, value_st);
          if ((*value_st).size() >= 2){
            value1 = (*value_st).top();
            (*value_st).pop();
            value2 = (*value_st).top();
            (*value_st).pop();
            sprintf(stmp, "  %%%d = add %d, %d\n", cnt_block+1, value2, value1);
          }
          else if ((*value_st).size() == 1){
            value1 = (*value_st).top();
            (*value_st).pop();
            sprintf(stmp, "  %%%d = add %d, %%%d\n", cnt_block+1, value1, cnt_block);
          }
          else{
            sprintf(stmp, "  %%%d = add %%%d, %%%d\n", cnt_block+1, cnt_block, cnt_block-1);
          }
          strcat(str, stmp);
          cnt_block++;
          break;
        case 3:
          // std::cout << "add_exp dump3" << std::endl;
          add_exp->Dump(str, cnt_block, value_st);
          mul_exp->Dump(str, cnt_block, value_st);
          if ((*value_st).size() >= 2){
            value1 = (*value_st).top();
            (*value_st).pop();
            value2 = (*value_st).top();
            (*value_st).pop();
            sprintf(stmp, "  %%%d = sub %d, %d\n", cnt_block+1, value2, value1);
          }
          else if ((*value_st).size() == 1){
            value1 = (*value_st).top();
            (*value_st).pop();
            sprintf(stmp, "  %%%d = sub %d, %%%d\n", cnt_block+1, value1, cnt_block);
          }
          else{
            sprintf(stmp, "  %%%d = sub %%%d, %%%d\n", cnt_block+1, cnt_block, cnt_block-1);
          }
          strcat(str, stmp);
          cnt_block++;
          break;
        default:
          break;
      }
    }
};

// RelExp ::= AddExp | RelExp ("<" | ">" | "<=" | ">=") AddExp;
class RelExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> add_exp;
    std::unique_ptr<BaseAST> rel_exp;
    int mode;

    void Dump(char *str, int & cnt_block, std::stack<int>* value_st) const override {
      int value1, value2;
      char stmp[20];
      switch (mode){
        case 1:
          add_exp->Dump(str, cnt_block, value_st);
          break;
        case 2:
          rel_exp->Dump(str, cnt_block, value_st);
          add_exp->Dump(str, cnt_block, value_st);
          if ((*value_st).size() >= 2){
            value1 = (*value_st).top();
            (*value_st).pop();
            value2 = (*value_st).top();
            (*value_st).pop();
            sprintf(stmp, "  %%%d = lt %d, %d\n", cnt_block+1, value2, value1);
          }
          else if ((*value_st).size() == 1){
            value1 = (*value_st).top();
            (*value_st).pop();
            sprintf(stmp, "  %%%d = lt %d, %%%d\n", cnt_block+1, value1, cnt_block);
          }
          else{
            sprintf(stmp, "  %%%d = lt %%%d, %%%d\n", cnt_block+1, cnt_block, cnt_block-1);
          }
          strcat(str, stmp);
          cnt_block++;
          break;
        case 3:
          rel_exp->Dump(str, cnt_block, value_st);
          add_exp->Dump(str, cnt_block, value_st);
          if ((*value_st).size() >= 2){
            value1 = (*value_st).top();
            (*value_st).pop();
            value2 = (*value_st).top();
            (*value_st).pop();
            sprintf(stmp, "  %%%d = gt %d, %d\n", cnt_block+1, value2, value1);
          }
          else if ((*value_st).size() == 1){
            value1 = (*value_st).top();
            (*value_st).pop();
            sprintf(stmp, "  %%%d = gt %d, %%%d\n", cnt_block+1, value1, cnt_block);
          }
          else{
            sprintf(stmp, "  %%%d = gt %%%d, %%%d\n", cnt_block+1, cnt_block, cnt_block-1);
          }
          strcat(str, stmp);
          cnt_block++;
          break;
        case 4:
          rel_exp->Dump(str, cnt_block, value_st);
          add_exp->Dump(str, cnt_block, value_st);
          if ((*value_st).size() >= 2){
            value1 = (*value_st).top();
            (*value_st).pop();
            value2 = (*value_st).top();
            (*value_st).pop();
            sprintf(stmp, "  %%%d = le %d, %d\n", cnt_block+1, value2, value1);
          }
          else if ((*value_st).size() == 1){
            value1 = (*value_st).top();
            (*value_st).pop();
            sprintf(stmp, "  %%%d = le %d, %%%d\n", cnt_block+1, value1, cnt_block);
          }
          else{
            sprintf(stmp, "  %%%d = le %%%d, %%%d\n", cnt_block+1, cnt_block, cnt_block-1);
          }
          strcat(str, stmp);
          cnt_block++;
          break;
        case 5:
          rel_exp->Dump(str, cnt_block, value_st);
          add_exp->Dump(str, cnt_block, value_st);
          if ((*value_st).size() >= 2){
            value1 = (*value_st).top();
            (*value_st).pop();
            value2 = (*value_st).top();
            (*value_st).pop();
            sprintf(stmp, "  %%%d = ge %d, %d\n", cnt_block+1, value2, value1);
          }
          else if ((*value_st).size() == 1){
            value1 = (*value_st).top();
            (*value_st).pop();
            sprintf(stmp, "  %%%d = ge %d, %%%d\n", cnt_block+1, value1, cnt_block);
          }
          else{
            sprintf(stmp, "  %%%d = ge %%%d, %%%d\n", cnt_block+1, cnt_block, cnt_block-1);
          }
          strcat(str, stmp);
          cnt_block++;
          break;
        default:
          break;
      }
    }
};

// EqExp ::= RelExp | EqExp ("==" | "!=") RelExp;
class EqExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> rel_exp;
    std::unique_ptr<BaseAST> eq_exp;
    int mode;

    void Dump(char *str, int & cnt_block, std::stack<int>* value_st) const override {
      int value1, value2;
      char stmp[20];
      switch (mode){
        case 1:
          rel_exp->Dump(str, cnt_block, value_st);
          break;
        case 2:
          eq_exp->Dump(str, cnt_block, value_st);
          rel_exp->Dump(str, cnt_block, value_st);
          if ((*value_st).size() >= 2){
            value1 = (*value_st).top();
            (*value_st).pop();
            value2 = (*value_st).top();
            (*value_st).pop();
            sprintf(stmp, "  %%%d = eq %d, %d\n", cnt_block+1, value2, value1);
          }
          else if ((*value_st).size() == 1){
            value1 = (*value_st).top();
            (*value_st).pop();
            sprintf(stmp, "  %%%d = eq %d, %%%d\n", cnt_block+1, value1, cnt_block);
          }
          else{
            sprintf(stmp, "  %%%d = eq %%%d, %%%d\n", cnt_block+1, cnt_block, cnt_block-1);
          }
          strcat(str, stmp);
          cnt_block++;
          break;
        case 3:
          eq_exp->Dump(str, cnt_block, value_st);
          rel_exp->Dump(str, cnt_block, value_st);
          if ((*value_st).size() >= 2){
            value1 = (*value_st).top();
            (*value_st).pop();
            value2 = (*value_st).top();
            (*value_st).pop();
            sprintf(stmp, "  %%%d = ne %d, %d\n", cnt_block+1, value2, value1);
          }
          else if ((*value_st).size() == 1){
            value1 = (*value_st).top();
            (*value_st).pop();
            sprintf(stmp, "  %%%d = ne %d, %%%d\n", cnt_block+1, value1, cnt_block);
          }
          else{
            sprintf(stmp, "  %%%d = ne %%%d, %%%d\n", cnt_block+1, cnt_block, cnt_block-1);
          }
          strcat(str, stmp);
          cnt_block++;
          break;
        default: break;
      }
    }
};

// LAndExp ::= EqExp | LAndExp "&&" EqExp;
class LAndExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> eq_exp;
    std::unique_ptr<BaseAST> land_exp;
    int mode;

    void Dump(char *str, int & cnt_block, std::stack<int>* value_st) const override {
      int value1, value2;
      char stmp[20];
      switch (mode){
        case 1:
          eq_exp->Dump(str, cnt_block, value_st);
          break;
        case 2:
          land_exp->Dump(str, cnt_block, value_st);
          eq_exp->Dump(str, cnt_block, value_st);
          if ((*value_st).size() >= 2){
            value1 = (*value_st).top();
            (*value_st).pop();
            value2 = (*value_st).top();
            (*value_st).pop();
            sprintf(stmp, "  %%%d = eq %d, 0\n", cnt_block+1, value2);
            strcat(str, stmp);
            cnt_block++;
            sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt_block+1, cnt_block);
            strcat(str, stmp);
            cnt_block++;
            sprintf(stmp, "  %%%d = eq %d, 0\n", cnt_block+1, value1);
            strcat(str, stmp);
            cnt_block++;
            sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt_block+1, cnt_block);
            strcat(str, stmp);
            cnt_block++;
            sprintf(stmp, "  %%%d = and %%%d, %%%d\n", cnt_block+1, cnt_block, cnt_block-2);
          }
          else if ((*value_st).size() == 1){
            value1 = (*value_st).top();
            (*value_st).pop();
            sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt_block+1, cnt_block);
            strcat(str, stmp);
            cnt_block++;
            sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt_block+1, cnt_block);
            strcat(str, stmp);
            cnt_block++;
            sprintf(stmp, "  %%%d = eq %d, 0\n", cnt_block+1, value1);
            strcat(str, stmp);
            cnt_block++;
            sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt_block+1, cnt_block);
            strcat(str, stmp);
            cnt_block++;
            sprintf(stmp, "  %%%d = and %%%d, %%%d\n", cnt_block+1, cnt_block, cnt_block-2);            
          }
          else{
            sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt_block+1, cnt_block);
            strcat(str, stmp);
            cnt_block++;
            sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt_block+1, cnt_block);
            strcat(str, stmp);
            cnt_block++;
            sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt_block+1, cnt_block-3);
            strcat(str, stmp);
            cnt_block++;
            sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt_block+1, cnt_block);
            strcat(str, stmp);
            cnt_block++;
            sprintf(stmp, "  %%%d = and %%%d, %%%d\n", cnt_block+1, cnt_block, cnt_block-2);
          }
          strcat(str, stmp);
          cnt_block++;
          break;
        default: break;
      }
    }
};

// LOrExp ::= LAndExp | LOrExp "||" LAndExp;
class LOrExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> land_exp;
    std::unique_ptr<BaseAST> lor_exp;
    int mode;

    void Dump(char *str, int & cnt_block, std::stack<int>* value_st) const override {
      int value1, value2;
      char stmp[20];
      switch (mode){
        case 1:
          land_exp->Dump(str, cnt_block, value_st);
          break;
        case 2:
          lor_exp->Dump(str, cnt_block, value_st);
          land_exp->Dump(str, cnt_block, value_st);
          if ((*value_st).size() >= 2){
            value1 = (*value_st).top();
            (*value_st).pop();
            value2 = (*value_st).top();
            (*value_st).pop();
            sprintf(stmp, "  %%%d = eq %d, 0\n", cnt_block+1, value2);
            strcat(str, stmp);
            cnt_block++;
            sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt_block+1, cnt_block);
            strcat(str, stmp);
            cnt_block++;
            sprintf(stmp, "  %%%d = eq %d, 0\n", cnt_block+1, value1);
            strcat(str, stmp);
            cnt_block++;
            sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt_block+1, cnt_block);
            strcat(str, stmp);
            cnt_block++;
            sprintf(stmp, "  %%%d = or %%%d, %%%d\n", cnt_block+1, cnt_block, cnt_block-2);
          }
          else if ((*value_st).size() == 1){
            value1 = (*value_st).top();
            (*value_st).pop();
            sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt_block+1, cnt_block);
            strcat(str, stmp);
            cnt_block++;
            sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt_block+1, cnt_block);
            strcat(str, stmp);
            cnt_block++;
            sprintf(stmp, "  %%%d = eq %d, 0\n", cnt_block+1, value1);
            strcat(str, stmp);
            cnt_block++;
            sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt_block+1, cnt_block);
            strcat(str, stmp);
            cnt_block++;
            sprintf(stmp, "  %%%d = or %%%d, %%%d\n", cnt_block+1, cnt_block, cnt_block-2);            
          }
          else{
            sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt_block+1, cnt_block);
            strcat(str, stmp);
            cnt_block++;
            sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt_block+1, cnt_block);
            strcat(str, stmp);
            cnt_block++;
            sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt_block+1, cnt_block-3);
            strcat(str, stmp);
            cnt_block++;
            sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt_block+1, cnt_block);
            strcat(str, stmp);
            cnt_block++;
            sprintf(stmp, "  %%%d = or %%%d, %%%d\n", cnt_block+1, cnt_block, cnt_block-2);
          }
          strcat(str, stmp);
          cnt_block++;
          break;
        default: break;
      }
    }
};

// ConstExp ::= Exp;
class ConstExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> exp;

    void Dump(char *str, int & cnt_block, std::stack<int>* value_st) const override {
      exp->Dump(str, cnt_block, value_st);
    }
};
