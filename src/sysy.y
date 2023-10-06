%code requires {
  #include <memory>
  #include <string>
  #include "ast.hpp"
}

%{

#include <iostream>
#include <memory>
#include <string>
#include "ast.hpp"

// 声明 lexer 函数和错误处理函数
int yylex();
void yyerror(std::unique_ptr<BaseAST> &ast, const char *s);

using namespace std;

%}

// 定义 parser 函数和错误处理函数的附加参数
// 我们需要返回一个字符串作为 AST, 所以我们把附加参数定义成字符串的智能指针
// 解析完成后, 我们要手动修改这个参数, 把它设置成解析得到的字符串
%parse-param { std::unique_ptr<BaseAST> &ast }

// yylval 的定义, 我们把它定义成了一个联合体 (union)
// 因为 token 的值有的是字符串指针, 有的是整数
// 之前我们在 lexer 中用到的 str_val 和 int_val 就是在这里被定义的
// 至于为什么要用字符串指针而不直接用 string 或者 unique_ptr<string>?
// 请自行 STFW 在 union 里写一个带析构函数的类会出现什么情况
%union {
  std::string *str_val;
  int int_val;
  BaseAST *ast_val;
}

// lexer 返回的所有 token 种类的声明
// 注意 IDENT 和 INT_CONST 会返回 token 的值, 分别对应 str_val 和 int_val
%token INT RETURN CONST VOID IF ELSE WHILE BREAK CONTINUE
%token LE GE EQ NE AND OR
%token <str_val> IDENT
%token <int_val> INT_CONST

// 非终结符的类型定义
%type <ast_val> CompUnit FuncDef Block Stmt
%type <ast_val> Exp PrimaryExp UnaryExp MulExp AddExp
%type <ast_val> RelExp EqExp LAndExp LOrExp
%type <ast_val> Decl ConstDecl ConstDefArr ConstDef
%type <ast_val> LVal BlockItemArr ConstInitVal ConstExp
%type <ast_val> VarDecl VarDefArr VarDef InitVal
%type <ast_val> FuncFParamArr FuncFParam FuncRParamArr FuncRParam
%type <ast_val> ConstInitValArr ConstExpMuti InitValArr ExpMuti
%type <int_val> Number

%%

// TreeHead ::= CompUnit
TreeHead
  : CompUnit {
    auto tree_head = make_unique<TreeHeadAST>();
    tree_head->comp_unit = unique_ptr<BaseAST>($1);
    ast = move(tree_head);
  }
  ;

// 开始符, CompUnit ::= FuncDef, 大括号后声明了解析完成后 parser 要做的事情
// 之前我们定义了 FuncDef 会返回一个 str_val, 也就是字符串指针
// 而 parser 一旦解析完 CompUnit, 就说明所有的 token 都被解析了, 即解析结束了
// 此时我们应该把 FuncDef 返回的结果收集起来, 作为 AST 传给调用 parser 的函数
// $1 指代规则里第一个符号的返回值, 也就是 FuncDef 的返回值

// CompUnit ::= FuncDef | Decl | CompUnit FuncDef | CompUnit Decl
CompUnit
  : FuncDef {
    auto ast = new CompUnitAST();
    ast->func_def = unique_ptr<BaseAST>($1);
    ast->mode = 1;
    $$ = ast;
  }
  | Decl {
    auto ast = new CompUnitAST();
    ast->decl = unique_ptr<BaseAST>($1);
    ast->mode = 2;
    $$ = ast;
  }
  | CompUnit FuncDef {
    auto ast = new CompUnitAST();
    ast->comp_unit = unique_ptr<BaseAST>($1);
    ast->func_def = unique_ptr<BaseAST>($2);
    ast->mode = 3;
    $$ = ast;
  }
  | CompUnit Decl {
    auto ast = new CompUnitAST();
    ast->comp_unit = unique_ptr<BaseAST>($1);
    ast->decl = unique_ptr<BaseAST>($2);
    ast->mode = 4;
    $$ = ast;
  }
  ;

// Decl ::= ConstDecl | VarDecl
Decl
  : ConstDecl {
    auto ast = new DeclAST();
    ast->const_decl = unique_ptr<BaseAST>($1);
    ast->mode = 1;
    $$ = ast;
  }
  | VarDecl {
    auto ast = new DeclAST();
    ast->var_decl = unique_ptr<BaseAST>($1);
    ast->mode = 2;
    $$ = ast;
  }
  ;

// ConstDecl ::= CONST INT ConstDefArr ";"
ConstDecl
  : CONST INT ConstDefArr ';' {
    auto ast = new ConstDeclAST();
    ast->const_def_arr = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

// ConstDefArr ::= ConstDefArr "," ConstDef | ConstDef
ConstDefArr
  : ConstDefArr ',' ConstDef {
    auto ast = new ConstDefArrAST();
    ast->const_def_arr = unique_ptr<BaseAST>($1);
    ast->const_def = unique_ptr<BaseAST>($3);
    ast->mode = 1;
    $$ = ast;
  }
  | ConstDef {
    auto ast = new ConstDefArrAST();
    ast->const_def = unique_ptr<BaseAST>($1);
    ast->mode = 2;
    $$ = ast;
  }
  ;

// ConstDef ::= IDENT "=" ConstInitVal
//            | IDENT ConstExpMuti "=" ConstInitVal
ConstDef
  : IDENT '=' ConstInitVal {
    auto ast = new ConstDefAST();
    ast->ident = *unique_ptr<string>($1);
    ast->const_init_val = unique_ptr<BaseAST>($3);
    ast->mode = 1;
    $$ = ast;
  }
  | IDENT ConstExpMuti '=' ConstInitVal {
    auto ast = new ConstDefAST();
    ast->ident = *unique_ptr<string>($1);
    ast->const_exp_muti = unique_ptr<BaseAST>($2);
    ast->const_init_val = unique_ptr<BaseAST>($4);
    ast->mode = 2;
    $$ = ast;
  }
  ;

// ConstExpMuti ::= "[" ConstExp "]" | ConstExpMuti "[" ConstExp "]";
ConstExpMuti
  : '[' ConstExp ']' {
    auto ast = new ConstExpMutiAST();
    ast->const_exp = unique_ptr<BaseAST>($2);
    ast->mode = 1;
    $$ = ast;
  }
  | ConstExpMuti '[' ConstExp ']' {
    auto ast = new ConstExpMutiAST();
    ast->const_exp_muti = unique_ptr<BaseAST>($1);
    ast->const_exp = unique_ptr<BaseAST>($3);
    ast->mode = 2;
    $$ = ast;
  }
  ;

// ConstInitVal ::= ConstExp | "{" "}" | "{" ConstInitValArr "}"
ConstInitVal
  : ConstExp {
    auto ast = new ConstInitValAST();
    ast->const_exp = unique_ptr<BaseAST>($1);
    ast->mode = 1;
    $$ = ast;
  }
  | '{' '}' {
    auto ast = new ConstInitValAST();
    ast->mode = 2;
    $$ = ast;
  }
  | '{' ConstInitValArr '}' {
    auto ast = new ConstInitValAST();
    ast->const_init_val_arr = unique_ptr<BaseAST>($2);
    ast->mode = 3;
    $$ = ast;
  }
  ;

// ConstInitValArr ::= ConstInitVal | ConstInitValArr "," ConstInitVal
ConstInitValArr
  : ConstInitVal {
    auto ast = new ConstInitValArrAST();
    ast->const_init_val = unique_ptr<BaseAST>($1);
    ast->mode = 1;
    $$ = ast;
  }
  | ConstInitValArr ',' ConstInitVal {
    auto ast = new ConstInitValArrAST();
    ast->const_init_val_arr = unique_ptr<BaseAST>($1);
    ast->const_init_val = unique_ptr<BaseAST>($3);
    ast->mode = 2;
    $$ = ast;
  }
  ;

// VarDecl ::= INT VarDefArr ";"
VarDecl
  : INT VarDefArr ';' {
    auto ast = new VarDeclAST();
    ast->var_def_arr = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;

// VarDefArr ::= VarDefArr "," VarDef | VarDef
VarDefArr
  : VarDefArr ',' VarDef {
    auto ast = new VarDefArrAST();
    ast->var_def_arr = unique_ptr<BaseAST>($1);
    ast->var_def = unique_ptr<BaseAST>($3);
    ast->mode = 1;
    $$ = ast;
  }
  | VarDef {
    auto ast = new VarDefArrAST();
    ast->var_def = unique_ptr<BaseAST>($1);
    ast->mode = 2;
    $$ = ast;
  }
  ;

// VarDef ::= IDENT | IDENT ConstExpMuti
//          | IDENT "=" InitVal | IDENT ConstExpMuti "=" InitVal;
VarDef
  : IDENT {
    auto ast = new VarDefAST();
    ast->ident = *unique_ptr<string>($1);
    ast->mode = 1;
    $$ = ast;
  }
  | IDENT ConstExpMuti {
    auto ast = new VarDefAST();
    ast->ident = *unique_ptr<string>($1);
    ast->const_exp_muti = unique_ptr<BaseAST>($2);
    ast->mode = 2;
    $$ = ast;
  }
  | IDENT '=' InitVal {
    auto ast = new VarDefAST();
    ast->ident = *unique_ptr<string>($1);
    ast->init_val = unique_ptr<BaseAST>($3);
    ast->mode = 3;
    $$ = ast;
  }
  | IDENT ConstExpMuti '=' InitVal {
    auto ast = new VarDefAST();
    ast->ident = *unique_ptr<string>($1);
    ast->const_exp_muti = unique_ptr<BaseAST>($2);
    ast->init_val = unique_ptr<BaseAST>($4);
    ast->mode = 4;
    $$ = ast;
  }
  ;

// InitVal ::= Exp | "{" "}" | "{" InitValArr "}"
InitVal
  : Exp {
    auto ast = new InitValAST();
    ast->exp = unique_ptr<BaseAST>($1);
    ast->mode = 1;
    $$ = ast;
  }
  | '{' '}' {
    auto ast = new InitValAST();
    ast->mode = 2;
    $$ = ast;
  }
  | '{' InitValArr '}' {
    auto ast = new InitValAST();
    ast->init_val_arr = unique_ptr<BaseAST>($2);
    ast->mode = 3;
    $$ = ast;
  }
  ;

// InitValArr ::= InitVal | InitValArr "," InitVal
InitValArr
  : InitVal {
    auto ast = new InitValArrAST();
    ast->init_val = unique_ptr<BaseAST>($1);
    ast->mode = 1;
    $$ = ast;
  }
  | InitValArr ',' InitVal {
    auto ast = new InitValArrAST();
    ast->init_val_arr = unique_ptr<BaseAST>($1);
    ast->init_val = unique_ptr<BaseAST>($3);
    ast->mode = 2;
    $$ = ast;
  }
  ;

// FuncDef ::= FuncType IDENT '(' ')' Block;
// 我们这里可以直接写 '(' 和 ')', 因为之前在 lexer 里已经处理了单个字符的情况
// 解析完成后, 把这些符号的结果收集起来, 然后拼成一个新的字符串, 作为结果返回
// $$ 表示非终结符的返回值, 我们可以通过给这个符号赋值的方法来返回结果
// 你可能会问, FuncType, IDENT 之类的结果已经是字符串指针了
// 为什么还要用 unique_ptr 接住它们, 然后再解引用, 把它们拼成另一个字符串指针呢
// 因为所有的字符串指针都是我们 new 出来的, new 出来的内存一定要 delete
// 否则会发生内存泄漏, 而 unique_ptr 这种智能指针可以自动帮我们 delete
// 虽然此处你看不出用 unique_ptr 和手动 delete 的区别, 但当我们定义了 AST 之后
// 这种写法会省下很多内存管理的负担

// FuncDef ::= INT IDENT "(" ")" Block
//           | VOID IDENT "(" ")" Block
//           | INT IDENT "(" FuncFParamArr ")" Block
//           | VOID IDENT "(" FuncFParamArr ")" Block
FuncDef
  : INT IDENT '(' ')' Block {
    auto ast = new FuncDefAST();
    ast->ident = *unique_ptr<string>($2);
    ast->block = unique_ptr<BaseAST>($5);
    ast->mode = 1;
    $$ = ast;  
  }
  | VOID IDENT '(' ')' Block {
    auto ast = new FuncDefAST();
    ast->ident = *unique_ptr<string>($2);
    ast->block = unique_ptr<BaseAST>($5);
    ast->mode = 2;
    $$ = ast;  
  }
  | INT IDENT '(' FuncFParamArr ')' Block {
    auto ast = new FuncDefAST();
    ast->ident = *unique_ptr<string>($2);
    ast->func_fparam_arr = unique_ptr<BaseAST>($4);
    ast->block = unique_ptr<BaseAST>($6);
    ast->mode = 3;
    $$ = ast;  
  }
  | VOID IDENT '(' FuncFParamArr ')' Block {
    auto ast = new FuncDefAST();
    ast->ident = *unique_ptr<string>($2);
    ast->func_fparam_arr = unique_ptr<BaseAST>($4);
    ast->block = unique_ptr<BaseAST>($6);
    ast->mode = 4;
    $$ = ast;  
  }
  ;

// FuncFParamArr ::= FuncFParamArr "," FuncFParam | FuncFParam
FuncFParamArr
  : FuncFParamArr ',' FuncFParam {
    auto ast = new FuncFParamArrAST();
    ast->func_fparam_arr = unique_ptr<BaseAST>($1);
    ast->func_fparam = unique_ptr<BaseAST>($3);
    ast->mode = 1;
    $$ = ast;
  }
  | FuncFParam {
    auto ast = new FuncFParamArrAST();
    ast->func_fparam = unique_ptr<BaseAST>($1);
    ast->mode = 2;
    $$ = ast;
  }
  ;

// FuncFParam ::= INT IDENT
FuncFParam
  : INT IDENT {
    auto ast = new FuncFParamAST();
    ast->ident = *unique_ptr<string>($2);
    $$ = ast;
  }
  ;

// Block ::= "{" BlockItemArr "}"
Block
  : '{' BlockItemArr '}' {
    auto ast = new BlockAST();
    ast->block_item_arr = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;

// BlockItemArr ::= BlockItemArr Decl | BlockItemArr Stmt | 
BlockItemArr
  : BlockItemArr Decl {
    auto ast = new BlockItemArrAST();
    ast->block_item_arr = unique_ptr<BaseAST>($1);
    ast->decl = unique_ptr<BaseAST>($2);
    ast->mode = 1;
    $$ = ast;
  }
  | BlockItemArr Stmt {
    auto ast = new BlockItemArrAST();
    ast->block_item_arr = unique_ptr<BaseAST>($1);
    ast->stmt = unique_ptr<BaseAST>($2);
    ast->mode = 2;
    $$ = ast;
  }
  | {
    auto ast = new BlockItemArrAST();
    ast->mode = 3;
    $$ = ast;
  }
  ; 

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
Stmt
  : IDENT '=' Exp ';' {
    auto ast = new StmtAST();
    ast->ident = *unique_ptr<string>($1);
    ast->exp = unique_ptr<BaseAST>($3);
    ast->mode = 1;
    $$ = ast;
  }
  | ';' {
    auto ast = new StmtAST();
    ast->mode = 2;
    $$ = ast;
  }
  | Exp ';' {
    auto ast = new StmtAST();
    ast->exp = unique_ptr<BaseAST>($1);
    ast->mode = 3;
    $$ = ast;
  }
  | Block {
    auto ast = new StmtAST();
    ast->block = unique_ptr<BaseAST>($1);
    ast->mode = 4;
    $$ = ast;
  }
  | IF '(' Exp ')' Stmt{
    auto ast = new StmtAST();
    ast->exp = unique_ptr<BaseAST>($3);
    ast->stmt = unique_ptr<BaseAST>($5);
    ast->mode = 5;
    $$ = ast;
  }
  | IF '(' Exp ')' Stmt ELSE Stmt {
    auto ast = new StmtAST();
    ast->exp = unique_ptr<BaseAST>($3);
    ast->stmt = unique_ptr<BaseAST>($5);
    ast->else_stmt = unique_ptr<BaseAST>($7);
    ast->mode = 6;
    $$ = ast;
  }
  | WHILE '(' Exp ')' Stmt {
    auto ast = new StmtAST();
    ast->exp = unique_ptr<BaseAST>($3);
    ast->stmt = unique_ptr<BaseAST>($5);
    ast->mode = 7;
    $$ = ast;
  }
  | BREAK ';' {
    auto ast = new StmtAST();
    ast->mode = 8;
    $$ = ast;
  }
  | CONTINUE ';' {
    auto ast = new StmtAST();
    ast->mode = 9;
    $$ = ast;
  }
  | RETURN ';' {
    auto ast = new StmtAST();
    ast->mode = 10;
    $$ = ast;
  }
  | RETURN Exp ';' {
    auto ast = new StmtAST();
    ast->exp = unique_ptr<BaseAST>($2);
    ast->mode = 11;
    $$ = ast;
  }
  ;

// Exp ::= LOrExp
Exp
  : LOrExp {
    auto ast = new ExpAST();
    ast->lor_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

// LVal ::= IDENT | IDENT ExpMuti;
LVal
  : IDENT {
    auto ast = new LValAST();
    ast->ident = *unique_ptr<string>($1);
    ast->mode = 1;
    $$ = ast;
  }
  | IDENT ExpMuti {
    auto ast = new LValAST();
    ast->ident = *unique_ptr<string>($1);
    ast->exp_muti = unique_ptr<BaseAST>($2);
    ast->mode = 2;
    $$ = ast;
  }
  ;

// ExpMuti ::= "[" Exp "]" | ExpMuti "[" Exp "]";
ExpMuti
  : '[' Exp ']' {
    auto ast = new ExpMutiAST();
    ast->exp = unique_ptr<BaseAST>($2);
    ast->mode = 1;
    $$ = ast;
  }
  | ExpMuti '[' Exp ']' {
    auto ast = new ExpMutiAST();
    ast->exp_muti = unique_ptr<BaseAST>($1);
    ast->exp = unique_ptr<BaseAST>($3);
    ast->mode = 2;
    $$ = ast;
  }
  ;

// PrimaryExp ::= "(" Exp ")" | LVal | Number
PrimaryExp
  : '(' Exp ')' {
    auto ast = new PrimaryExpAST();
    ast->exp = unique_ptr<BaseAST>($2);
    ast->mode = 1;
    $$ = ast;
  }
  | LVal {
    auto ast = new PrimaryExpAST();
    ast->lval = unique_ptr<BaseAST>($1);
    ast->mode = 2;
    $$ = ast;
  }
  | Number {
    auto ast = new PrimaryExpAST();
    ast->number = $1;
    ast->mode = 3;
    $$ = ast;
  }
  ;

// Number ::= INT_CONST
Number
  : INT_CONST {
    int num = $1;
    $$ = num;
  }
  ;

// UnaryExp ::= PrimaryExp
//            | IDENT "(" ")"
//            | IDENT "(" FuncRParamArr ")"
//            | ("+" | "-" | "!") UnaryExp
UnaryExp
  : PrimaryExp {
    auto ast = new UnaryExpAST();
    ast->primary_exp = unique_ptr<BaseAST>($1);
    ast->mode = 1;
    $$ = ast;
  }
  | IDENT '(' ')' {
    auto ast = new UnaryExpAST();
    ast->ident = *unique_ptr<string>($1);
    ast->mode = 2;
    $$ = ast;  
  }
  | IDENT '(' FuncRParamArr ')' {
    auto ast = new UnaryExpAST();
    ast->ident = *unique_ptr<string>($1);
    ast->func_rparam_arr = unique_ptr<BaseAST>($3);
    ast->mode = 3;
    $$ = ast;  
  }
  | '+' UnaryExp {
    auto ast = new UnaryExpAST();
    ast->unary_exp = unique_ptr<BaseAST>($2);
    ast->mode = 4;
    $$ = ast;
  }
  | '-' UnaryExp {
    auto ast = new UnaryExpAST();
    ast->unary_exp = unique_ptr<BaseAST>($2);
    ast->mode = 5;
    $$ = ast;
  }
  | '!' UnaryExp {
    auto ast = new UnaryExpAST();
    ast->unary_exp = unique_ptr<BaseAST>($2);
    ast->mode = 6;
    $$ = ast;
  }
  ;

// FuncRParamArr ::= FuncRParamArr "," FuncRParam | FuncRParam
FuncRParamArr
  : FuncRParamArr ',' FuncRParam {
    auto ast = new FuncRParamArrAST();
    ast->func_rparam_arr = unique_ptr<BaseAST>($1);
    ast->func_rparam = unique_ptr<BaseAST>($3);
    ast->mode = 1;
    $$ = ast;
  }
  | FuncRParam {
    auto ast = new FuncRParamArrAST();
    ast->func_rparam = unique_ptr<BaseAST>($1);
    ast->mode = 2;
    $$ = ast;
  }
  ;

// FuncRParam ::= Exp
FuncRParam
  : Exp {
    auto ast = new FuncRParamAST();
    ast->exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

// MulExp ::= UnaryExp | MulExp ("*" | "/" | "%") UnaryExp
MulExp
  : UnaryExp {
    auto ast = new MulExpAST();
    ast->unary_exp = unique_ptr<BaseAST>($1);
    ast->mode = 1;
    $$ = ast;
  }
  | MulExp '*' UnaryExp {
    auto ast = new MulExpAST();
    ast->mul_exp = unique_ptr<BaseAST>($1);
    ast->unary_exp = unique_ptr<BaseAST>($3);
    ast->mode = 2;
    $$ = ast;
  }
  | MulExp '/' UnaryExp {
    auto ast = new MulExpAST();
    ast->mul_exp = unique_ptr<BaseAST>($1);
    ast->unary_exp = unique_ptr<BaseAST>($3);
    ast->mode = 3;
    $$ = ast;
  }
  | MulExp '%' UnaryExp {
    auto ast = new MulExpAST();
    ast->mul_exp = unique_ptr<BaseAST>($1);
    ast->unary_exp = unique_ptr<BaseAST>($3);
    ast->mode = 4;
    $$ = ast;
  }
  ;

// AddExp ::= MulExp | AddExp ("+" | "-") MulExp
AddExp
  : MulExp {
    auto ast = new AddExpAST();
    ast->mul_exp = unique_ptr<BaseAST>($1);
    ast->mode = 1;
    $$ = ast;
  }
  | AddExp '+' MulExp {
    auto ast = new AddExpAST();
    ast->add_exp = unique_ptr<BaseAST>($1);
    ast->mul_exp = unique_ptr<BaseAST>($3);
    ast->mode = 2;
    $$ = ast;
  }
  | AddExp '-' MulExp {
    auto ast = new AddExpAST();
    ast->add_exp = unique_ptr<BaseAST>($1);
    ast->mul_exp = unique_ptr<BaseAST>($3);
    ast->mode = 3;
    $$ = ast;
  }
  ;

// RelExp ::= AddExp | RelExp ("<" | ">" | "<=" | ">=") AddExp
RelExp
  : AddExp {
    auto ast = new RelExpAST();
    ast->add_exp = unique_ptr<BaseAST>($1);
    ast->mode = 1;
    $$ = ast;
  }
  | RelExp '<' AddExp {
    auto ast = new RelExpAST();
    ast->rel_exp = unique_ptr<BaseAST>($1);
    ast->add_exp = unique_ptr<BaseAST>($3);
    ast->mode = 2;
    $$ = ast;
  }
  | RelExp '>' AddExp {
    auto ast = new RelExpAST();
    ast->rel_exp = unique_ptr<BaseAST>($1);
    ast->add_exp = unique_ptr<BaseAST>($3);
    ast->mode = 3;
    $$ = ast;
  }
  | RelExp LE AddExp {
    auto ast = new RelExpAST();
    ast->rel_exp = unique_ptr<BaseAST>($1);
    ast->add_exp = unique_ptr<BaseAST>($3);
    ast->mode = 4;
    $$ = ast;
  }
  | RelExp GE AddExp {
    auto ast = new RelExpAST();
    ast->rel_exp = unique_ptr<BaseAST>($1);
    ast->add_exp = unique_ptr<BaseAST>($3);
    ast->mode = 5;
    $$ = ast;
  }
  ;

// EqExp ::= RelExp | EqExp ("==" | "!=") RelExp
EqExp
  : RelExp {
    auto ast = new EqExpAST();
    ast->rel_exp = unique_ptr<BaseAST>($1);
    ast->mode = 1;
    $$ = ast;
  }
  | EqExp EQ RelExp {
    auto ast = new EqExpAST();
    ast->eq_exp = unique_ptr<BaseAST>($1);
    ast->rel_exp = unique_ptr<BaseAST>($3);
    ast->mode = 2;
    $$ = ast;
  }
  | EqExp NE RelExp {
    auto ast = new EqExpAST();
    ast->eq_exp = unique_ptr<BaseAST>($1);
    ast->rel_exp = unique_ptr<BaseAST>($3);
    ast->mode = 3;
    $$ = ast;
  }
  ;

// LAndExp ::= EqExp | LAndExp "&&" EqExp
LAndExp
  : EqExp {
    auto ast = new LAndExpAST();
    ast->eq_exp = unique_ptr<BaseAST>($1);
    ast->mode = 1;
    $$ = ast;
  }
  | LAndExp AND EqExp {
    auto ast = new LAndExpAST();
    ast->land_exp = unique_ptr<BaseAST>($1);
    ast->eq_exp = unique_ptr<BaseAST>($3);
    ast->mode = 2;
    $$ = ast;
  }
  ;

// LOrExp ::= LAndExp | LOrExp "||" LAndExp
LOrExp
  : LAndExp {
    auto ast = new LOrExpAST();
    ast->land_exp = unique_ptr<BaseAST>($1);
    ast->mode = 1;
    $$ = ast;
  }
  | LOrExp OR LAndExp {
    auto ast = new LOrExpAST();
    ast->lor_exp = unique_ptr<BaseAST>($1);
    ast->land_exp = unique_ptr<BaseAST>($3);
    ast->mode = 2;
    $$ = ast;
  }
  ;

// ConstExp ::= Exp
ConstExp
  : Exp {
    auto ast = new ConstExpAST();
    ast->exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

%%

// 定义错误处理函数, 其中第二个参数是错误信息
// parser 如果发生错误 (例如输入的程序出现了语法错误), 就会调用这个函数
void yyerror(unique_ptr<BaseAST> &ast, const char *s) {
  cerr << "error: " << s << endl;
}
