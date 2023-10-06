#include <cassert>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <memory>
#include <stack>
#include <map>
#include <string>
#include <cstring>
#include "sym.hpp"
#include "koopa.h"

int used = 0;
int sum_stack;
std::stack<num_t> value_st;
std::map<std::string, sym_t> value_ma;

// 访问 integer
void Visit_integer(const koopa_raw_integer_t &integer){
  int value = integer.value;
  std::cout << value;
}

// 访问 binary 指令
void Visit_binary(const koopa_raw_binary_t &binary){
  koopa_raw_value_t lhs = binary.lhs;
  koopa_raw_value_t rhs = binary.rhs;
  num_t tmpnum, tmpnum1, tmpnum2;
  switch (binary.op){
    // Not equal to
    case KOOPA_RBO_NOT_EQ:
      if (lhs->kind.tag == KOOPA_RVT_INTEGER){
        printf("  li    t0, ");
        Visit_integer(lhs->kind.data.integer);
        std::cout << std::endl;
      }
      else{
        tmpnum1 = value_st.top();
        value_st.pop();
        printf("  lw    t0, %d(sp)\n", tmpnum1.num_val);
      }
      if (rhs->kind.tag == KOOPA_RVT_INTEGER){
        printf("  li    t1, ");
        Visit_integer(rhs->kind.data.integer);
        std::cout << std::endl;
      }
      else{
        tmpnum2 = value_st.top();
        value_st.pop();
        printf("  lw    t1, %d(sp)\n", tmpnum2.num_val);
      }
      printf("  xor   t0, t0, t1\n");
      printf("  snez  t0, t0\n");
      printf("  sw    t0, %d(sp)\n", used);
      tmpnum.num_val = used;
      value_st.push(tmpnum);
      used += 4;
      break;
    
    // Equal to
    case KOOPA_RBO_EQ:
      if (lhs->kind.tag == KOOPA_RVT_INTEGER){
        printf("  li    t0, ");
        Visit_integer(lhs->kind.data.integer);
        std::cout << std::endl;
      }
      else{
        tmpnum1 = value_st.top();
        value_st.pop();
        printf("  lw    t0, %d(sp)\n", tmpnum1.num_val);
      }
      if (rhs->kind.tag == KOOPA_RVT_INTEGER){
        printf("  li    t1, ");
        Visit_integer(rhs->kind.data.integer);
        std::cout << std::endl;
      }
      else{
        tmpnum2 = value_st.top();
        value_st.pop();
        printf("  lw    t1, %d(sp)\n", tmpnum2.num_val);
      }
      printf("  xor   t0, t0, t1\n");
      printf("  seqz  t0, t0\n");
      printf("  sw    t0, %d(sp)\n", used);
      tmpnum.num_val = used;
      value_st.push(tmpnum);
      used += 4;
      break;
    
    // Greater than
    case KOOPA_RBO_GT:
      if (lhs->kind.tag == KOOPA_RVT_INTEGER){
        printf("  li    t0, ");
        Visit_integer(lhs->kind.data.integer);
        std::cout << std::endl;
      }
      else{
        tmpnum1 = value_st.top();
        value_st.pop();
        printf("  lw    t0, %d(sp)\n", tmpnum1.num_val);
      }
      if (rhs->kind.tag == KOOPA_RVT_INTEGER){
        printf("  li    t1, ");
        Visit_integer(rhs->kind.data.integer);
        std::cout << std::endl;
      }
      else{
        tmpnum2 = value_st.top();
        value_st.pop();
        printf("  lw    t1, %d(sp)\n", tmpnum2.num_val);
      }
      printf("  sgt   t0, t0, t1\n");
      printf("  sw    t0, %d(sp)\n", used);
      tmpnum.num_val = used;
      value_st.push(tmpnum);
      used += 4;
      break;
    
    // Less than
    case KOOPA_RBO_LT:
      if (lhs->kind.tag == KOOPA_RVT_INTEGER){
        printf("  li    t0, ");
        Visit_integer(lhs->kind.data.integer);
        std::cout << std::endl;
      }
      else{
        tmpnum1 = value_st.top();
        value_st.pop();
        printf("  lw    t0, %d(sp)\n", tmpnum1.num_val);
      }
      if (rhs->kind.tag == KOOPA_RVT_INTEGER){
        printf("  li    t1, ");
        Visit_integer(rhs->kind.data.integer);
        std::cout << std::endl;
      }
      else{
        tmpnum2 = value_st.top();
        value_st.pop();
        printf("  lw    t1, %d(sp)\n", tmpnum2.num_val);
      }
      printf("  slt   t0, t0, t1\n");
      printf("  sw    t0, %d(sp)\n", used);
      tmpnum.num_val = used;
      value_st.push(tmpnum);
      used += 4;
      break;
    
    // Greater than or equal to
    case KOOPA_RBO_GE:
      if (lhs->kind.tag == KOOPA_RVT_INTEGER){
        printf("  li    t0, ");
        Visit_integer(lhs->kind.data.integer);
        std::cout << std::endl;
      }
      else{
        tmpnum1 = value_st.top();
        value_st.pop();
        printf("  lw    t0, %d(sp)\n", tmpnum1.num_val);
      }
      if (rhs->kind.tag == KOOPA_RVT_INTEGER){
        printf("  li    t1, ");
        Visit_integer(rhs->kind.data.integer);
        std::cout << std::endl;
      }
      else{
        tmpnum2 = value_st.top();
        value_st.pop();
        printf("  lw    t1, %d(sp)\n", tmpnum2.num_val);
      }
      printf("  slt   t0, t0, t1\n");
      printf("  seqz  t0, t0\n");
      printf("  sw    t0, %d(sp)\n", used);
      tmpnum.num_val = used;
      value_st.push(tmpnum);
      used += 4;
      break;
    
    // Less than or equal to
    case KOOPA_RBO_LE:
      if (lhs->kind.tag == KOOPA_RVT_INTEGER){
        printf("  li    t0, ");
        Visit_integer(lhs->kind.data.integer);
        std::cout << std::endl;
      }
      else{
        tmpnum1 = value_st.top();
        value_st.pop();
        printf("  lw    t0, %d(sp)\n", tmpnum1.num_val);
      }
      if (rhs->kind.tag == KOOPA_RVT_INTEGER){
        printf("  li    t1, ");
        Visit_integer(rhs->kind.data.integer);
        std::cout << std::endl;
      }
      else{
        tmpnum2 = value_st.top();
        value_st.pop();
        printf("  lw    t1, %d(sp)\n", tmpnum2.num_val);
      }
      printf("  sgt   t0, t0, t1\n");
      printf("  seqz  t0, t0\n");
      printf("  sw    t0, %d(sp)\n", used);
      tmpnum.num_val = used;
      value_st.push(tmpnum);
      used += 4;
      break;
    
    // Addition
    case KOOPA_RBO_ADD:
      if (lhs->kind.tag == KOOPA_RVT_INTEGER){
        printf("  li    t0, ");
        Visit_integer(lhs->kind.data.integer);
        std::cout << std::endl;
      }
      else{
        tmpnum1 = value_st.top();
        value_st.pop();
        printf("  lw    t0, %d(sp)\n", tmpnum1.num_val);
      }
      if (rhs->kind.tag == KOOPA_RVT_INTEGER){
        printf("  li    t1, ");
        Visit_integer(rhs->kind.data.integer);
        std::cout << std::endl;
      }
      else{
        tmpnum2 = value_st.top();
        value_st.pop();
        printf("  lw    t1, %d(sp)\n", tmpnum2.num_val);
      }
      printf("  add   t0, t0, t1\n");
      printf("  sw    t0, %d(sp)\n", used);
      tmpnum.num_val = used;
      value_st.push(tmpnum);
      used += 4;
      break;
    
    // Subtraction
    case KOOPA_RBO_SUB:
      if (lhs->kind.tag == KOOPA_RVT_INTEGER){
        printf("  li    t0, ");
        Visit_integer(lhs->kind.data.integer);
        std::cout << std::endl;
      }
      else{
        tmpnum1 = value_st.top();
        value_st.pop();
        printf("  lw    t0, %d(sp)\n", tmpnum1.num_val);
      }
      if (rhs->kind.tag == KOOPA_RVT_INTEGER){
        printf("  li    t1, ");
        Visit_integer(rhs->kind.data.integer);
        std::cout << std::endl;
      }
      else{
        tmpnum2 = value_st.top();
        value_st.pop();
        printf("  lw    t1, %d(sp)\n", tmpnum2.num_val);
      }
      printf("  sub   t0, t0, t1\n");
      printf("  sw    t0, %d(sp)\n", used);
      tmpnum.num_val = used;
      value_st.push(tmpnum);
      used += 4;
      break;
    
    // Multiplication
    case KOOPA_RBO_MUL:
      if (lhs->kind.tag == KOOPA_RVT_INTEGER){
        printf("  li    t0, ");
        Visit_integer(lhs->kind.data.integer);
        std::cout << std::endl;
      }
      else{
        tmpnum1 = value_st.top();
        value_st.pop();
        printf("  lw    t0, %d(sp)\n", tmpnum1.num_val);
      }
      if (rhs->kind.tag == KOOPA_RVT_INTEGER){
        printf("  li    t1, ");
        Visit_integer(rhs->kind.data.integer);
        std::cout << std::endl;
      }
      else{
        tmpnum2 = value_st.top();
        value_st.pop();
        printf("  lw    t1, %d(sp)\n", tmpnum2.num_val);
      }
      printf("  mul   t0, t0, t1\n");
      printf("  sw    t0, %d(sp)\n", used);
      tmpnum.num_val = used;
      value_st.push(tmpnum);
      used += 4;
      break;
    
    // Division
    case KOOPA_RBO_DIV:
      if (lhs->kind.tag == KOOPA_RVT_INTEGER){
        printf("  li    t0, ");
        Visit_integer(lhs->kind.data.integer);
        std::cout << std::endl;
      }
      else{
        tmpnum1 = value_st.top();
        value_st.pop();
        printf("  lw    t0, %d(sp)\n", tmpnum1.num_val);
      }
      if (rhs->kind.tag == KOOPA_RVT_INTEGER){
        printf("  li    t1, ");
        Visit_integer(rhs->kind.data.integer);
        std::cout << std::endl;
      }
      else{
        tmpnum2 = value_st.top();
        value_st.pop();
        printf("  lw    t1, %d(sp)\n", tmpnum2.num_val);
      }
      printf("  div   t0, t0, t1\n");
      printf("  sw    t0, %d(sp)\n", used);
      tmpnum.num_val = used;
      value_st.push(tmpnum);
      used += 4;
      break;
    
    // Modulo
    case KOOPA_RBO_MOD:
      if (lhs->kind.tag == KOOPA_RVT_INTEGER){
        printf("  li    t0, ");
        Visit_integer(lhs->kind.data.integer);
        std::cout << std::endl;
      }
      else{
        tmpnum1 = value_st.top();
        value_st.pop();
        printf("  lw    t0, %d(sp)\n", tmpnum1.num_val);
      }
      if (rhs->kind.tag == KOOPA_RVT_INTEGER){
        printf("  li    t1, ");
        Visit_integer(rhs->kind.data.integer);
        std::cout << std::endl;
      }
      else{
        tmpnum2 = value_st.top();
        value_st.pop();
        printf("  lw    t1, %d(sp)\n", tmpnum2.num_val);
      }
      printf("  rem   t0, t0, t1\n");
      printf("  sw    t0, %d(sp)\n", used);
      tmpnum.num_val = used;
      value_st.push(tmpnum);
      used += 4;
      break;
    
    // Bitwise AND
    case KOOPA_RBO_AND:
      if (lhs->kind.tag == KOOPA_RVT_INTEGER){
        printf("  li    t0, ");
        Visit_integer(lhs->kind.data.integer);
        std::cout << std::endl;
      }
      else{
        tmpnum1 = value_st.top();
        value_st.pop();
        printf("  lw    t0, %d(sp)\n", tmpnum1.num_val);
      }
      if (rhs->kind.tag == KOOPA_RVT_INTEGER){
        printf("  li    t1, ");
        Visit_integer(rhs->kind.data.integer);
        std::cout << std::endl;
      }
      else{
        tmpnum2 = value_st.top();
        value_st.pop();
        printf("  lw    t1, %d(sp)\n", tmpnum2.num_val);
      }
      printf("  and   t0, t0, t1\n");
      printf("  sw    t0, %d(sp)\n", used);
      tmpnum.num_val = used;
      value_st.push(tmpnum);
      used += 4;
      break;
    
    // Bitwise OR
    case KOOPA_RBO_OR:
      if (lhs->kind.tag == KOOPA_RVT_INTEGER){
        printf("  li    t0, ");
        Visit_integer(lhs->kind.data.integer);
        std::cout << std::endl;
      }
      else{
        tmpnum1 = value_st.top();
        value_st.pop();
        printf("  lw    t0, %d(sp)\n", tmpnum1.num_val);
      }
      if (rhs->kind.tag == KOOPA_RVT_INTEGER){
        printf("  li    t1, ");
        Visit_integer(rhs->kind.data.integer);
        std::cout << std::endl;
      }
      else{
        tmpnum2 = value_st.top();
        value_st.pop();
        printf("  lw    t1, %d(sp)\n", tmpnum2.num_val);
      }
      printf("  or    t0, t0, t1\n");
      printf("  sw    t0, %d(sp)\n", used);
      tmpnum.num_val = used;
      value_st.push(tmpnum);
      used += 4;
      break;  
    default:
      assert(false);
      break;
  }
}

// 访问 alloc 指令
void Visit_alloc(const koopa_raw_global_alloc_t &alloc){
  // koopa_raw_value_t init = alloc.init;
  // std::cout << "alloc tag = " << init->kind.tag << std::endl;
  // switch (init->kind.tag){
  //   case KOOPA_RVT_INTEGER:
  //     break;
  //   default:
  //     break;
  // }
}

// 访问 global alloc 指令
void Visit_global_alloc(const koopa_raw_global_alloc_t &global_alloc){

}

// 访问 load 指令
void Visit_load(const koopa_raw_load_t &load){
  koopa_raw_value_t src = load.src;
  std::string ident = src->name;
  int load_src = value_ma[ident].val_t;
  num_t tmpnum;
  switch (src->kind.tag){
    case KOOPA_RVT_ALLOC:
      printf("  lw    t0, %d(sp)\n", load_src);
      printf("  sw    t0, %d(sp)\n", used);
      tmpnum.num_val = used;
      value_st.push(tmpnum);
      used += 4;
      break;
    case KOOPA_RVT_GLOBAL_ALLOC:
      break;
    default:
      break;
  }
}

// 访问 store 指令
void Visit_store(const koopa_raw_store_t &store){
  koopa_raw_value_t value = store.value;
  koopa_raw_value_t dest = store.dest;
  std::string ident = dest->name;
  int store_dest = 0;
  sym_t tmpsym;
  num_t tmpnum;
  if (!value_ma.count(ident)){
    store_dest = used;
    tmpsym.val_t = used;
    value_ma[ident] = tmpsym;
    used += 4;
  }
  else{
    store_dest = value_ma[ident].val_t;
  }
  switch (value->kind.tag){
    // 数字
    case KOOPA_RVT_INTEGER:
      printf("  li    t0, ");
      Visit_integer(value->kind.data.integer);
      printf("\n");
      printf("  sw    t0, %d(sp)\n", store_dest);
      break;
    // 运算结果
    case KOOPA_RVT_BINARY:
      tmpnum = value_st.top();
      value_st.pop();
      printf("  lw    t0, %d(sp)\n", tmpnum.num_val);
      printf("  sw    t0, %d(sp)\n", store_dest);
      break;
    default:
      assert(false);
      break;
  }
}

// 访问 branch
void Visit_branch(const koopa_raw_branch_t &branch){

}

// 访问 jump
void Visit_jump(const koopa_raw_jump_t &jump){

}

// 访问 call
void Visit_call(const koopa_raw_call_t &call){
  
}

// 访问 return 指令
void Visit_ret(const koopa_raw_return_t &ret){
  koopa_raw_value_t ret_value = ret.value;
  num_t tmp_num;
  switch (ret_value->kind.tag){
    // 直接返回数字
    case KOOPA_RVT_INTEGER:
      printf("  li    a0, ");
      Visit_integer(ret_value->kind.data.integer);
      printf("\n");
      printf("  addi  sp, sp, %d\n", sum_stack*4);
      printf("  ret\n");
      break;
    // 返回运算结果
    case KOOPA_RVT_BINARY:
      tmp_num = value_st.top();
      value_st.pop();
      printf("  lw    a0, %d(sp)\n", tmp_num.num_val);
      printf("  li    t0, %d\n", sum_stack*4);
      printf("  add   sp, sp, t0\n");
      printf("  ret\n");
      break;
    case KOOPA_RVT_LOAD:
      tmp_num = value_st.top();
      value_st.pop();
      printf("  lw    a0, %d(sp)\n", tmp_num.num_val);
      printf("  li    t0, %d\n", sum_stack*4);
      printf("  add   sp, sp, t0\n");
      printf("  ret\n");
      break;
    default:
      assert(false);
      break;
  }
}

// 访问指令
void Visit_inst(const koopa_raw_value_t &value){
  // 根据指令类型判断后续需要如何访问
  const auto &kind = value->kind;
  switch (kind.tag) {
    case KOOPA_RVT_INTEGER:
      // 访问 integer 指令
      Visit_integer(kind.data.integer);
      break;
    
    case KOOPA_RVT_ALLOC:
      // 访问 alloc 指令
      Visit_alloc(kind.data.global_alloc);
      break;
    
    case KOOPA_RVT_GLOBAL_ALLOC:
      // 访问 global alloc 指令
      Visit_global_alloc(kind.data.global_alloc);
      break;

    case KOOPA_RVT_LOAD:
      // 访问 load 指令
      Visit_load(kind.data.load);
      break;
    
    case KOOPA_RVT_STORE:
      // 访问 store 指令
      Visit_store(kind.data.store);
      break;
    
    case KOOPA_RVT_BRANCH:
      // 访问 branch 指令
      Visit_branch(kind.data.branch);
      break;
    
    case KOOPA_RVT_JUMP:
      // 访问 jump 指令
      Visit_jump(kind.data.jump);
      break;
    
    case KOOPA_RVT_CALL:
      // 访问 call 指令
      Visit_call(kind.data.call);
      break;

    case KOOPA_RVT_BINARY:
      // 访问 binary 指令
      Visit_binary(kind.data.binary);
      break;
    
    case KOOPA_RVT_RETURN:
      // 访问 return 指令
      Visit_ret(kind.data.ret);
      break;
    
    default:
      // 其他类型暂时遇不到
      assert(false);
      break;
  }
}

// 访问基本块
void Visit_block(const koopa_raw_basic_block_t &bb){
  // 执行一些其他的必要操作
  // ...
  // 访问所有指令
  for (size_t i = 0; i < bb->insts.len; ++i){
      auto ptr = bb->insts.buffer[i];
      Visit_inst(reinterpret_cast<koopa_raw_value_t>(ptr));
  }
}

// 计算栈帧
int Cal_block(const koopa_raw_basic_block_t &bb){
  int tmp = 0;
  // std::cout << "bbs.len = " << bb->insts.len << std::endl;
  for (size_t i = 0; i < bb->insts.len; ++i){
    auto ptr = bb->insts.buffer[i];
    const koopa_raw_value_t &inst = reinterpret_cast<koopa_raw_value_t>(ptr);
    if (inst->ty->tag != KOOPA_RTT_UNIT) tmp++;
  }
  return tmp;
}

// 访问函数
void Visit_func(const koopa_raw_function_t &func){
  // 执行一些其他的必要操作
  std::cout << "main:" << std::endl;

  // 访问所有基本块
  for (size_t i = 0; i < func->bbs.len; ++i){
    auto ptr = func->bbs.buffer[i];
    sum_stack += Cal_block(reinterpret_cast<koopa_raw_basic_block_t>(ptr));
    // std::cout << "sum_stack = " << sum_stack << std::endl;
  }
  sum_stack += 8;

  printf("  li    t0, -%d\n", sum_stack*4);
  printf("  add   sp, sp, t0\n");

  for (size_t i = 0; i < func->bbs.len; ++i){
    auto ptr = func->bbs.buffer[i];
    Visit_block(reinterpret_cast<koopa_raw_basic_block_t>(ptr));
  }
}

// 访问 raw slice
void Visit_slice(const koopa_raw_slice_t &slice){
  for (size_t i = 0; i < slice.len; ++i){
    auto ptr = slice.buffer[i];
    // 根据 slice 的 kind 决定将 ptr 视作何种元素
    switch (slice.kind){
      case KOOPA_RSIK_FUNCTION:
        // 访问函数
        Visit_func(reinterpret_cast<koopa_raw_function_t>(ptr));
        break;
      case KOOPA_RSIK_BASIC_BLOCK:
        // 访问基本块
        Visit_block(reinterpret_cast<koopa_raw_basic_block_t>(ptr));
        break;
      case KOOPA_RSIK_VALUE:
        // 访问指令
        Visit_inst(reinterpret_cast<koopa_raw_value_t>(ptr));
        break;
      default:
        // 我们暂时不会遇到其他内容, 于是不对其做任何处理
        assert(false);
    }
  }
}

// 访问 raw program
void Visit_pro(const koopa_raw_program_t &program){
  // 执行一些其他的必要操作
  std::cout << "  .text" << std::endl;
  std::cout << "  .global main" << std::endl;

  // 访问所有全局变量
  Visit_slice(program.values);
  // 访问所有函数
  Visit_slice(program.funcs);
}

void solve_koopa(char *str){
    // 解析字符串 str, 得到 Koopa IR 程序
    koopa_program_t program;
    koopa_error_code_t ret = koopa_parse_from_string(str, &program);
    assert(ret == KOOPA_EC_SUCCESS);  // 确保解析时没有出错
    // 创建一个 raw program builder, 用来构建 raw program
    koopa_raw_program_builder_t builder = koopa_new_raw_program_builder();
    // 将 Koopa IR 程序转换为 raw program
    koopa_raw_program_t raw = koopa_build_raw_program(builder, program);
    // 释放 Koopa IR 程序占用的内存
    koopa_delete_program(program);

    // 处理 raw program
    Visit_pro(raw);

    // 处理完成, 释放 raw program builder 占用的内存
    // 注意, raw program 中所有的指针指向的内存均为 raw program builder 的内存
    // 所以不要在 raw program 处理完毕之前释放 builder
    koopa_delete_raw_program_builder(builder);
}