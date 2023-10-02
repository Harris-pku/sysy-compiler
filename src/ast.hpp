#pragma once
#include <iostream>
#include <cstring>

// 所有 AST 的基类
class BaseAST {
  public:
    virtual ~BaseAST() = default;
    virtual int Cal() const = 0;
    virtual void Dump(char *str, int & cnt_block, int & ret_value) const = 0;
};

// CompUnit ::= FuncDef;
class CompUnitAST : public BaseAST {
  public:
    // 用智能指针管理对象
    std::unique_ptr<BaseAST> func_def;

    int Cal() const override { return 0; }

    void Dump(char *str, int & cnt_block, int & ret_value) const override {
      func_def->Dump(str, cnt_block, ret_value);
    }
};

// FuncDef ::= FuncType IDENT "(" ")" Block;
class FuncDefAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> func_type;
    std::string ident;
    std::unique_ptr<BaseAST> block;

    int Cal() const override { return 0; }

    void Dump(char *str, int & cnt_block, int & ret_value) const override {
      char stmp[20] = "fun @";
      strcat(stmp, ident.c_str());
      char stmp1[10] = "(): ";
      strcat(stmp, stmp1);
      strcat(str, stmp);
      func_type->Dump(str, cnt_block, ret_value);
      block->Dump(str, cnt_block, ret_value);
    }
};

// FuncType ::= "int";
class FuncTypeAST : public BaseAST {
  public:
    std::string type;

    int Cal() const override { return 0; }

    void Dump(char *str, int & cnt_block, int & ret_value) const override {
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

    int Cal() const override { return 0; }

    void Dump(char *str, int & cnt_block, int & ret_value) const override {
      char stmp[10] = " {\n";
      strcat(str, stmp);
      stmt->Dump(str, cnt_block, ret_value);
      char stmp1[10] = "}\n";
      strcat(str, stmp1);
    }
};

// Stmt ::= "return" Exp ";";
class StmtAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> exp;

    int Cal() const override { return 0; }

    void Dump(char *str, int & cnt_block, int & ret_value) const override {
      char stmp[20] = "%entry:\n";
      strcat(str, stmp);
      // int result = exp->Cal();
      exp->Dump(str, cnt_block, ret_value);
      char stmp1[20];
      if (cnt_block == 0) {
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
    std::unique_ptr<BaseAST> unary_exp;

    int Cal() const override {
      return unary_exp->Cal();
    }

    void Dump(char *str, int & cnt_block, int & ret_value) const override {
      unary_exp->Dump(str, cnt_block, ret_value);
    }
};

// PrimaryExp ::= "(" Exp ")" | Number;
class PrimaryExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> exp;
    int number, mode;

    int Cal() const override {
      if (mode == 1){
        return 0;
      }
      else{
        return number;
      }
    }

    void Dump(char *str, int & cnt_block, int & ret_value) const override {
      if (mode == 1){
        exp->Dump(str, cnt_block, ret_value);
      }
      else{
        ret_value = number;
      }
    }
};

// Number ::= INT_CONST;
class NumberAST : public BaseAST {
  public:
    int num;

    int Cal() const override{
      return num;
    }

    void Dump(char *str, int & cnt_block, int & ret_value) const override {
      ret_value = num;
    }
};

// UnaryExp ::= PrimaryExp | ("+" | "-" | "!") UnaryExp;
class UnaryExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> primary_exp;
    std::unique_ptr<BaseAST> unary_exp;
    int mode; char op;

    int Cal() const override{
      if (mode == 1){
        return primary_exp->Cal();
      }
      else{
        return 0;
      }
    }

    void Dump(char *str, int & cnt_block, int & ret_value) const override {
      if (mode == 1) {
        primary_exp->Dump(str, cnt_block, ret_value);
      }
      else{
        unary_exp->Dump(str, cnt_block, ret_value);
        char stmp[20];
        switch (mode){
          case 2: 
            break;
          case 3: 
            if (cnt_block == 0){
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
              sprintf(stmp, "  %%1 = eq %d, 0\n", ret_value);
            }
            else{
              sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt_block+1, cnt_block);
            }
            // std::cout << "unary_exp dump4" << std::endl;
            cnt_block++;
            strcat(str, stmp);
          default: break;
        }
      }
    }
};

// // MulExp ::= UnaryExp | MulExp ("*" | "/" | "%") UnaryExp;
// class MulExpAST : public BaseAST {
//   public:
//     std::unique_ptr<BaseAST> unary_exp1;
//     std::unique_ptr<BaseAST> mul_exp;
//     std::unique_ptr<BaseAST> unary_exp2;
//     int mode;

//     int Cal() const override { return 0; }

//     void Dump(char *str, int & cnt_block, int & ret_value) const override {
//       if (mode == 1){
//         unary_exp1->Dump(str, cnt_block, ret_value);
//       }
//       else{
//         mul_exp->Dump(str, cnt_block, ret_value);
//         unary_exp2->Dump(str, cnt_block, ret_value);
//       }
//     }
// };

// // AddExp ::= MulExp | AddExp ("+" | "-") MulExp;
// class AddExpAST : public BaseAST {
//   public:
//     std::unique_ptr<BaseAST> mul_exp1;
//     std::unique_ptr<BaseAST> add_exp;
//     std::unique_ptr<BaseAST> mul_exp2;
//     int mode;

//     int Cal() const override { return 0; }

//     void Dump(char *str, int & cnt_block, int & ret_value) const override {
//       if (mode == 1){
//         mul_exp1->Dump(str, cnt_block, ret_value);
//       }
//       else{
//         add_exp->Dump(str, cnt_block, ret_value);
//         mul_exp2->Dump(str, cnt_block, ret_value);
//       }
//     }
// };
