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

// Block ::= "{" Stmt "}";
class BlockAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> stmt;

    void Dump(char *str, int & cnt_block, std::stack<int>* value_st) const override {
      char stmp[10] = " {\n";
      strcat(str, stmp);
      stmt->Dump(str, cnt_block, value_st);
      char stmp1[10] = "}\n";
      strcat(str, stmp1);
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

// Exp ::= AddExp;
class ExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> add_exp;

    void Dump(char *str, int & cnt_block, std::stack<int>* value_st) const override {
      add_exp->Dump(str, cnt_block, value_st);
    }
};

// PrimaryExp ::= "(" Exp ")" | Number;
class PrimaryExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> exp;
    int number, mode;

    void Dump(char *str, int & cnt_block, std::stack<int>* value_st) const override {
      if (mode == 1){
        exp->Dump(str, cnt_block, value_st);
      }
      else{
        (*value_st).push(number);
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
