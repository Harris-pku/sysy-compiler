#pragma once
#include <iostream>
#include <stack>
#include <vector>

struct num_t{
  // valid = 1 => 数值
  // valid = 0 => 寄存器号
  int num_val, valid;
};

struct sym_t{
  // type = 0 => 常量
  // type = 1 => 变量
  // type = 2 => int 函数
  // tpye = 3 => void 函数

  // type = 5 => 参数变量
  // type = 6 => 数组
  int val_t, type, number;
  std::vector<int> array;
};