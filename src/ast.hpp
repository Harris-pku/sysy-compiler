#pragma once
#include <cassert>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <stack>
#include <vector>
#include <map>
#include "sym.hpp"

// 所有 AST 的基类
class BaseAST {
  public:
    virtual ~BaseAST() = default;
    virtual int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) = 0;
    virtual void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
                      std::stack<num_t>* val_st, int global,
                      std::map<std::string, sym_t>* val_ma) const = 0;
};

// TreeHead ::= CompUnit
class TreeHeadAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> comp_unit;

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override { return 0; }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      comp_unit->Dump(str, cnt, loop_cur, val_st, global, val_ma);
    }
};

// CompUnit ::= FuncDef | Decl | CompUnit FuncDef | CompUnit Decl
class CompUnitAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> func_def;
    std::unique_ptr<BaseAST> decl;
    std::unique_ptr<BaseAST> comp_unit;
    int mode;

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override { return 0; }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      switch (mode){
        case 1:
          func_def->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        case 2:
          decl->Dump(str, cnt, loop_cur, val_st, 1, val_ma);
          break;
        case 3:
          comp_unit->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          func_def->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        case 4:
          comp_unit->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          decl->Dump(str, cnt, loop_cur, val_st, 1, val_ma);
          break;
        default:
          assert(false);
          break;
      }
    }
};

// Decl ::= ConstDecl | VarDecl
class DeclAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> const_decl;
    std::unique_ptr<BaseAST> var_decl;
    int mode;

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override { return 0; }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      switch (mode){
        case 1:
          const_decl->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        case 2:
          var_decl->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        default:
          assert(false);
          break;
      }
    }
};

// ConstDecl ::= CONST INT ConstDefArr ";"
class ConstDeclAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> const_def_arr;

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override { return 0; }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      const_def_arr->Dump(str, cnt, loop_cur, val_st, global, val_ma);
    }
};

// ConstDefArr ::= ConstDefArr "," ConstDef | ConstDef
class ConstDefArrAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> const_def_arr;
    std::unique_ptr<BaseAST> const_def;
    int mode;

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override { return 0; }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      switch (mode){
        case 1:
          const_def_arr->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          const_def->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        case 2:
          const_def->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        default:
          assert(false);
          break;
      }
    }
};

// ConstDef ::= IDENT "=" ConstInitVal
//            | IDENT ConstExpMuti "=" ConstInitVal
class ConstDefAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> const_init_val;
    std::unique_ptr<BaseAST> const_exp_muti;
    std::string ident;
    int mode;

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override { return 0; }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      sym_t sym;
      int value;
      switch (mode){
        case 1:
          sym.val_t = const_init_val->Cal(str, val_st, val_ma);
          sym.type = 0;
          (*val_ma)[ident] = sym;
          break;
        case 2:
          value = const_exp_muti->Cal(str, val_st, val_ma);
          sym.val_t = 0;
          sym.type = 6;
          sym.number = value;
          (*val_ma)[ident] = sym;
          break;
        default:
          assert(false);
          break;
      }
    }
};

// ConstExpMuti ::= "[" ConstExp "]" | ConstExpMuti "[" ConstExp "]";
class ConstExpMutiAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> const_exp;
    std::unique_ptr<BaseAST> const_exp_muti;
    int mode;

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override { return 0; }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      switch (mode){
        case 1:
          const_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        case 2:
          const_exp_muti->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          const_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        default:
          assert(false);
      }
    }
};

// ConstInitVal ::= ConstExp | "{" "}" | "{" ConstInitValArr "}"
class ConstInitValAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> const_exp;
    std::unique_ptr<BaseAST> const_init_val_arr;
    int mode;

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override {
      int val = 0;
      switch (mode){
        case 1:
          val = const_exp->Cal(str, val_st, val_ma);
          break;
        case 2:
          break;
        case 3:
          break;
        default:
          assert(false);
          break;
      }
      return val;
    }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      switch (mode){
        case 1:
          const_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        case 2:
          break;
        case 3:
          break;
        default:
          assert(false);
          break;
      }
    }
};

// ConstInitValArr ::= ConstInitVal | ConstInitValArr "," ConstInitVal
class ConstInitValArrAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> const_init_val;
    std::unique_ptr<BaseAST> const_init_val_arr;
    int mode;

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override { return 0; }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      switch (mode){
        case 1:
          const_init_val->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        case 2:
          const_init_val_arr->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          const_init_val->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        default:
          assert(false);
      }
    }
};

// VarDecl ::= INT VarDefArr ";"
class VarDeclAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> var_def_arr;

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override { return 0; }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      var_def_arr->Dump(str, cnt, loop_cur, val_st, global, val_ma);
    }
};

// VarDefArr ::= VarDefArr "," VarDef | VarDef;
class VarDefArrAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> var_def_arr;
    std::unique_ptr<BaseAST> var_def;
    int mode;

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override { return 0; }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      switch (mode){
        case 1:
          var_def_arr->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          var_def->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        case 2:
          var_def->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        default:
          assert(false);
          break;
      }
    }
};

// VarDef ::= IDENT | IDENT ConstExpMuti
//          | IDENT "=" InitVal | IDENT ConstExpMuti "=" InitVal
class VarDefAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> init_val;
    std::unique_ptr<BaseAST> const_exp_muti;
    std::string ident;
    int mode;

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override { return 0; }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      char stmp[50];
      int tmpval;
      sym_t sym;
      num_t tmpnum;
      switch (mode){
        case 1:
          sym.val_t = 0;
          sym.type = 1;
          (*val_ma)[ident] = sym;
          if (global == 0){
            sprintf(stmp, "  @%s = alloc i32\n", ident.c_str());
            strcat(str, stmp);
            sprintf(stmp, "  store 0, @%s\n", ident.c_str());
            strcat(str, stmp);
          }
          else{
            sprintf(stmp, "global @%s = alloc i32, 0\n\n", ident.c_str());
            strcat(str, stmp);
          }
          break;
        case 2:
          break;
        case 3:
          if (global == 0){
            init_val->Dump(str, cnt, loop_cur, val_st, global, val_ma);
            tmpnum = (*val_st).top();
            (*val_st).pop();
            sprintf(stmp, "  @%s = alloc i32\n", ident.c_str());
            strcat(str, stmp);
            if (tmpnum.valid == 1){
              sym.val_t = tmpnum.num_val;
              sprintf(stmp, "  store %d, @%s\n", sym.val_t, ident.c_str());
              strcat(str, stmp);
            }
            else{
              sym.val_t = 0;
              sprintf(stmp, "  store %%%d, @%s\n", tmpnum.num_val, ident.c_str());
              strcat(str, stmp);
            }
            sym.type = 1;
            (*val_ma)[ident] = sym;
          }
          else{
            tmpval = init_val->Cal(str, val_st, val_ma);
            sprintf(stmp, "global @%s = alloc i32, %d\n\n", ident.c_str(), tmpval);
            strcat(str, stmp);
            sym.val_t = tmpval;
            sym.type = 1;
            (*val_ma)[ident] = sym;
          }
          break;
        case 4:
          break;
        default:
          assert(false);
          break;
      }
    }
};

// InitVal ::= Exp | "{" "}" | "{" InitValArr "}"
class InitValAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> exp;
    std::unique_ptr<BaseAST> init_val_arr;
    int mode;

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override {
      int val = 0;
      switch (mode){
        case 1:
          val = exp->Cal(str, val_st, val_ma);
          break;
        case 2:
          break;
        case 3:
          break;
        default:
          assert(false);
          break;
      }
      return val;
    }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      switch (mode){
        case 1:
          exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        case 2:
          break;
        case 3:
          init_val_arr->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        default:
          assert(false);
          break;
      }
    }
};

// InitValArr ::= InitVal | InitValArr "," InitVal
class InitValArrAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> init_val;
    std::unique_ptr<BaseAST> init_val_arr;
    int mode;

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override { return 0; }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      switch (mode){
        case 1:
          init_val->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        case 2:
          init_val_arr->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          init_val->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        default:
          assert(false);
          break;
      }
    }
};

// FuncDef ::= INT IDENT "(" ")" Block
//           | VOID IDENT "(" ")" Block
//           | INT IDENT "(" FuncFParamArr ")" Block
//           | VOID IDENT "(" FuncFParamArr ")" Block
class FuncDefAST : public BaseAST {
  public:
    std::string ident;
    std::unique_ptr<BaseAST> block;
    std::unique_ptr<BaseAST> func_fparam_arr;
    int mode;

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override { return 0; }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      char stmp[50];
      sym_t loop_sym;
      switch (mode){
        case 1:
          sprintf(stmp, "fun @%s(): i32 {\n", ident.c_str());
          strcat(str, stmp);
          sprintf(stmp, "%%entry:\n");
          strcat(str, stmp);
          loop_sym.type = 2;
          loop_sym.val_t = 0;
          (*val_ma)[ident] = loop_sym;
          block->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          sprintf(stmp, "}\n\n");
          strcat(str, stmp);
          break;
        case 2:
          sprintf(stmp, "fun @%s() {\n", ident.c_str());
          strcat(str, stmp);  
          sprintf(stmp, "%%entry:\n");
          strcat(str, stmp);
          loop_sym.type = 3;
          loop_sym.val_t = 0;
          (*val_ma)[ident] = loop_sym;
          block->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          if (block->Cal(str, val_st, val_ma) == 3){
            sprintf(stmp, "  ret\n");
            strcat(str, stmp);
          }
          sprintf(stmp, "}\n\n");
          strcat(str, stmp);
          break;
        case 3:
          sprintf(stmp, "fun @%s(", ident.c_str());
          strcat(str, stmp);
          loop_sym.type = 2;
          loop_sym.val_t = 0;
          (*val_ma)[ident] = loop_sym;
          func_fparam_arr->Cal(str, val_st, val_ma);
          sprintf(stmp, "): i32 {\n%%entry:\n");
          strcat(str, stmp);
          func_fparam_arr->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          block->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          sprintf(stmp, "}\n\n");
          strcat(str, stmp);
          break;
        case 4:
          sprintf(stmp, "fun @%s(", ident.c_str());
          strcat(str, stmp);
          loop_sym.type = 3;
          loop_sym.val_t = 0;
          (*val_ma)[ident] = loop_sym;
          func_fparam_arr->Cal(str, val_st, val_ma);
          sprintf(stmp, ") {\n%%entry:\n");
          strcat(str, stmp);
          func_fparam_arr->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          block->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          if (block->Cal(str, val_st, val_ma) == 3){
            sprintf(stmp, "  ret\n");
            strcat(str, stmp);
          }
          sprintf(stmp, "}\n\n");
          strcat(str, stmp);
          break;
        default:
          assert(false);
          break;
      }
    }
};

// FuncFParamArr ::= FuncFParamArr "," FuncFParam | FuncFParam
class FuncFParamArrAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> func_fparam_arr;
    std::unique_ptr<BaseAST> func_fparam;
    int mode;

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override {
      char stmp[20];
      switch (mode){
        case 1:
          func_fparam_arr->Cal(str, val_st, val_ma);
          sprintf(stmp, ", ");
          strcat(str, stmp);
          func_fparam->Cal(str, val_st, val_ma);
          break;
        case 2:
          func_fparam->Cal(str, val_st, val_ma);
          break;
        default:
          assert(false);
          break;
      }
      return 0;
    }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      switch (mode){
        case 1:
          func_fparam_arr->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          func_fparam->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        case 2:
          func_fparam->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        default:
          assert(false);
          break;
      }
    }
};

// FuncFParam ::= INT IDENT
class FuncFParamAST : public BaseAST {
  public:
    std::string ident;

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override {
      char stmp[50];
      sprintf(stmp, "@%s: i32", ident.c_str());
      strcat(str, stmp);
      return 0;
    }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      char stmp[50];
      sprintf(stmp, "  %%%s = alloc i32\n", ident.c_str());
      strcat(str, stmp);
      sprintf(stmp, "  store @%s, %%%s\n", ident.c_str(), ident.c_str());
      strcat(str, stmp);
      sym_t tmp_sym;
      tmp_sym.type = 5;
      tmp_sym.val_t = 0;
      (*val_ma)[ident] = tmp_sym;
    }
};

// Block ::= "{" BlockItemArr "}"
class BlockAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> block_item_arr;

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override {
      int val = block_item_arr->Cal(str, val_st, val_ma);
      return val;
    }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      block_item_arr->Dump(str, cnt, loop_cur, val_st, global, val_ma);
    }
};

// BlockItemArr ::= BlockItemArr Decl | BlockItemArr Stmt | 
class BlockItemArrAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> block_item_arr;
    std::unique_ptr<BaseAST> decl;
    std::unique_ptr<BaseAST> stmt;
    int mode;

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override {
      int val = mode;
      return val;
    }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      // std::cout << "blockitemarr dump mode = " << mode << std::endl;
      switch (mode){
        case 1:
          block_item_arr->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          decl->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        case 2:
          block_item_arr->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          stmt->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        case 3:
          break;
        default:
          assert(false);
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
//        | RETURN Exp ";"
class StmtAST : public BaseAST {
  public:
    // std::unique_ptr<BaseAST> lval;
    std::string ident;
    std::unique_ptr<BaseAST> exp;
    std::unique_ptr<BaseAST> block;
    std::unique_ptr<BaseAST> stmt;
    std::unique_ptr<BaseAST> else_stmt;
    int mode;

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override { return mode; }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      char stmp[50];
      num_t tmpnum;
      sym_t tmpsym;
      int ret_value, value, cur, tmpval;
      switch (mode){
        case 1:
          tmpsym = (*val_ma)[ident];
          if (tmpsym.type == 1){
            sprintf(stmp, "  %%%d = load @%s\n", cnt+1, ident.c_str());
            cnt++;
            strcat(str, stmp);
            exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
            tmpnum = (*val_st).top();
            (*val_st).pop();
            ret_value = tmpnum.num_val;
            if (tmpnum.valid == 1){
              sprintf(stmp, "  %%%d = add 0, %d\n", cnt+1, ret_value);
              strcat(str, stmp);
              cnt++;
            }
            sprintf(stmp, "  store %%%d, @%s\n", cnt, ident.c_str());
            strcat(str, stmp);
          }
          else{
            sprintf(stmp, "  %%%d = load %%%s\n", cnt+1, ident.c_str());
            cnt++;
            strcat(str, stmp);
            exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
            tmpnum = (*val_st).top();
            (*val_st).pop();
            ret_value = tmpnum.num_val;
            if (tmpnum.valid == 1){
              sprintf(stmp, "  %%%d = add 0, %d\n", cnt+1, ret_value);
              strcat(str, stmp);
              cnt++;
            }
            sprintf(stmp, "  store %%%d, @%s\n", cnt, ident.c_str());
            strcat(str, stmp);
          }
          break;
        case 2:
          break;
        case 3:
          exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        case 4:
          block->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        case 5:
          exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          tmpnum = (*val_st).top();
          value = tmpnum.num_val;
          (*val_st).pop();
          cur = std::max(cnt, 0);
          if (tmpnum.valid == 1){
            sprintf(stmp, "  br %d, %%then%d, %%next%d\n\n", value, cur, cur);
          }
          else{
            sprintf(stmp, "  br %%%d, %%then%d, %%next%d\n\n", value, cur, cur);
          }
          strcat(str, stmp);

          sprintf(stmp, "%%then%d:\n", cur);
          strcat(str, stmp);
          stmt->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          if (stmt->Cal(str, val_st, val_ma) != 11){
            sprintf(stmp, "  jump %%next%d\n\n", cur);
          }
          else{
            sprintf(stmp, "\n");
          }
          strcat(str, stmp);

          sprintf(stmp, "%%next%d:\n", cur);
          strcat(str, stmp);
          break;
        case 6:
          exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          tmpnum = (*val_st).top();
          value = tmpnum.num_val;
          (*val_st).pop();
          cur = std::max(cnt, 0);
          if (tmpnum.valid == 1){
            sprintf(stmp, "  br %d, %%then%d, %%else%d\n\n", value, cur, cur);
          }
          else{
            sprintf(stmp, "  br %%%d, %%then%d, %%else%d\n\n", value, cur, cur);
          }
          strcat(str, stmp);

          sprintf(stmp, "%%then%d:\n", cur);
          strcat(str, stmp);
          stmt->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          if (stmt->Cal(str, val_st, val_ma) != 11){
            sprintf(stmp, "  jump %%next%d\n\n", cur);
          }
          else{
            sprintf(stmp, "\n");
          }
          strcat(str, stmp);

          sprintf(stmp, "%%else%d:\n", cur);
          strcat(str, stmp);
          else_stmt->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          if (else_stmt->Cal(str, val_st, val_ma) != 11){
            sprintf(stmp, "  jump %%next%d\n\n", cur);
          }
          else{
            sprintf(stmp, "\n");
          }
          strcat(str, stmp);

          sprintf(stmp, "%%next%d:\n", cur);
          strcat(str, stmp);
          break;
        case 7:
          cur = std::max(cnt, 0);
          (*loop_cur).push(cur);
          sprintf(stmp, "  jump %%while_entry%d\n\n", cur);
          strcat(str, stmp);

          sprintf(stmp, "%%while_entry%d:\n", cur);
          strcat(str, stmp);
          exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          tmpnum = (*val_st).top();
          value = tmpnum.num_val;
          (*val_st).pop();
          if (tmpnum.valid == 1){
            sprintf(stmp, "  br %d, %%while_body%d, %%next%d\n\n", value, cur, cur);
          }
          else{
            sprintf(stmp, "  br %%%d, %%while_body%d, %%next%d\n\n", value, cur, cur);
          }
          strcat(str, stmp);

          sprintf(stmp, "%%while_body%d:\n", cur);
          strcat(str, stmp);
          stmt->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          tmpval = stmt->Cal(str, val_st, val_ma);
          if (tmpval != 11){
            sprintf(stmp, "  jump %%while_entry%d\n\n", cur);
          }
          else{
            sprintf(stmp, "\n");
          }
          strcat(str, stmp);

          sprintf(stmp, "%%next%d:\n", cur);
          strcat(str, stmp);
          (*loop_cur).pop();
          break;
        case 8:
          cur = (*loop_cur).top();
          sprintf(stmp, "  jump %%next%d\n\n", cur);
          strcat(str, stmp);
          sprintf(stmp, "%%while_body_%d:\n", cur);
          strcat(str, stmp);
          break;
        case 9:
          cur = (*loop_cur).top();
          sprintf(stmp, "  jump %%while_entry%d\n\n", cur);
          strcat(str, stmp);
          sprintf(stmp, "%%while_body_%d:\n", cur);
          strcat(str, stmp);
          break;
        case 10:
          sprintf(stmp, "  ret\n");
          strcat(str, stmp);
          break;
        case 11:
          exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
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

// Exp ::= LorExp
class ExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> lor_exp;

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override {
      int val = lor_exp->Cal(str, val_st, val_ma);
      return val;
    }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      lor_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
    }
};

// LVal ::= IDENT | IDENT ExpMuti
class LValAST : public BaseAST {
  public:
    std::string ident;
    std::unique_ptr<BaseAST> exp_muti;
    int mode;

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override {
      int val = 0;
      sym_t sym;
      switch (mode){
        case 1:
          assert((*val_ma).count(ident) == 1);
          sym = (*val_ma)[ident];
          val = sym.val_t;
          break;
        case 2:
          break;
        default:
          break;
      }
      return val;
    }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      char stmp[50];
      num_t tmpnum;
      switch (mode){
        case 1:
          if ((*val_ma).count(ident) == 1){
            sym_t sym = (*val_ma)[ident];
            if (sym.type == 0){
              tmpnum.num_val = sym.val_t;
              tmpnum.valid = 1;
              (*val_st).push(tmpnum);
            }
            else if (sym.type == 1){
              sprintf(stmp, "  %%%d = load @%s\n", cnt+1, ident.c_str());
              cnt++;
              strcat(str, stmp);
              tmpnum.num_val = cnt;
              tmpnum.valid = 0;
              (*val_st).push(tmpnum);
            }
            else{
              sprintf(stmp, "  %%%d = load %%%s\n", cnt+1, ident.c_str());
              cnt++;
              strcat(str, stmp);
              tmpnum.num_val = cnt;
              tmpnum.valid = 0;
              (*val_st).push(tmpnum);
           }
          }
          break;
        case 2:
          break;
        default:
          assert(false);
          break;
      }
    }
};

// ExpMuti ::= "[" Exp "]" | ExpMuti "[" Exp "]";
class ExpMutiAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> exp;
    std::unique_ptr<BaseAST> exp_muti;
    int mode;

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override { return 0; }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      switch (mode){
        case 1:
          exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        case 2:
          exp_muti->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        default:
          assert(false);
          break;
      }
    }
};

// PrimaryExp ::= "(" Exp ")" | LVal | Number;
class PrimaryExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> exp;
    std::unique_ptr<BaseAST> lval;
    int number, mode;

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override {
      int val = 0;
      switch (mode){
        case 1:
          val = exp->Cal(str, val_st, val_ma);
          break;
        case 2:
          val = lval->Cal(str, val_st, val_ma);
          break;
        case 3:
          val = number;
          break;
        default:
          assert(false);
          break;
      }
      return val;
    }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      switch (mode){
        case 1:
          exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        case 2:
          lval->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        case 3:
          num_t tmpnum;
          tmpnum.num_val = number;
          tmpnum.valid = 1;
          (*val_st).push(tmpnum);
          break;
        default:
          assert(false);
          break;
      }
    }
};

// Number ::= INT_CONST;
class NumberAST : public BaseAST {
  public:
    int num;
    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override {
      return num;
    }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      num_t tmpnum;
      tmpnum.num_val = num;
      tmpnum.valid = 1;
      (*val_st).push(tmpnum);
    }
};

// UnaryExp ::= PrimaryExp
//            | IDENT "(" ")"
//            | IDENT "(" FuncRParamArr ")"
//            | ("+" | "-" | "!") UnaryExp;
class UnaryExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> primary_exp;
    std::string ident;
    std::unique_ptr<BaseAST> func_rparam_arr;
    std::unique_ptr<BaseAST> unary_exp;
    int mode;

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override {
      int val = 0;
      switch (mode){
        case 1:
          val = primary_exp->Cal(str, val_st, val_ma);
          break;
        case 2:
          break;
        case 3:
          break;
        case 4:
          val = unary_exp->Cal(str, val_st, val_ma);
          break;
        case 5:
          val = unary_exp->Cal(str, val_st, val_ma);
          val = -val;
          break;
        case 6:
          val = unary_exp->Cal(str, val_st, val_ma);
          val = !val;
          break;
        default:
          assert(false);
          break;
      }
      return val;
    }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
        
      char stmp[50];
      num_t tmpnum;
      sym_t tmp_loop;
      int value;
      switch (mode){
        case 1:
          primary_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        case 2:
          tmp_loop = (*val_ma)[ident];
          if (tmp_loop.type == 2){
            sprintf(stmp, "  %%%d = call @%s()\n", cnt+1, ident.c_str());
            cnt++;
            tmpnum.num_val = cnt;
            tmpnum.valid = 0;
            (*val_st).push(tmpnum);
          }
          else if (tmp_loop.type == 3){
            sprintf(stmp, "  call @%s()\n", ident.c_str());
          }
          strcat(str, stmp);
          break;
        case 3:
          tmp_loop = (*val_ma)[ident];
          if (tmp_loop.type == 2){
            func_rparam_arr->Dump(str, cnt, loop_cur, val_st, global, val_ma);
            sprintf(stmp, "  %%%d = call @%s(", cnt+1, ident.c_str());
            strcat(str, stmp);
            cnt++;
            func_rparam_arr->Cal(str, val_st, val_ma);
            sprintf(stmp, ")\n");
            tmpnum.num_val = cnt;
            tmpnum.valid = 0;
            (*val_st).push(tmpnum);
          }
          else if (tmp_loop.type == 3){
            func_rparam_arr->Dump(str, cnt, loop_cur, val_st, global, val_ma);
            sprintf(stmp, "  call @%s(", ident.c_str());
            strcat(str, stmp);
            func_rparam_arr->Cal(str, val_st, val_ma);
            sprintf(stmp, ")\n");
          }
          strcat(str, stmp);
          break;
        case 4:
          unary_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        case 5:
          // std::cout << "unary_exp dump5" << std::endl;
          unary_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          tmpnum = (*val_st).top();
          value = tmpnum.num_val;
          (*val_st).pop();
          if (tmpnum.valid == 1){
            sprintf(stmp, "  %%%d = sub 0, %d\n", cnt+1, value);
          }
          else{
            sprintf(stmp, "  %%%d = sub 0, %%%d\n", cnt+1, value);
          }
          cnt++;
          strcat(str, stmp);
          tmpnum.valid = 0;
          tmpnum.num_val = cnt;
          (*val_st).push(tmpnum);
          break;
        case 6:
          // std::cout << "unary_exp dump6" << std::endl;
          unary_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          tmpnum = (*val_st).top();
          value = tmpnum.num_val;
          (*val_st).pop();
          if (tmpnum.valid == 1){
            sprintf(stmp, "  %%%d = eq %d, 0\n", cnt+1, value);
          }
          else{
            sprintf(stmp, "  %%%d = eq %%%d, 0\n", cnt+1, value);
          }
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
};

// FuncRParamArr ::= FuncRParamArr "," FuncRParam | FuncRParam;
class FuncRParamArrAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> func_rparam_arr;
    std::unique_ptr<BaseAST> func_rparam;
    int mode;

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override {
      char stmp[20];
      switch (mode){
        case 1:
          // std::cout << "func_rparam_arr cal1" << std::endl;
          func_rparam_arr->Cal(str, val_st, val_ma);
          sprintf(stmp, ", ");
          strcat(str, stmp);
          func_rparam->Cal(str, val_st, val_ma);
          break;
        case 2:
          // std::cout << "func_rparam_arr cal2" << std::endl;
          func_rparam->Cal(str, val_st, val_ma);
          break;
        default:
          assert(false);
          break;
      }
      return 0;
    }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      switch (mode){
        case 1:
          func_rparam->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          func_rparam_arr->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        case 2:
          func_rparam->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        default:
          assert(false);
          break;
      }
    }
};

// FuncRParam ::= Exp;
class FuncRParamAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> exp;

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override {
      num_t tmpnum= (*val_st).top();
      int value = tmpnum.num_val;
      (*val_st).pop();
      char stmp[20];
      if (tmpnum.valid == 1){
        sprintf(stmp, "%d", value);
      }
      else{
        sprintf(stmp, "%%%d", value);
      }
      strcat(str, stmp);
      return 0;
    }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
    }
};

// MulExp ::= UnaryExp | MulExp ("*" | "/" | "%") UnaryExp;
class MulExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> unary_exp;
    std::unique_ptr<BaseAST> mul_exp;
    int mode;

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override {
      int val = 0, valx, valy;
      switch (mode){
        case 1:
          val = unary_exp->Cal(str, val_st, val_ma);
          break;
        case 2:
          valx = mul_exp->Cal(str, val_st, val_ma);
          valy = unary_exp->Cal(str, val_st, val_ma);
          val = valx * valy;
          break;
        case 3:
          valx = mul_exp->Cal(str, val_st, val_ma);
          valy = unary_exp->Cal(str, val_st, val_ma);
          val = valx / valy;
          break;
        case 4:
          valx = mul_exp->Cal(str, val_st, val_ma);
          valy = unary_exp->Cal(str, val_st, val_ma);
          val = valx % valy;
          break;
        default:
          assert(false);
          break;
      }
      return val;
    }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      num_t tmpnum1, tmpnum2;
      int value1, value2;
      char stmp[20];
      switch (mode){
        case 1:
          // std::cout << "mul_exp dump1" << std::endl;
          unary_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        case 2:
          // std::cout << "mul_exp dump2" << std::endl;
          mul_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          unary_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
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
          mul_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          unary_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
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
          mul_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          unary_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
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

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override {
      int val = 0, valx, valy;
      switch (mode){
        case 1:
          val = mul_exp->Cal(str, val_st, val_ma);
          break;
        case 2:
          valx = add_exp->Cal(str, val_st, val_ma);
          valy = mul_exp->Cal(str, val_st, val_ma);
          val = valx + valy;
          break;
        case 3:
          valx = add_exp->Cal(str, val_st, val_ma);
          valy = mul_exp->Cal(str, val_st, val_ma);
          val = valx - valy;
          break;
        default:
          assert(false);
          break;
      }
      return val;
    }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      num_t tmpnum1, tmpnum2;
      int value1, value2;
      char stmp[20];
      switch (mode){
        case 1:
          // std::cout << "add_exp dump1" << std::endl;
          mul_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        case 2:
          // std::cout << "add_exp dump2" << std::endl;
          add_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          mul_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
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
          add_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          mul_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
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
    
    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override {
      int val = 0, valx, valy;
      switch (mode){
        case 1:
          val = add_exp->Cal(str, val_st, val_ma);
          break;
        case 2:
          valx = rel_exp->Cal(str, val_st, val_ma);
          valy = add_exp->Cal(str, val_st, val_ma);
          val = valx < valy;
          break;
        case 3:
          valx = rel_exp->Cal(str, val_st, val_ma);
          valy = add_exp->Cal(str, val_st, val_ma);
          val = valx > valy;
          break;
        case 4:
          valx = rel_exp->Cal(str, val_st, val_ma);
          valy = add_exp->Cal(str, val_st, val_ma);
          val = valx <= valy;
          break;
        case 5:
          valx = rel_exp->Cal(str, val_st, val_ma);
          valy = add_exp->Cal(str, val_st, val_ma);
          val = valx >= valy;
          break;
        default:
          assert(false);
          break;
      }
      return val;
    }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      num_t tmpnum1, tmpnum2;
      int value1, value2;
      char stmp[20];
      switch (mode){
        case 1:
          add_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        case 2:
          rel_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          add_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
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
          rel_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          add_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
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
          rel_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          add_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
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
          rel_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          add_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
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

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override {
      int val = 0, valx, valy;
      switch (mode){
        case 1:
          val = rel_exp->Cal(str, val_st, val_ma);
          break;
        case 2:
          valx = eq_exp->Cal(str, val_st, val_ma);
          valy = rel_exp->Cal(str, val_st, val_ma);
          val = valx == valy;
          break;
        case 3:
          valx = eq_exp->Cal(str, val_st, val_ma);
          valy = rel_exp->Cal(str, val_st, val_ma);
          val = valx != valy;
          break;
        default:
          assert(false);
          break;
      }
      return val;
    }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      num_t tmpnum1, tmpnum2;
      int value1, value2;
      char stmp[20];
      switch (mode){
        case 1:
          rel_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        case 2:
          eq_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          rel_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
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
          eq_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          rel_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
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

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override {
      int val = 0, valx, valy;
      switch (mode){
        case 1:
          val = eq_exp->Cal(str, val_st, val_ma);
          break;
        case 2:
          valx = land_exp->Cal(str, val_st, val_ma);
          valy = eq_exp->Cal(str, val_st, val_ma);
          val = valx && valy;
          break;
        default:
          assert(false);
          break;
      }
      return val;
    }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      num_t tmpnum1, tmpnum2;
      int value1, value2;
      char stmp[20];
      switch (mode){
        case 1:
          eq_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        case 2:
          land_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          eq_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
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

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override {
      int val = 0, valx, valy;
      switch (mode){
        case 1:
          val = land_exp->Cal(str, val_st, val_ma);
          break;
        case 2:
          valx = lor_exp->Cal(str, val_st, val_ma);
          valy = land_exp->Cal(str, val_st, val_ma);
          val = valx || valy;
          break;
        default:
          assert(false);
          break;
      }
      return val;
    }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      num_t tmpnum1, tmpnum2;
      int value1, value2;
      char stmp[20];
      switch (mode){
        case 1:
          land_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          break;
        case 2:
          lor_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
          land_exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
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

    int Cal(char *str, std::stack<num_t>* val_st, std::map<std::string, sym_t>* val_ma) override {
      int val = exp->Cal(str, val_st, val_ma);
      return val;
    }

    void Dump(char *str, int & cnt, std::stack<int>* loop_cur,
              std::stack<num_t>* val_st, int global,
              std::map<std::string, sym_t>* val_ma) const override {
      exp->Dump(str, cnt, loop_cur, val_st, global, val_ma);
    }
};
