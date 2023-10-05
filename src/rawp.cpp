#include <cassert>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <memory>
#include <stack>
#include <string>
#include <cstring>
#include "sym.hpp"
#include "koopa.h"

int cnt_reg = -1;
int sum_stack;
std::stack<num_t> value_st;
const char reg[32][5] = {"t0", "t1", "t2", "t3", "t4", "t5", "t6",
                         "a1", "a2", "a3", "a4", "a5", "a6", "a7", "a0"};

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
        cnt_reg++;
        printf("  li    %s, ", reg[cnt_reg]);
        Visit_integer(lhs->kind.data.integer);
        std::cout << std::endl;
        if (rhs->kind.tag == KOOPA_RVT_INTEGER){
          if (rhs->kind.data.integer.value == 0){
            printf("  xor   %s, %s, x0\n", reg[cnt_reg], reg[cnt_reg]);
            printf("  snez  %s, %s\n", reg[cnt_reg], reg[cnt_reg]);
            tmpnum.num_val = cnt_reg;
            value_st.push(tmpnum);
          }
          else{
            cnt_reg++;
            printf("  li    %s, ", reg[cnt_reg]);
            Visit_integer(rhs->kind.data.integer);
            std::cout << std::endl;
            printf("  xor   %s, %s, %s\n", reg[cnt_reg], reg[cnt_reg-1], reg[cnt_reg]);
            printf("  snez  %s, %s\n", reg[cnt_reg], reg[cnt_reg]);
            tmpnum.num_val = cnt_reg;
            value_st.push(tmpnum);
          }
        }
        else{
          tmpnum2 = value_st.top();
          value_st.pop();
          printf("  xor   %s, %s, %s\n", reg[cnt_reg], reg[cnt_reg], reg[tmpnum2.num_val]);
          printf("  snez  %s, %s\n", reg[cnt_reg], reg[cnt_reg]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
      }
      else{
        if (rhs->kind.tag == KOOPA_RVT_INTEGER){
          tmpnum1 = value_st.top();
          value_st.pop();
          if (rhs->kind.data.integer.value == 0){
            printf("  xor   %s, %s, x0\n", reg[tmpnum1.num_val], reg[tmpnum1.num_val]);
            printf("  snez  %s, %s\n", reg[tmpnum1.num_val], reg[tmpnum1.num_val]);
            tmpnum.num_val = tmpnum1.num_val;
            value_st.push(tmpnum);
          }
          else{
            cnt_reg++;
            printf("  li    %s, ", reg[cnt_reg]);
            Visit_integer(rhs->kind.data.integer);
            std::cout << std::endl;
            // std::cout << "---" << std::endl;
            printf("  xor   %s, %s, %s\n", reg[cnt_reg], reg[tmpnum1.num_val], reg[cnt_reg]);
            printf("  snez  %s, %s\n", reg[cnt_reg], reg[cnt_reg]);
            tmpnum.num_val = cnt_reg;
            value_st.push(tmpnum);
          }
        }
        else{
          tmpnum2 = value_st.top();
          value_st.pop();
          tmpnum1 = value_st.top();
          value_st.pop();
          printf("  xor   %s, %s, %s\n", reg[tmpnum2.num_val], reg[tmpnum1.num_val], reg[tmpnum2.num_val]);
          printf("  seqz  %s, %s\n", reg[tmpnum2.num_val], reg[tmpnum2.num_val]);
          tmpnum.num_val = tmpnum2.num_val;
          value_st.push(tmpnum);
        }
      }
      break;

    // Equal to
    case KOOPA_RBO_EQ:
      if (lhs->kind.tag == KOOPA_RVT_INTEGER){
        cnt_reg++;
        printf("  li    %s, ", reg[cnt_reg]);
        Visit_integer(lhs->kind.data.integer);
        std::cout << std::endl;
        if (rhs->kind.tag == KOOPA_RVT_INTEGER){
          if (rhs->kind.data.integer.value == 0){
            printf("  xor   %s, %s, x0\n", reg[cnt_reg], reg[cnt_reg]);
            printf("  seqz  %s, %s\n", reg[cnt_reg], reg[cnt_reg]);
            tmpnum.num_val = cnt_reg;
            value_st.push(tmpnum);
          }
          else{
            cnt_reg++;
            printf("  li    %s, ", reg[cnt_reg]);
            Visit_integer(rhs->kind.data.integer);
            std::cout << std::endl;
            printf("  xor   %s, %s, %s\n", reg[cnt_reg], reg[cnt_reg-1], reg[cnt_reg]);
            printf("  seqz  %s, %s\n", reg[cnt_reg], reg[cnt_reg]);
            tmpnum.num_val = cnt_reg;
            value_st.push(tmpnum);
          }
        }
        else{
          tmpnum2 = value_st.top();
          value_st.pop();
          printf("  xor   %s, %s, %s\n", reg[cnt_reg], reg[cnt_reg], reg[tmpnum2.num_val]);
          printf("  seqz  %s, %s\n", reg[cnt_reg], reg[cnt_reg]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
      }
      else{
        if (rhs->kind.tag == KOOPA_RVT_INTEGER){
          tmpnum1 = value_st.top();
          value_st.pop();
          if (rhs->kind.data.integer.value == 0){
            // std::cout << "---" << std::endl;
            printf("  xor   %s, %s, x0\n", reg[tmpnum1.num_val], reg[tmpnum1.num_val]);
            printf("  seqz  %s, %s\n", reg[tmpnum1.num_val], reg[tmpnum1.num_val]);
            tmpnum.num_val = tmpnum1.num_val;
            value_st.push(tmpnum);
          }
          else{
            cnt_reg++;
            printf("  li    %s, ", reg[cnt_reg]);
            Visit_integer(rhs->kind.data.integer);
            std::cout << std::endl;
            printf("  xor   %s, %s, %s\n", reg[cnt_reg], reg[tmpnum1.num_val], reg[cnt_reg]);
            printf("  seqz  %s, %s\n", reg[cnt_reg], reg[cnt_reg]);
            tmpnum.num_val = cnt_reg;
            value_st.push(tmpnum);
          }
        }
        else{
          tmpnum2 = value_st.top();
          value_st.pop();
          tmpnum1 = value_st.top();
          value_st.pop();
          printf("  xor   %s, %s, %s\n", reg[tmpnum2.num_val], reg[tmpnum1.num_val], reg[tmpnum2.num_val]);
          printf("  seqz  %s, %s\n", reg[tmpnum2.num_val], reg[tmpnum2.num_val]);
          tmpnum.num_val = tmpnum2.num_val;
          value_st.push(tmpnum);
        }
      }
      break;

    // Greater than
    case KOOPA_RBO_GT:
      if (lhs->kind.tag == KOOPA_RVT_INTEGER){
        cnt_reg++;
        printf("  li    %s, ", reg[cnt_reg]);
        Visit_integer(lhs->kind.data.integer);
        std::cout << std::endl;
        if (rhs->kind.tag == KOOPA_RVT_INTEGER){
          cnt_reg++;
          printf("  li    %s, ", reg[cnt_reg]);
          Visit_integer(rhs->kind.data.integer);
          std::cout << std::endl;
          printf("  sgt   %s, %s, %s\n", reg[cnt_reg], reg[cnt_reg-1], reg[cnt_reg]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
        else{
          tmpnum2 = value_st.top();
          value_st.pop();
          printf("  sgt   %s, %s, %s\n", reg[cnt_reg], reg[cnt_reg], reg[tmpnum2.num_val]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
      }
      else{
        if (rhs->kind.tag == KOOPA_RVT_INTEGER){
          tmpnum1 = value_st.top();
          value_st.pop();
          cnt_reg++;
          printf("  li    %s, ", reg[cnt_reg]);
          Visit_integer(rhs->kind.data.integer);
          std::cout << std::endl;
          printf("  sgt   %s, %s, %s\n", reg[cnt_reg], reg[tmpnum1.num_val], reg[cnt_reg]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
        else{
          tmpnum2 = value_st.top();
          value_st.pop();
          tmpnum1 = value_st.top();
          value_st.pop();
          printf("  sgt   %s, %s, %s\n", reg[tmpnum2.num_val], reg[tmpnum1.num_val], reg[tmpnum2.num_val]);
          tmpnum.num_val = tmpnum2.num_val;
          value_st.push(tmpnum);
        }
      }
      break;

    // Less than
    case KOOPA_RBO_LT:
      if (lhs->kind.tag == KOOPA_RVT_INTEGER){
        cnt_reg++;
        printf("  li    %s, ", reg[cnt_reg]);
        Visit_integer(lhs->kind.data.integer);
        std::cout << std::endl;
        if (rhs->kind.tag == KOOPA_RVT_INTEGER){
          cnt_reg++;
          printf("  li    %s, ", reg[cnt_reg]);
          Visit_integer(rhs->kind.data.integer);
          std::cout << std::endl;
          printf("  slt   %s, %s, %s\n", reg[cnt_reg], reg[cnt_reg-1], reg[cnt_reg]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
        else{
          tmpnum2 = value_st.top();
          value_st.pop();
          printf("  slt   %s, %s, %s\n", reg[cnt_reg], reg[cnt_reg], reg[tmpnum2.num_val]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
      }
      else{
        if (rhs->kind.tag == KOOPA_RVT_INTEGER){
          tmpnum1 = value_st.top();
          value_st.pop();
          cnt_reg++;
          printf("  li    %s, ", reg[cnt_reg]);
          Visit_integer(rhs->kind.data.integer);
          std::cout << std::endl;
          printf("  slt   %s, %s, %s\n", reg[cnt_reg], reg[tmpnum1.num_val], reg[cnt_reg]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
        else{
          tmpnum2 = value_st.top();
          value_st.pop();
          tmpnum1 = value_st.top();
          value_st.pop();
          printf("  slt   %s, %s, %s\n", reg[tmpnum2.num_val], reg[tmpnum1.num_val], reg[tmpnum2.num_val]);
          tmpnum.num_val = tmpnum2.num_val;
          value_st.push(tmpnum);
        }
      }
      break;

    // Greater than or equal to
    case KOOPA_RBO_GE:
      if (lhs->kind.tag == KOOPA_RVT_INTEGER){
        cnt_reg++;
        printf("  li    %s, ", reg[cnt_reg]);
        Visit_integer(lhs->kind.data.integer);
        std::cout << std::endl;
        if (rhs->kind.tag == KOOPA_RVT_INTEGER){
          cnt_reg++;
          printf("  li    %s, ", reg[cnt_reg]);
          Visit_integer(rhs->kind.data.integer);
          std::cout << std::endl;
          printf("  slt   %s, %s, %s\n", reg[cnt_reg], reg[cnt_reg-1], reg[cnt_reg]);
          printf("  seqz  %s, %s\n", reg[cnt_reg], reg[cnt_reg]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
        else{
          tmpnum2 = value_st.top();
          value_st.pop();
          printf("  slt   %s, %s, %s\n", reg[cnt_reg], reg[cnt_reg], reg[tmpnum2.num_val]);
          printf("  seqz  %s, %s\n", reg[cnt_reg], reg[cnt_reg]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
      }
      else{
        if (rhs->kind.tag == KOOPA_RVT_INTEGER){
          tmpnum1 = value_st.top();
          value_st.pop();
          cnt_reg++;
          printf("  li    %s, ", reg[cnt_reg]);
          Visit_integer(rhs->kind.data.integer);
          std::cout << std::endl;
          printf("  slt   %s, %s, %s\n", reg[cnt_reg], reg[tmpnum1.num_val], reg[cnt_reg]);
          printf("  seqz  %s, %s\n", reg[cnt_reg], reg[cnt_reg]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
        else{
          tmpnum2 = value_st.top();
          value_st.pop();
          tmpnum1 = value_st.top();
          value_st.pop();
          printf("  slt   %s, %s, %s\n", reg[tmpnum2.num_val], reg[tmpnum1.num_val], reg[tmpnum2.num_val]);
          printf("  seqz  %s, %s\n", reg[tmpnum2.num_val], reg[tmpnum2.num_val]);
          tmpnum.num_val = tmpnum2.num_val;
          value_st.push(tmpnum);
        }
      }
      break;

    // Less than or equal to
    case KOOPA_RBO_LE:
      if (lhs->kind.tag == KOOPA_RVT_INTEGER){
        cnt_reg++;
        printf("  li    %s, ", reg[cnt_reg]);
        Visit_integer(lhs->kind.data.integer);
        std::cout << std::endl;
        if (rhs->kind.tag == KOOPA_RVT_INTEGER){
          cnt_reg++;
          printf("  li    %s, ", reg[cnt_reg]);
          Visit_integer(rhs->kind.data.integer);
          std::cout << std::endl;
          printf("  sgt   %s, %s, %s\n", reg[cnt_reg], reg[cnt_reg-1], reg[cnt_reg]);
          printf("  seqz  %s, %s\n", reg[cnt_reg], reg[cnt_reg]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
        else{
          tmpnum2 = value_st.top();
          value_st.pop();
          printf("  sgt   %s, %s, %s\n", reg[cnt_reg], reg[cnt_reg], reg[tmpnum2.num_val]);
          printf("  seqz  %s, %s\n", reg[cnt_reg], reg[cnt_reg]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
      }
      else{
        if (rhs->kind.tag == KOOPA_RVT_INTEGER){
          tmpnum1 = value_st.top();
          value_st.pop();
          cnt_reg++;
          printf("  li    %s, ", reg[cnt_reg]);
          Visit_integer(rhs->kind.data.integer);
          std::cout << std::endl;
          printf("  sgt   %s, %s, %s\n", reg[cnt_reg], reg[tmpnum1.num_val], reg[cnt_reg]);
          printf("  seqz  %s, %s\n", reg[cnt_reg], reg[cnt_reg]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
        else{
          tmpnum2 = value_st.top();
          value_st.pop();
          tmpnum1 = value_st.top();
          value_st.pop();
          printf("  sgt   %s, %s, %s\n", reg[tmpnum2.num_val], reg[tmpnum1.num_val], reg[tmpnum2.num_val]);
          printf("  seqz  %s, %s\n", reg[tmpnum2.num_val], reg[tmpnum2.num_val]);
          tmpnum.num_val = tmpnum2.num_val;
          value_st.push(tmpnum);
        }
      }
      break;

    // Addition
    case KOOPA_RBO_ADD:
      if (lhs->kind.tag == KOOPA_RVT_INTEGER){
        cnt_reg++;
        printf("  li    %s, ", reg[cnt_reg]);
        Visit_integer(lhs->kind.data.integer);
        std::cout << std::endl;
        if (rhs->kind.tag == KOOPA_RVT_INTEGER){
          cnt_reg++;
          printf("  li    %s, ", reg[cnt_reg]);
          Visit_integer(rhs->kind.data.integer);
          std::cout << std::endl;
          printf("  add   %s, %s, %s\n", reg[cnt_reg], reg[cnt_reg-1], reg[cnt_reg]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
        else{
          tmpnum2 = value_st.top();
          value_st.pop();
          printf("  add   %s, %s, %s\n", reg[cnt_reg], reg[cnt_reg], reg[tmpnum2.num_val]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
      }
      else{
        if (rhs->kind.tag == KOOPA_RVT_INTEGER){
          tmpnum1 = value_st.top();
          value_st.pop();
          cnt_reg++;
          printf("  li    %s, ", reg[cnt_reg]);
          Visit_integer(rhs->kind.data.integer);
          std::cout << std::endl;
          printf("  add   %s, %s, %s\n", reg[cnt_reg], reg[tmpnum1.num_val], reg[cnt_reg]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
        else{
          tmpnum2 = value_st.top();
          value_st.pop();
          tmpnum1 = value_st.top();
          value_st.pop();
          printf("  add   %s, %s, %s\n", reg[tmpnum2.num_val], reg[tmpnum1.num_val], reg[tmpnum2.num_val]);
          tmpnum.num_val = tmpnum2.num_val;
          value_st.push(tmpnum);
        }
      }
      break;

    // Subtraction
    case KOOPA_RBO_SUB:
      if (lhs->kind.tag == KOOPA_RVT_INTEGER){
        if (lhs->kind.data.integer.value == 0){
          if (rhs->kind.tag == KOOPA_RVT_INTEGER){
            cnt_reg++;
            printf("  li    %s, ", reg[cnt_reg]);
            Visit_integer(rhs->kind.data.integer);
            std::cout << std::endl;
            printf("  sub   %s, x0, %s\n", reg[cnt_reg], reg[cnt_reg]);
            tmpnum.num_val = cnt_reg;
            value_st.push(tmpnum);
          }
          else{
            tmpnum2 = value_st.top();
            value_st.pop();
            printf("  sub   %s, x0, %s\n", reg[tmpnum2.num_val], reg[tmpnum2.num_val]);
            tmpnum.num_val = tmpnum2.num_val;
            value_st.push(tmpnum);
          }
        }
        else{
          cnt_reg++;
          printf("  li    %s, ", reg[cnt_reg]);
          Visit_integer(lhs->kind.data.integer);
          std::cout << std::endl;
          if (rhs->kind.tag == KOOPA_RVT_INTEGER){
            cnt_reg++;
            printf("  li    %s, ", reg[cnt_reg]);
            Visit_integer(rhs->kind.data.integer);
            std::cout << std::endl;
            printf("  sub   %s, %s, %s\n", reg[cnt_reg], reg[cnt_reg-1], reg[cnt_reg]);
            tmpnum.num_val = cnt_reg;
            value_st.push(tmpnum);
          }
          else{
            tmpnum2 = value_st.top();
            value_st.pop();
            printf("  sub   %s, %s, %s\n", reg[cnt_reg], reg[cnt_reg], reg[tmpnum2.num_val]);
            tmpnum.num_val = cnt_reg;
            value_st.push(tmpnum);
          }
        }
      }
      else{
        if (rhs->kind.tag == KOOPA_RVT_INTEGER){
          tmpnum1 = value_st.top();
          value_st.pop();
          cnt_reg++;
          printf("  li    %s, ", reg[cnt_reg]);
          Visit_integer(rhs->kind.data.integer);
          std::cout << std::endl;
          printf("  sub   %s, %s, %s\n", reg[cnt_reg], reg[tmpnum1.num_val], reg[cnt_reg]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
        else{
          tmpnum2 = value_st.top();
          value_st.pop();
          tmpnum1 = value_st.top();
          value_st.pop();
          printf("  sub   %s, %s, %s\n", reg[tmpnum2.num_val], reg[tmpnum1.num_val], reg[tmpnum2.num_val]);
          tmpnum.num_val = tmpnum2.num_val;
          value_st.push(tmpnum);
        }
      }
      break;

    // Multiplication
    case KOOPA_RBO_MUL:
      if (lhs->kind.tag == KOOPA_RVT_INTEGER){
        cnt_reg++;
        printf("  li    %s, ", reg[cnt_reg]);
        Visit_integer(lhs->kind.data.integer);
        std::cout << std::endl;
        if (rhs->kind.tag == KOOPA_RVT_INTEGER){
          cnt_reg++;
          printf("  li    %s, ", reg[cnt_reg]);
          Visit_integer(rhs->kind.data.integer);
          std::cout << std::endl;
          printf("  mul   %s, %s, %s\n", reg[cnt_reg], reg[cnt_reg-1], reg[cnt_reg]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
        else{
          tmpnum2 = value_st.top();
          value_st.pop();
          printf("  mul   %s, %s, %s\n", reg[cnt_reg], reg[cnt_reg], reg[tmpnum2.num_val]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
      }
      else{
        if (rhs->kind.tag == KOOPA_RVT_INTEGER){
          tmpnum1 = value_st.top();
          value_st.pop();
          cnt_reg++;
          printf("  li    %s, ", reg[cnt_reg]);
          Visit_integer(rhs->kind.data.integer);
          std::cout << std::endl;
          printf("  mul   %s, %s, %s\n", reg[cnt_reg], reg[tmpnum1.num_val], reg[cnt_reg]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
        else{
          tmpnum2 = value_st.top();
          value_st.pop();
          tmpnum1 = value_st.top();
          value_st.pop();
          printf("  mul   %s, %s, %s\n", reg[tmpnum2.num_val], reg[tmpnum1.num_val], reg[tmpnum2.num_val]);
          tmpnum.num_val = tmpnum2.num_val;
          value_st.push(tmpnum);
        }
      }
      break;

    // Division
    case KOOPA_RBO_DIV:
      if (lhs->kind.tag == KOOPA_RVT_INTEGER){
        cnt_reg++;
        printf("  li    %s, ", reg[cnt_reg]);
        Visit_integer(lhs->kind.data.integer);
        std::cout << std::endl;
        if (rhs->kind.tag == KOOPA_RVT_INTEGER){
          cnt_reg++;
          printf("  li    %s, ", reg[cnt_reg]);
          Visit_integer(rhs->kind.data.integer);
          std::cout << std::endl;
          printf("  div   %s, %s, %s\n", reg[cnt_reg], reg[cnt_reg-1], reg[cnt_reg]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
        else{
          tmpnum2 = value_st.top();
          value_st.pop();
          printf("  div   %s, %s, %s\n", reg[cnt_reg], reg[cnt_reg], reg[tmpnum2.num_val]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
      }
      else{
        if (rhs->kind.tag == KOOPA_RVT_INTEGER){
          tmpnum1 = value_st.top();
          value_st.pop();
          cnt_reg++;
          printf("  li    %s, ", reg[cnt_reg]);
          Visit_integer(rhs->kind.data.integer);
          std::cout << std::endl;
          printf("  div   %s, %s, %s\n", reg[cnt_reg], reg[tmpnum1.num_val], reg[cnt_reg]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
        else{
          tmpnum2 = value_st.top();
          value_st.pop();
          tmpnum1 = value_st.top();
          value_st.pop();
          printf("  div   %s, %s, %s\n", reg[tmpnum2.num_val], reg[tmpnum1.num_val], reg[tmpnum2.num_val]);
          tmpnum.num_val = tmpnum2.num_val;
          value_st.push(tmpnum);
        }
      }
      break;

    // Modulo
    case KOOPA_RBO_MOD:
      if (lhs->kind.tag == KOOPA_RVT_INTEGER){
        cnt_reg++;
        printf("  li    %s, ", reg[cnt_reg]);
        Visit_integer(lhs->kind.data.integer);
        std::cout << std::endl;
        if (rhs->kind.tag == KOOPA_RVT_INTEGER){
          cnt_reg++;
          printf("  li    %s, ", reg[cnt_reg]);
          Visit_integer(rhs->kind.data.integer);
          std::cout << std::endl;
          printf("  rem   %s, %s, %s\n", reg[cnt_reg], reg[cnt_reg-1], reg[cnt_reg]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
        else{
          tmpnum2 = value_st.top();
          value_st.pop();
          printf("  rem   %s, %s, %s\n", reg[cnt_reg], reg[cnt_reg], reg[tmpnum2.num_val]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
      }
      else{
        if (rhs->kind.tag == KOOPA_RVT_INTEGER){
          tmpnum1 = value_st.top();
          value_st.pop();
          cnt_reg++;
          printf("  li    %s, ", reg[cnt_reg]);
          Visit_integer(rhs->kind.data.integer);
          std::cout << std::endl;
          printf("  rem   %s, %s, %s\n", reg[cnt_reg], reg[tmpnum1.num_val], reg[cnt_reg]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
        else{
          tmpnum2 = value_st.top();
          value_st.pop();
          tmpnum1 = value_st.top();
          value_st.pop();
          printf("  rem   %s, %s, %s\n", reg[tmpnum2.num_val], reg[tmpnum1.num_val], reg[tmpnum2.num_val]);
          tmpnum.num_val = tmpnum2.num_val;
          value_st.push(tmpnum);
        }
      }
      break;

    // Bitwise AND
    case KOOPA_RBO_AND:
      if (lhs->kind.tag == KOOPA_RVT_INTEGER){
        cnt_reg++;
        printf("  li    %s, ", reg[cnt_reg]);
        Visit_integer(lhs->kind.data.integer);
        std::cout << std::endl;
        if (rhs->kind.tag == KOOPA_RVT_INTEGER){
          cnt_reg++;
          printf("  li    %s, ", reg[cnt_reg]);
          Visit_integer(rhs->kind.data.integer);
          std::cout << std::endl;
          printf("  and   %s, %s, %s\n", reg[cnt_reg], reg[cnt_reg-1], reg[cnt_reg]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
        else{
          tmpnum2 = value_st.top();
          value_st.pop();
          printf("  and   %s, %s, %s\n", reg[cnt_reg], reg[cnt_reg], reg[tmpnum2.num_val]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
      }
      else{
        if (rhs->kind.tag == KOOPA_RVT_INTEGER){
          tmpnum1 = value_st.top();
          value_st.pop();
          cnt_reg++;
          printf("  li    %s, ", reg[cnt_reg]);
          Visit_integer(rhs->kind.data.integer);
          std::cout << std::endl;
          printf("  and   %s, %s, %s\n", reg[cnt_reg], reg[tmpnum1.num_val], reg[cnt_reg]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
        else{
          tmpnum2 = value_st.top();
          value_st.pop();
          tmpnum1 = value_st.top();
          value_st.pop();
          printf("  and   %s, %s, %s\n", reg[tmpnum2.num_val], reg[tmpnum1.num_val], reg[tmpnum2.num_val]);
          tmpnum.num_val = tmpnum2.num_val;
          value_st.push(tmpnum);
        }
      }
      break;

    // Bitwise OR
    case KOOPA_RBO_OR:
      if (lhs->kind.tag == KOOPA_RVT_INTEGER){
        cnt_reg++;
        printf("  li    %s, ", reg[cnt_reg]);
        Visit_integer(lhs->kind.data.integer);
        std::cout << std::endl;
        if (rhs->kind.tag == KOOPA_RVT_INTEGER){
          cnt_reg++;
          printf("  li    %s, ", reg[cnt_reg]);
          Visit_integer(rhs->kind.data.integer);
          std::cout << std::endl;
          printf("  or    %s, %s, %s\n", reg[cnt_reg], reg[cnt_reg-1], reg[cnt_reg]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
        else{
          tmpnum2 = value_st.top();
          value_st.pop();
          printf("  or    %s, %s, %s\n", reg[cnt_reg], reg[cnt_reg], reg[tmpnum2.num_val]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
      }
      else{
        if (rhs->kind.tag == KOOPA_RVT_INTEGER){
          tmpnum1 = value_st.top();
          value_st.pop();
          cnt_reg++;
          printf("  li    %s, ", reg[cnt_reg]);
          Visit_integer(rhs->kind.data.integer);
          std::cout << std::endl;
          printf("  or    %s, %s, %s\n", reg[cnt_reg], reg[tmpnum1.num_val], reg[cnt_reg]);
          tmpnum.num_val = cnt_reg;
          value_st.push(tmpnum);
        }
        else{
          tmpnum2 = value_st.top();
          value_st.pop();
          tmpnum1 = value_st.top();
          value_st.pop();
          printf("  or    %s, %s, %s\n", reg[tmpnum2.num_val], reg[tmpnum1.num_val], reg[tmpnum2.num_val]);
          tmpnum.num_val = tmpnum2.num_val;
          value_st.push(tmpnum);
        }
      }
      break;

    default:
      assert(false);
      break;
  }
}

// 访问 alloc 指令
void Visit_alloc(const koopa_raw_global_alloc_t &alloc){
  koopa_raw_value_t init = alloc.init;
  // std::cout << "alloc tag = " << init->kind.tag << std::endl;
  switch (init->kind.tag){
    case KOOPA_RVT_INTEGER:
      break;
    default:
      break;
  }
}

// 访问 global alloc 指令
void Visit_global_alloc(const koopa_raw_global_alloc_t &global_alloc){

}

// 访问 load 指令
void Visit_load(const koopa_raw_load_t &load){
  koopa_raw_value_t src = load.src;
  // std::cout << "tag = " << src->kind.tag << std::endl;
  switch (src->kind.tag){
    case KOOPA_RVT_ALLOC:
      break;
    default:
      break;
  }
}

// 访问 store 指令
void Visit_store(const koopa_raw_store_t &store){
  koopa_raw_value_t value = store.value;
  // std::cout << "value tag = " << value->kind.tag << std::endl;
  // koopa_raw_value_t dest = store.dest;
  // std::cout << "dest data = " << dest->kind.data.global_alloc.init->kind.tag << std::endl;
  switch (value->kind.tag){
    // 数字
    case KOOPA_RVT_INTEGER:
      cnt_reg++;
      printf("  li    %s, ", reg[cnt_reg]);
      Visit_integer(value->kind.data.integer);
      printf("\n");
      printf("  sw    %s, %d(sp)\n", reg[cnt_reg], 444);
      break;
    // 运算结果
    case KOOPA_RVT_BINARY:

    default:
      assert(false);
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
  switch (ret_value->kind.tag) {
    // 直接返回数字
    case KOOPA_RVT_INTEGER:
      printf("  li    a0, ");
      Visit_integer(ret_value->kind.data.integer);
      printf("\n");
      printf("  addi    sp, sp, %d\n", sum_stack*4);
      printf("  ret\n");
      break;

    // 返回运算结果
    case KOOPA_RVT_BINARY:
      tmp_num = value_st.top();
      value_st.pop();
      printf("  mv    a0, %s\n", reg[tmp_num.num_val]);
      printf("  addi    sp, sp, %d\n", sum_stack*4);
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

  printf("  addi   sp, sp, -%d\n\n", sum_stack*4);

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