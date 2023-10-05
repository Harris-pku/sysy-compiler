#include <cassert>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <memory>
#include <string>
#include <stack>
#include <map>
#include "ast.hpp"
#include "sym.hpp"

using namespace std;

// 声明 lexer 的输入, 以及 parser 函数
// 为什么不引用 sysy.tab.hpp 呢? 因为首先里面没有 yyin 的定义
// 其次, 因为这个文件不是我们自己写的, 而是被 Bison 生成出来的
// 你的代码编辑器/IDE 很可能找不到这个文件, 然后会给你报错 (虽然编译不会出错)
// 看起来会很烦人, 于是干脆采用这种看起来 dirty 但实际很有效的手段
extern FILE *yyin;
extern FILE *yyout;
extern int yyparse(unique_ptr<BaseAST> &ast);
extern void solve_koopa(char *str);
void init_str(char *str, std::map<std::string, sym_t>* val_ma);

int cnt;
stack<num_t>* val_st = new stack<num_t>;
stack<int>* loop_cur = new stack<int>;
map<string, sym_t>* val_ma = new map<string, sym_t>;

int main(int argc, const char *argv[]) {
    // 解析命令行参数. 测试脚本/评测平台要求你的编译器能接收如下参数:
    // compiler 模式 输入文件 -o 输出文件
    assert(argc == 5);
    auto mode = argv[1];
    auto input = argv[2];
    auto output = argv[4];

    char *str = (char *)malloc(10000 * sizeof(char));

    // 打开输入文件, 并且指定 lexer 在解析的时候读取这个文件
    yyin = freopen(input, "r", stdin);
    assert(yyin);

    // 打开输出文件
    yyout = freopen(output, "w", stdout);
    // assert(yyout);

    init_str(str, val_ma);

    // 调用 parser 函数, parser 函数会进一步调用 lexer 解析输入文件的
    unique_ptr<BaseAST> ast;
    auto ret = yyparse(ast);
    assert(!ret);
    
    // 输出解析得到的 AST, 其实就是个字符串
    ast->Dump(str, cnt, loop_cur, val_st, 0, val_ma);
    if (mode[1] == 'k'){
        cout << str;
    } else if (mode[1] == 'r'){
        solve_koopa(str);
    } else {
        cerr << "Unknown Parameters!" << endl;
    }

    (*val_ma).clear();
    free(val_st);
    free(loop_cur);
    free(val_ma);
    free(str);

    fclose(stdin);
    fclose(stdout);
    return 0;
}

void init_str(char *str, std::map<std::string, sym_t>* val_ma){
  char stmp[50];
  sprintf(stmp, "decl @getint(): i32\ndecl @getch(): i32\n");
  strcat(str, stmp);
  sprintf(stmp, "decl @getarray(*i32): i32\ndecl @putint(i32)\n");
  strcat(str, stmp);
  sprintf(stmp, "decl @putch(i32)\ndecl @putarray(i32, *i32)\n");
  strcat(str, stmp);
  sprintf(stmp, "decl @starttime()\ndecl @stoptime()\n\n");
  strcat(str, stmp);

  sym_t tmp_loop;
  tmp_loop.val_t = 0;
  std::string s;

  tmp_loop.type = 2;
  s = "getint";
  (*val_ma)[s] = tmp_loop;
  s = "getch";
  (*val_ma)[s] = tmp_loop;
  s = "getarray";
  (*val_ma)[s] = tmp_loop;

  tmp_loop.type = 3;
  s = "putint";
  (*val_ma)[s] = tmp_loop;
  s = "putch";
  (*val_ma)[s] = tmp_loop;
  s = "putarray";
  (*val_ma)[s] = tmp_loop;
  s = "starttime";
  (*val_ma)[s] = tmp_loop;
  s = "stoptime";
  (*val_ma)[s] = tmp_loop;
}
