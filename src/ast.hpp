#pragma once
#include <iostream>

// 所有 AST 的基类
class BaseAST {
  public:
    virtual ~BaseAST() = default;

    virtual void Dump() const = 0;
};

// CompUnit ::= FuncDef;
class CompUnitAST : public BaseAST {
  public:
    // 用智能指针管理对象
    std::unique_ptr<BaseAST> func_def;

    void Dump() const override {
      // std::cout << "CompUnitAST { ";
      func_def->Dump();
      // std::cout << " }";
    }
};

// FuncDef ::= FuncType IDENT "(" ")" Block;
class FuncDefAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> func_type;
    std::string ident;
    std::unique_ptr<BaseAST> block;

    void Dump() const override {
      std::cout << "fun @" << ident << "(): ";
      func_type->Dump();
      block->Dump();
    }
};

// FuncType ::= "int";
class FuncTypeAST : public BaseAST {
  public:
    std::string type;

    void Dump() const override {
      if (type == "int"){
        std::cout << "i32";
      }
    }
};

// Block ::= "{" Stmt "}";
class BlockAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> stmt;

    void Dump() const override {
      std::cout << " {\n";
      stmt->Dump();
      std::cout << "}";
    }
};

// Stmt ::= "return" Number ";";
class StmtAST : public BaseAST {
  public:
    int number;

    void Dump() const override {
      std::cout << "%entry:\n  ret ";
      std::cout << number << "\n";
    }
};

// Number ::= INT_CONST;
class NumberAST : public BaseAST {
  public:
    int num;

    void Dump() const override {
      std::cout << num;
    }
};
