#pragma once
#include <cassert>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <stack>
#include <vector>
#include <map>
#include <sym.hpp>

// 所有 AST 的基类
class BaseAST {
  public:
    virtual ~BaseAST() = default;
    virtual int Cal(std::map<std::string, sym_t>* val_ma) = 0;
    virtual void Dump(char *str, int & cnt, std::stack<num_t>* val_st,
                      std::map<std::string, sym_t>* val_ma) const = 0;
};

// CompUnit ::= FuncDef;
class CompUnitAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> func_def;

    int Cal(std::map<std::string, sym_t>* val_ma) override { return 0; }

    void Dump(char *str, int & cnt, std::stack<num_t>* val_st,
              std::map<std::string, sym_t>* val_ma) const override {
      func_def->Dump(str, cnt, val_st, val_ma);
    }
};

// Decl ::= ConstDecl | VarDecl;
class DeclAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> const_decl;
    std::unique_ptr<BaseAST> var_decl;
    int mode;

    int Cal(std::map<std::string, sym_t>* val_ma) override { return 0; }

    void Dump(char *str, int & cnt, std::stack<num_t>* val_st,
              std::map<std::string, sym_t>* val_ma) const override {
      switch (mode){
        case 1:
          const_decl->Dump(str, cnt, val_st, val_ma);
          break;
        case 2:
          var_decl->Dump(str, cnt, val_st, val_ma);
          break;
        default:
          break;
      }
    }
};

// ConstDecl ::= CONST INT ConstDefArr ";";
class ConstDeclAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> const_def_arr;

    int Cal(std::map<std::string, sym_t>* val_ma) override { return 0; }

    void Dump(char *str, int & cnt, std::stack<num_t>* val_st,
              std::map<std::string, sym_t>* val_ma) const override {
      const_def_arr->Dump(str, cnt, val_st, val_ma);
    }
};

// ConstDefArr ::= ConstDefArr "," ConstDef | ConstDef; 
class ConstDefArrAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> const_def_arr;
    std::unique_ptr<BaseAST> const_def;
    int mode;

    int Cal(std::map<std::string, sym_t>* val_ma) override { return 0; }

    void Dump(char *str, int & cnt, std::stack<num_t>* val_st,
              std::map<std::string, sym_t>* val_ma) const override {
      switch (mode){
        case 1:
          const_def_arr->Dump(str, cnt, val_st, val_ma);
          const_def->Dump(str, cnt, val_st, val_ma);
          break;
        case 2:
          const_def->Dump(str, cnt, val_st, val_ma);
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

    int Cal(std::map<std::string, sym_t>* val_ma) override { return 0; }

    void Dump(char *str, int & cnt, std::stack<num_t>* val_st,
              std::map<std::string, sym_t>* val_ma) const override {
      sym_t sym;
      sym.val_t = const_init_val->Cal(val_ma);
      sym.type = 0;
      (*val_ma)[ident] = sym;
    }
};

// ConstInitVal ::= ConstExp;
class ConstInitValAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> const_exp;

    int Cal(std::map<std::string, sym_t>* val_ma) override {
      int val = const_exp->Cal(val_ma);
      return val;
    }

    void Dump(char *str, int & cnt, std::stack<num_t>* val_st,
              std::map<std::string, sym_t>* val_ma) const override {
      const_exp->Dump(str, cnt, val_st, val_ma);
    }
};

// VarDecl ::= INT VarDefArr ";";
class VarDeclAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> var_def_arr;

    int Cal(std::map<std::string, sym_t>* val_ma) override { return 0; }

    void Dump(char *str, int & cnt, std::stack<num_t>* val_st,
              std::map<std::string, sym_t>* val_ma) const override {
      var_def_arr->Dump(str, cnt, val_st, val_ma);
    }
};

// VarDefArr ::= VarDefArr "," VarDef | VarDef;
class VarDefArrAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> var_def_arr;
    std::unique_ptr<BaseAST> var_def;
    int mode;

    int Cal(std::map<std::string, sym_t>* val_ma) override { return 0; }

    void Dump(char *str, int & cnt, std::stack<num_t>* val_st,
              std::map<std::string, sym_t>* val_ma) const override {
      switch (mode){
        case 1:
          var_def_arr->Dump(str, cnt, val_st, val_ma);
          var_def->Dump(str, cnt, val_st, val_ma);
          break;
        case 2:
          var_def->Dump(str, cnt, val_st, val_ma);
          break;
        default:
          break;
      }
    }
};

// VarDef ::= IDENT | IDENT "=" InitVal;
class VarDefAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> init_val;
    std::string ident;
    int mode;

    int Cal(std::map<std::string, sym_t>* val_ma) override { return 0; }

    void Dump(char *str, int & cnt, std::stack<num_t>* val_st,
              std::map<std::string, sym_t>* val_ma) const override {
      char stmp[50];
      sym_t sym;
      switch (mode){
        case 1:
          sym.val_t = 0;
          sym.type = 1;
          (*val_ma)[ident] = sym;
          sprintf(stmp, "  @%s = alloc i32\n", ident.c_str());
          strcat(str, stmp);
          sprintf(stmp, "  store 0, @%s\n\n", ident.c_str());
          strcat(str, stmp);
          break;
        case 2:
          sym.val_t = init_val->Cal(val_ma);
          sym.type = 1;
          (*val_ma)[ident] = sym;
          sprintf(stmp, "  @%s = alloc i32\n", ident.c_str());
          strcat(str, stmp);
          sprintf(stmp, "  store %d, @%s\n\n", sym.val_t, ident.c_str());
          strcat(str, stmp);
          break;
        default:
          break;
      }
    }
};

// InitVal ::= Exp;
class InitValAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> exp;

    int Cal(std::map<std::string, sym_t>* val_ma) override {
      int val = exp->Cal(val_ma);
      return val;
    }

    void Dump(char *str, int & cnt, std::stack<num_t>* val_st,
              std::map<std::string, sym_t>* val_ma) const override {
      exp->Dump(str, cnt, val_st, val_ma);
    }
};

// FuncDef ::= FuncType IDENT "(" ")" Block;
class FuncDefAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> func_type;
    std::string ident;
    std::unique_ptr<BaseAST> block;

    int Cal(std::map<std::string, sym_t>* val_ma) override { return 0; }

    void Dump(char *str, int & cnt, std::stack<num_t>* val_st,
              std::map<std::string, sym_t>* val_ma) const override {
      char stmp[20] = "fun @";
      strcat(stmp, ident.c_str());
      char stmp1[10] = "(): ";
      strcat(stmp, stmp1);
      strcat(str, stmp);
      func_type->Dump(str, cnt, val_st, val_ma);
      block->Dump(str, cnt, val_st, val_ma);
    }
};

// FuncType ::= "int";
class FuncTypeAST : public BaseAST {
  public:
    std::string type;

    int Cal(std::map<std::string, sym_t>* val_ma) override { return 0; }

    void Dump(char *str, int & cnt, std::stack<num_t>* val_st,
              std::map<std::string, sym_t>* val_ma) const override {
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

    int Cal(std::map<std::string, sym_t>* val_ma) override { return 0; }

    void Dump(char *str, int & cnt, std::stack<num_t>* val_st,
              std::map<std::string, sym_t>* val_ma) const override {
      char stmp[10] = " {\n";
      strcat(str, stmp);
      char stmp1[20] = "%entry:\n";
      strcat(str, stmp1);
      block_item_arr->Dump(str, cnt, val_st, val_ma);
      char stmp2[10] = "}\n";
      strcat(str, stmp2);
    }
};

// BlockItemArr ::= BlockItemArr Decl | BlockItemArr Stmt | ;
class BlockItemArrAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> block_item_arr;
    std::unique_ptr<BaseAST> decl;
    std::unique_ptr<BaseAST> stmt;
    int mode;

    int Cal(std::map<std::string, sym_t>* val_ma) override { return 0; }

    void Dump(char *str, int & cnt, std::stack<num_t>* val_st,
              std::map<std::string, sym_t>* val_ma) const override {
      // std::cout << "blockitemarr dump mode = " << mode << std::endl;
      switch (mode){
        case 1:
          block_item_arr->Dump(str, cnt, val_st, val_ma);
          decl->Dump(str, cnt, val_st, val_ma);
          break;
        case 2:
          block_item_arr->Dump(str, cnt, val_st, val_ma);
          stmt->Dump(str, cnt, val_st, val_ma);
          break;
        case 3:
          break;
        default:
          break;
      }
    }
};

// Stmt ::= LVal "=" Exp ";"
//        | ";"
//        | Exp ";"
//        | Block
//        | IF "(" Exp ")" Stmt
//        | IF "(" Exp ")" Stmt ELSE Stmt
//        | WHILE "(" Exp ")" Stmt
//        | BREAK ";"
//        | CONTINUE ";"
//        | RETURN ";"
//        | RETURN Exp ";";
class StmtAST : public BaseAST {
  public:
    // std::unique_ptr<BaseAST> lval;
    std::string ident;
    std::unique_ptr<BaseAST> exp;
    std::unique_ptr<BaseAST> block;
    std::unique_ptr<BaseAST> stmt;
    std::unique_ptr<BaseAST> else_stmt;
    int mode;

    int Cal(std::map<std::string, sym_t>* val_ma) override { return 0; }

    void Dump(char *str, int & cnt, std::stack<num_t>* val_st,
              std::map<std::string, sym_t>* val_ma) const override {
      char stmp[20];
      num_t tmpnum;
      int ret_value;
      switch (mode){
        case 1:
          sprintf(stmp, "  %%%d = load @%s\n", cnt+1, ident.c_str());
          cnt++;
          strcat(str, stmp);
          exp->Dump(str, cnt, val_st, val_ma);
          tmpnum = (*val_st).top();
          (*val_st).pop();
          ret_value = tmpnum.num_val;
          if (tmpnum.valid == 1){
            sprintf(stmp, "  %%%d = add 0, %d\n", cnt+1, ret_value);
            strcat(str, stmp);
            cnt++;
          }
          sprintf(stmp, "  store %%%d, @%s\n\n", cnt, ident.c_str());
          strcat(str, stmp);
          break;
        case 2:
          break;
        case 3:
          exp->Dump(str, cnt, val_st, val_ma);
          break;
        case 4:
          block->Dump(str, cnt, val_st, val_ma);
          break;
        case 5:
          exp->Dump(str, cnt, val_st, val_ma);
          stmt->Dump(str, cnt, val_st, val_ma);
          else_stmt->Dump(str, cnt, val_st, val_ma);
          break;
        case 6:
          exp->Dump(str, cnt, val_st, val_ma);
          stmt->Dump(str, cnt, val_st, val_ma);
          break;
        case 7:
          exp->Dump(str, cnt, val_st, val_ma);
          stmt->Dump(str, cnt, val_st, val_ma);
          break;
        case 8:
          break;
        case 9:
          break;
        case 10:
          sprintf(stmp, "  ret\n");
          strcat(str, stmp);
          break;
        case 11:
          exp->Dump(str, cnt, val_st, val_ma);
          tmpnum = (*val_st).top();
          ret_value = tmpnum.num_val;
          (*val_st).pop();
          if (tmpnum.valid == 1){
            sprintf(stmp, "  ret %d\n", ret_value);
          }
          else{
            sprintf(stmp, "  ret %%%d\n", ret_value);
          }
          strcat(str, stmp);
          break;
        default:
          assert(false);
          break;
      }
    }
};

// Exp ::= LorExp;
class ExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> lor_exp;

    int Cal(std::map<std::string, sym_t>* val_ma) override {
      int val = lor_exp->Cal(val_ma);
      return val;
    }

    void Dump(char *str, int & cnt, std::stack<num_t>* val_st,
              std::map<std::string, sym_t>* val_ma) const override {
      lor_exp->Dump(str, cnt, val_st, val_ma);
    }
};

// LVal ::= IDENT;
class LValAST : public BaseAST {
  public:
    std::string ident;

    int Cal(std::map<std::string, sym_t>* val_ma) override {
      assert((*val_ma).count(ident) == 1);
      sym_t sym = (*val_ma)[ident];
      return sym.val_t;
    }

    void Dump(char *str, int & cnt, std::stack<num_t>* val_st,
              std::map<std::string, sym_t>* val_ma) const override {
      if ((*val_ma).count(ident) == 1){
        sym_t sym = (*val_ma)[ident];
        if (sym.type == 0){
          num_t tmpnum;
          tmpnum.num_val = sym.val_t;
          tmpnum.valid = 1;
          (*val_st).push(tmpnum);
        }
        else{
          char stmp[20];
          sprintf(stmp, "  %%%d = load @%s\n", cnt+1, ident.c_str());
          cnt++;
          strcat(str, stmp);
          num_t tmpnum;
          tmpnum.num_val = cnt;
          tmpnum.valid = 0;
          (*val_st).push(tmpnum);
        }
      }
    }
};

// PrimaryExp ::= "(" Exp ")" | LVal | Number;
class PrimaryExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> exp;
    std::unique_ptr<BaseAST> lval;
    int number, mode;

    int Cal(std::map<std::string, sym_t>* val_ma) override {
      int val = 0;
      switch (mode){
        case 1:
          val = exp->Cal(val_ma);
          break;
        case 2:
          val = lval->Cal(val_ma);
          break;
        case 3:
          val = number;
          break;
        default:
          break;
      }
      return val;
    }

    void Dump(char *str, int & cnt, std::stack<num_t>* val_st,
              std::map<std::string, sym_t>* val_ma) const override {
      switch (mode){
        case 1:
          exp->Dump(str, cnt, val_st, val_ma);
          break;
        case 2:
          lval->Dump(str, cnt, val_st, val_ma);
          break;
        case 3:
          num_t tmpnum;
          tmpnum.num_val = number;
          tmpnum.valid = 1;
          (*val_st).push(tmpnum);
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
    int Cal(std::map<std::string, sym_t>* val_ma) override {
      return num;
    }

    void Dump(char *str, int & cnt, std::stack<num_t>* val_st,
              std::map<std::string, sym_t>* val_ma) const override {
      num_t tmpnum;
      tmpnum.num_val = num;
      tmpnum.valid = 1;
      (*val_st).push(tmpnum);

    }
};

// UnaryExp ::= PrimaryExp | ("+" | "-" | "!") UnaryExp;
class UnaryExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> primary_exp;
    std::unique_ptr<BaseAST> unary_exp;
    int mode;

    int Cal(std::map<std::string, sym_t>* val_ma) override {
      int val = 0;
      switch (mode){
        case 1:
          val = primary_exp->Cal(val_ma);
          break;
        case 2:
          val = unary_exp->Cal(val_ma);
          break;
        case 3:
          val = unary_exp->Cal(val_ma);
          val = -val;
          break;
        case 4:
          val = unary_exp->Cal(val_ma);
          val = !val;
          break;
        default:
          break;
      }
      return val;
    }

    void Dump(char *str, int & cnt, std::stack<num_t>* val_st,
              std::map<std::string, sym_t>* val_ma) const override {
      if (mode == 1){
        primary_exp->Dump(str, cnt, val_st, val_ma);
      }
      else{
        unary_exp->Dump(str, cnt, val_st, val_ma);
        char stmp[20];
        num_t tmpnum;
        int value;
        // std::cout << "mode == " << mode << std::endl;
        switch (mode){
          case 2:
            break;
          case 3:
            tmpnum = (*val_st).top();
            value = tmpnum.num_val;
            (*val_st).pop();
            if (tmpnum.valid == 1){
              sprintf(stmp, "  %%%d = sub 0, %d\n", cnt+1, value);
            }
            else{
              sprintf(stmp, "  %%%d = sub 0, %%%d\n", cnt+1, value);
            }
            // std::cout << "unary_exp dump3" << std::endl;
            cnt++;
            strcat(str, stmp);
            tmpnum.valid = 0;
            tmpnum.num_val = cnt;
            (*val_st).push(tmpnum);
            break;
          case 4:
            tmpnum = (*val_st).top();
            value = tmpnum.num_val;
            (*val_st).pop();
            if (tmpnum.valid == 1){
              sprintf(stmp, "  %%%d = eq %d, 0\n", cnt+1, value);
            }
            else{
              sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt+1, value);
            }
            // std::cout << "unary_exp dump4" << std::endl;
            cnt++;
            strcat(str, stmp);
            tmpnum.valid = 0;
            tmpnum.num_val = cnt;
            (*val_st).push(tmpnum);
            break;
          default:
            assert(false);
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
    int mode;

    int Cal(std::map<std::string, sym_t>* val_ma) override {
      int val = 0, valx, valy;
      switch (mode){
        case 1:
          val = unary_exp->Cal(val_ma);
          break;
        case 2:
          valx = mul_exp->Cal(val_ma);
          valy = unary_exp->Cal(val_ma);
          val = valx * valy;
          break;
        case 3:
          valx = mul_exp->Cal(val_ma);
          valy = unary_exp->Cal(val_ma);
          val = valx / valy;
          break;
        case 4:
          valx = mul_exp->Cal(val_ma);
          valy = unary_exp->Cal(val_ma);
          val = valx % valy;
          break;
        default:
          break;
      }
      return val;
    }

    void Dump(char *str, int & cnt, std::stack<num_t>* val_st,
              std::map<std::string, sym_t>* val_ma) const override {
      num_t tmpnum1, tmpnum2;
      int value1, value2;
      char stmp[20];
      switch (mode){
        case 1:
          // std::cout << "mul_exp dump1" << std::endl;
          unary_exp->Dump(str, cnt, val_st, val_ma);
          break;
        case 2:
          // std::cout << "mul_exp dump2" << std::endl;
          mul_exp->Dump(str, cnt, val_st, val_ma);
          unary_exp->Dump(str, cnt, val_st, val_ma);
          tmpnum1 = (*val_st).top();
          value1 = tmpnum1.num_val;
          (*val_st).pop();
          tmpnum2 = (*val_st).top();
          value2 = tmpnum2.num_val;
          (*val_st).pop();
          if (tmpnum1.valid == 1){
            if (tmpnum2.valid == 1){
              sprintf(stmp, "  %%%d = mul %d, %d\n", cnt+1, value2, value1);
            }
            else{
              sprintf(stmp, "  %%%d = mul %%%d, %d\n", cnt+1, value2, value1);
            }
          }
          else{
            if (tmpnum2.valid == 1){
              sprintf(stmp, "  %%%d = mul %d, %%%d\n", cnt+1, value2, value1);
            }
            else{
              sprintf(stmp, "  %%%d = mul %%%d, %%%d\n", cnt+1, value2, value1);
            }
          }
          strcat(str, stmp);
          cnt++;
          tmpnum1.num_val = cnt;
          tmpnum1.valid = 0;
          (*val_st).push(tmpnum1);
          break;
        case 3:
          // std::cout << "mul_exp dump3" << std::endl;
          mul_exp->Dump(str, cnt, val_st, val_ma);
          unary_exp->Dump(str, cnt, val_st, val_ma);
          tmpnum1 = (*val_st).top();
          value1 = tmpnum1.num_val;
          (*val_st).pop();
          tmpnum2 = (*val_st).top();
          value2 = tmpnum2.num_val;
          (*val_st).pop();
          if (tmpnum1.valid == 1){
            if (tmpnum2.valid == 1){
              sprintf(stmp, "  %%%d = div %d, %d\n", cnt+1, value2, value1);
            }
            else{
              sprintf(stmp, "  %%%d = div %%%d, %d\n", cnt+1, value2, value1);
            }
          }
          else{
            if (tmpnum2.valid == 1){
              sprintf(stmp, "  %%%d = div %d, %%%d\n", cnt+1, value2, value1);
            }
            else{
              sprintf(stmp, "  %%%d = div %%%d, %%%d\n", cnt+1, value2, value1);
            }
          }
          strcat(str, stmp);
          cnt++;
          tmpnum1.num_val = cnt;
          tmpnum1.valid = 0;
          (*val_st).push(tmpnum1);
          break;
        case 4:
          // std::cout << "mul_exp dump4" << std::endl;
          mul_exp->Dump(str, cnt, val_st, val_ma);
          unary_exp->Dump(str, cnt, val_st, val_ma);
          tmpnum1 = (*val_st).top();
          value1 = tmpnum1.num_val;
          (*val_st).pop();
          tmpnum2 = (*val_st).top();
          value2 = tmpnum2.num_val;
          (*val_st).pop();
          if (tmpnum1.valid == 1){
            if (tmpnum2.valid == 1){
              sprintf(stmp, "  %%%d = mod %d, %d\n", cnt+1, value2, value1);
            }
            else{
              sprintf(stmp, "  %%%d = mod %%%d, %d\n", cnt+1, value2, value1);
            }
          }
          else{
            if (tmpnum2.valid == 1){
              sprintf(stmp, "  %%%d = mod %d, %%%d\n", cnt+1, value2, value1);
            }
            else{
              sprintf(stmp, "  %%%d = mod %%%d, %%%d\n", cnt+1, value2, value1);
            }
          }
          strcat(str, stmp);
          cnt++;
          tmpnum1.num_val = cnt;
          tmpnum1.valid = 0;
          (*val_st).push(tmpnum1);
          break;
        default:
          assert(false);
          break;
      }
    }
};

// AddExp ::= MulExp | AddExp ("+" | "-") MulExp;
class AddExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> mul_exp;
    std::unique_ptr<BaseAST> add_exp;
    int mode;

    int Cal(std::map<std::string, sym_t>* val_ma) override {
      int val = 0, valx, valy;
      switch (mode){
        case 1:
          val = mul_exp->Cal(val_ma);
          break;
        case 2:
          valx = add_exp->Cal(val_ma);
          valy = mul_exp->Cal(val_ma);
          val = valx + valy;
          break;
        case 3:
          valx = add_exp->Cal(val_ma);
          valy = mul_exp->Cal(val_ma);
          val = valx - valy;
          break;
        default:
          break;
      }
      return val;
    }

    void Dump(char *str, int & cnt, std::stack<num_t>* val_st,
              std::map<std::string, sym_t>* val_ma) const override {
      num_t tmpnum1, tmpnum2;
      int value1, value2;
      char stmp[20];
      switch (mode){
        case 1:
          // std::cout << "add_exp dump1" << std::endl;
          mul_exp->Dump(str, cnt, val_st, val_ma);
          break;
        case 2:
          // std::cout << "add_exp dump2" << std::endl;
          add_exp->Dump(str, cnt, val_st, val_ma);
          mul_exp->Dump(str, cnt, val_st, val_ma);
          tmpnum1 = (*val_st).top();
          value1 = tmpnum1.num_val;
          (*val_st).pop();
          tmpnum2 = (*val_st).top();
          value2 = tmpnum2.num_val;
          (*val_st).pop();
          if (tmpnum1.valid == 1){
            if (tmpnum2.valid == 1){
              sprintf(stmp, "  %%%d = add %d, %d\n", cnt+1, value2, value1);
            }
            else{
              sprintf(stmp, "  %%%d = add %%%d, %d\n", cnt+1, value2, value1);
            }
          }
          else{
            if (tmpnum2.valid == 1){
              sprintf(stmp, "  %%%d = add %d, %%%d\n", cnt+1, value2, value1);
            }
            else{
              sprintf(stmp, "  %%%d = add %%%d, %%%d\n", cnt+1, value2, value1);
            }
          }
          strcat(str, stmp);
          cnt++;
          tmpnum1.num_val = cnt;
          tmpnum1.valid = 0;
          (*val_st).push(tmpnum1);
          break;
        case 3:
          // std::cout << "add_exp dump3" << std::endl;
          add_exp->Dump(str, cnt, val_st, val_ma);
          mul_exp->Dump(str, cnt, val_st, val_ma);
          tmpnum1 = (*val_st).top();
          value1 = tmpnum1.num_val;
          (*val_st).pop();
          tmpnum2 = (*val_st).top();
          value2 = tmpnum2.num_val;
          (*val_st).pop();
          if (tmpnum1.valid == 1){
            if (tmpnum2.valid == 1){
              sprintf(stmp, "  %%%d = sub %d, %d\n", cnt+1, value2, value1);
            }
            else{
              sprintf(stmp, "  %%%d = sub %%%d, %d\n", cnt+1, value2, value1);
            }
          }
          else{
            if (tmpnum2.valid == 1){
              sprintf(stmp, "  %%%d = sub %d, %%%d\n", cnt+1, value2, value1);
            }
            else{
              sprintf(stmp, "  %%%d = sub %%%d, %%%d\n", cnt+1, value2, value1);
            }
          }
          strcat(str, stmp);
          cnt++;
          tmpnum1.num_val = cnt;
          tmpnum1.valid = 0;
          (*val_st).push(tmpnum1);
          break;
        default:
          assert(false);
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
    
    int Cal(std::map<std::string, sym_t>* val_ma) override {
      int val = 0, valx, valy;
      switch (mode){
        case 1:
          val = add_exp->Cal(val_ma);
          break;
        case 2:
          valx = rel_exp->Cal(val_ma);
          valy = add_exp->Cal(val_ma);
          val = valx < valy;
          break;
        case 3:
          valx = rel_exp->Cal(val_ma);
          valy = add_exp->Cal(val_ma);
          val = valx > valy;
          break;
        case 4:
          valx = rel_exp->Cal(val_ma);
          valy = add_exp->Cal(val_ma);
          val = valx <= valy;
          break;
        case 5:
          valx = rel_exp->Cal(val_ma);
          valy = add_exp->Cal(val_ma);
          val = valx >= valy;
          break;
        default:
          break;
      }
      return val;
    }

    void Dump(char *str, int & cnt, std::stack<num_t>* val_st,
              std::map<std::string, sym_t>* val_ma) const override {
      num_t tmpnum1, tmpnum2;
      int value1, value2;
      char stmp[20];
      switch (mode){
        case 1:
          add_exp->Dump(str, cnt, val_st, val_ma);
          break;
        case 2:
          rel_exp->Dump(str, cnt, val_st, val_ma);
          add_exp->Dump(str, cnt, val_st, val_ma);
          tmpnum1 = (*val_st).top();
          value1 = tmpnum1.num_val;
          (*val_st).pop();
          tmpnum2 = (*val_st).top();
          value2 = tmpnum2.num_val;
          (*val_st).pop();
          if (tmpnum1.valid == 1){
            if (tmpnum2.valid == 1){
              sprintf(stmp, "  %%%d = lt %d, %d\n", cnt+1, value2, value1);
            }
            else{
              sprintf(stmp, "  %%%d = lt %%%d, %d\n", cnt+1, value2, value1);
            }
          }
          else{
            if (tmpnum2.valid == 1){
              sprintf(stmp, "  %%%d = lt %d, %%%d\n", cnt+1, value2, value1);
            }
            else{
              sprintf(stmp, "  %%%d = lt %%%d, %%%d\n", cnt+1, value2, value1);
            }
          }
          strcat(str, stmp);
          cnt++;
          tmpnum1.num_val = cnt;
          tmpnum1.valid = 0;
          (*val_st).push(tmpnum1);
          break;
        case 3:
          rel_exp->Dump(str, cnt, val_st, val_ma);
          add_exp->Dump(str, cnt, val_st, val_ma);
          tmpnum1 = (*val_st).top();
          value1 = tmpnum1.num_val;
          (*val_st).pop();
          tmpnum2 = (*val_st).top();
          value2 = tmpnum2.num_val;
          (*val_st).pop();
          if (tmpnum1.valid == 1){
            if (tmpnum2.valid == 1){
              sprintf(stmp, "  %%%d = gt %d, %d\n", cnt+1, value2, value1);
            }
            else{
              sprintf(stmp, "  %%%d = gt %%%d, %d\n", cnt+1, value2, value1);
            }
          }
          else{
            if (tmpnum2.valid == 1){
              sprintf(stmp, "  %%%d = gt %d, %%%d\n", cnt+1, value2, value1);
            }
            else{
              sprintf(stmp, "  %%%d = gt %%%d, %%%d\n", cnt+1, value2, value1);
            }
          }
          strcat(str, stmp);
          cnt++;
          tmpnum1.num_val = cnt;
          tmpnum1.valid = 0;
          (*val_st).push(tmpnum1);
          break;
        case 4:
          rel_exp->Dump(str, cnt, val_st, val_ma);
          add_exp->Dump(str, cnt, val_st, val_ma);
          tmpnum1 = (*val_st).top();
          value1 = tmpnum1.num_val;
          (*val_st).pop();
          tmpnum2 = (*val_st).top();
          value2 = tmpnum2.num_val;
          (*val_st).pop();
          if (tmpnum1.valid == 1){
            if (tmpnum2.valid == 1){
              sprintf(stmp, "  %%%d = le %d, %d\n", cnt+1, value2, value1);
            }
            else{
              sprintf(stmp, "  %%%d = le %%%d, %d\n", cnt+1, value2, value1);
            }
          }
          else{
            if (tmpnum2.valid == 1){
              sprintf(stmp, "  %%%d = le %d, %%%d\n", cnt+1, value2, value1);
            }
            else{
              sprintf(stmp, "  %%%d = le %%%d, %%%d\n", cnt+1, value2, value1);
            }
          }
          strcat(str, stmp);
          cnt++;
          tmpnum1.num_val = cnt;
          tmpnum1.valid = 0;
          (*val_st).push(tmpnum1);
          break;
        case 5:
          rel_exp->Dump(str, cnt, val_st, val_ma);
          add_exp->Dump(str, cnt, val_st, val_ma);
          tmpnum1 = (*val_st).top();
          value1 = tmpnum1.num_val;
          (*val_st).pop();
          tmpnum2 = (*val_st).top();
          value2 = tmpnum2.num_val;
          (*val_st).pop();
          if (tmpnum1.valid == 1){
            if (tmpnum2.valid == 1){
              sprintf(stmp, "  %%%d = ge %d, %d\n", cnt+1, value2, value1);
            }
            else{
              sprintf(stmp, "  %%%d = ge %%%d, %d\n", cnt+1, value2, value1);
            }
          }
          else{
            if (tmpnum2.valid == 1){
              sprintf(stmp, "  %%%d = ge %d, %%%d\n", cnt+1, value2, value1);
            }
            else{
              sprintf(stmp, "  %%%d = ge %%%d, %%%d\n", cnt+1, value2, value1);
            }
          }
          strcat(str, stmp);
          cnt++;
          tmpnum1.num_val = cnt;
          tmpnum1.valid = 0;
          (*val_st).push(tmpnum1);
          break;
        default:
          assert(false);
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

    int Cal(std::map<std::string, sym_t>* val_ma) override {
      int val = 0, valx, valy;
      switch (mode){
        case 1:
          val = rel_exp->Cal(val_ma);
          break;
        case 2:
          valx = eq_exp->Cal(val_ma);
          valy = rel_exp->Cal(val_ma);
          val = valx == valy;
          break;
        case 3:
          valx = eq_exp->Cal(val_ma);
          valy = rel_exp->Cal(val_ma);
          val = valx != valy;
          break;
        default:
          break;
      }
      return val;
    }

    void Dump(char *str, int & cnt, std::stack<num_t>* val_st,
              std::map<std::string, sym_t>* val_ma) const override {
      num_t tmpnum1, tmpnum2;
      int value1, value2;
      char stmp[20];
      switch (mode){
        case 1:
          rel_exp->Dump(str, cnt, val_st, val_ma);
          break;
        case 2:
          eq_exp->Dump(str, cnt, val_st, val_ma);
          rel_exp->Dump(str, cnt, val_st, val_ma);
          tmpnum1 = (*val_st).top();
          value1 = tmpnum1.num_val;
          (*val_st).pop();
          tmpnum2 = (*val_st).top();
          value2 = tmpnum2.num_val;
          (*val_st).pop();
          if (tmpnum1.valid == 1){
            if (tmpnum2.valid == 1){
              sprintf(stmp, "  %%%d = eq %d, %d\n", cnt+1, value2, value1);
            }
            else{
              sprintf(stmp, "  %%%d = eq %%%d, %d\n", cnt+1, value2, value1);
            }
          }
          else{
            if (tmpnum2.valid == 1){
              sprintf(stmp, "  %%%d = eq %d, %%%d\n", cnt+1, value2, value1);
            }
            else{
              sprintf(stmp, "  %%%d = eq %%%d, %%%d\n", cnt+1, value2, value1);
            }
          }
          strcat(str, stmp);
          cnt++;
          tmpnum1.num_val = cnt;
          tmpnum1.valid = 0;
          (*val_st).push(tmpnum1);
          break;
        case 3:
          eq_exp->Dump(str, cnt, val_st, val_ma);
          rel_exp->Dump(str, cnt, val_st, val_ma);
          tmpnum1 = (*val_st).top();
          value1 = tmpnum1.num_val;
          (*val_st).pop();
          tmpnum2 = (*val_st).top();
          value2 = tmpnum2.num_val;
          (*val_st).pop();
          if (tmpnum1.valid == 1){
            if (tmpnum2.valid == 1){
              sprintf(stmp, "  %%%d = ne %d, %d\n", cnt+1, value2, value1);
            }
            else{
              sprintf(stmp, "  %%%d = ne %%%d, %d\n", cnt+1, value2, value1);
            }
          }
          else{
            if (tmpnum2.valid == 1){
              sprintf(stmp, "  %%%d = ne %d, %%%d\n", cnt+1, value2, value1);
            }
            else{
              sprintf(stmp, "  %%%d = ne %%%d, %%%d\n", cnt+1, value2, value1);
            }
          }
          strcat(str, stmp);
          cnt++;
          tmpnum1.num_val = cnt;
          tmpnum1.valid = 0;
          (*val_st).push(tmpnum1);
          break;
        default:
          assert(false);
          break;
      }
    }
};

// LAndExp ::= EqExp | LAndExp "&&" EqExp;
class LAndExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> eq_exp;
    std::unique_ptr<BaseAST> land_exp;
    int mode;

    int Cal(std::map<std::string, sym_t>* val_ma) override {
      int val = 0, valx, valy;
      switch (mode){
        case 1:
          val = eq_exp->Cal(val_ma);
          break;
        case 2:
          valx = land_exp->Cal(val_ma);
          valy = eq_exp->Cal(val_ma);
          val = valx && valy;
          break;
        default:
          break;
      }
      return val;
    }

    void Dump(char *str, int & cnt, std::stack<num_t>* val_st,
              std::map<std::string, sym_t>* val_ma) const override {
      num_t tmpnum1, tmpnum2;
      int value1, value2;
      char stmp[20];
      switch (mode){
        case 1:
          eq_exp->Dump(str, cnt, val_st, val_ma);
          break;
        case 2:
          land_exp->Dump(str, cnt, val_st, val_ma);
          eq_exp->Dump(str, cnt, val_st, val_ma);
          tmpnum1 = (*val_st).top();
          value1 = tmpnum1.num_val;
          (*val_st).pop();
          tmpnum2 = (*val_st).top();
          value2 = tmpnum2.num_val;
          (*val_st).pop();
          if (tmpnum1.valid == 1){
            if (tmpnum2.valid == 1){
              sprintf(stmp, "  %%%d = eq %d, 0\n", cnt+1, value2);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt+1, cnt);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = eq %d, 0\n", cnt+1, value1);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt+1, cnt);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = and %%%d, %%%d\n", cnt+1, cnt, cnt-2);
            }
            else{
              sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt+1, value2);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt+1, cnt);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = eq %d, 0\n", cnt+1, value1);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt+1, cnt);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = and %%%d, %%%d\n", cnt+1, cnt, cnt-2);
            }
          }
          else{
            if (tmpnum2.valid == 1){
              sprintf(stmp, "  %%%d = eq %d, 0\n", cnt+1, value2);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt+1, cnt);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt+1, value1);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt+1, cnt);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = and %%%d, %%%d\n", cnt+1, cnt, cnt-2);
            }
            else{
              sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt+1, value2);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt+1, cnt);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt+1, value1);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt+1, cnt);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = and %%%d, %%%d\n", cnt+1, cnt, cnt-2);
            }
          }
          strcat(str, stmp);
          cnt++;
          tmpnum1.num_val = cnt;
          tmpnum1.valid = 0;
          (*val_st).push(tmpnum1);
          break;
        default:
          assert(false);
          break;
      }
    }
};

// LOrExp ::= LAndExp | LOrExp "||" LAndExp;
class LOrExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> land_exp;
    std::unique_ptr<BaseAST> lor_exp;
    int mode;

    int Cal(std::map<std::string, sym_t>* val_ma) override {
      int val = 0, valx, valy;
      switch (mode){
        case 1:
          val = land_exp->Cal(val_ma);
          break;
        case 2:
          valx = lor_exp->Cal(val_ma);
          valy = land_exp->Cal(val_ma);
          val = valx || valy;
          break;
        default:
          break;
      }
      return val;
    }

    void Dump(char *str, int & cnt, std::stack<num_t>* val_st,
              std::map<std::string, sym_t>* val_ma) const override {
      num_t tmpnum1, tmpnum2;
      int value1, value2;
      char stmp[20];
      switch (mode){
        case 1:
          land_exp->Dump(str, cnt, val_st, val_ma);
          break;
        case 2:
          lor_exp->Dump(str, cnt, val_st, val_ma);
          land_exp->Dump(str, cnt, val_st, val_ma);
          tmpnum1 = (*val_st).top();
          value1 = tmpnum1.num_val;
          (*val_st).pop();
          tmpnum2 = (*val_st).top();
          value2 = tmpnum2.num_val;
          (*val_st).pop();
          if (tmpnum1.valid == 1){
            if (tmpnum2.valid == 1){
              sprintf(stmp, "  %%%d = eq %d, 0\n", cnt+1, value2);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt+1, cnt);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = eq %d, 0\n", cnt+1, value1);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt+1, cnt);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = or %%%d, %%%d\n", cnt+1, cnt, cnt-2);
            }
            else{
              sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt+1, value2);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt+1, cnt);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = eq %d, 0\n", cnt+1, value1);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt+1, cnt);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = or %%%d, %%%d\n", cnt+1, cnt, cnt-2);
            }
          }
          else{
            if (tmpnum2.valid == 1){
              sprintf(stmp, "  %%%d = eq %d, 0\n", cnt+1, value2);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt+1, cnt);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt+1, value1);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt+1, cnt);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = or %%%d, %%%d\n", cnt+1, cnt, cnt-2);
            }
            else{
              sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt+1, value2);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt+1, cnt);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt+1, value1);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt+1, cnt);
              strcat(str, stmp);
              cnt++;
              sprintf(stmp, "  %%%d = or %%%d, %%%d\n", cnt+1, cnt, cnt-2);
            }
          }
          strcat(str, stmp);
          cnt++;
          tmpnum1.num_val = cnt;
          tmpnum1.valid = 0;
          (*val_st).push(tmpnum1);
          break;
        default:
          assert(false);
          break;
      }
    }
};

// ConstExp ::= Exp;
class ConstExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> exp;

    int Cal(std::map<std::string, sym_t>* val_ma) override {
      int val = exp->Cal(val_ma);
      return val;
    }

    void Dump(char *str, int & cnt, std::stack<num_t>* val_st,
              std::map<std::string, sym_t>* val_ma) const override {
      exp->Dump(str, cnt, val_st, val_ma);
    }
};
