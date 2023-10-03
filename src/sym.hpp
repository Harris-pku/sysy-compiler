#pragma once
#include <iostream>

struct num_t{
  // valid = 1 => 数值
  // valid = 0 => 寄存器号
  int num_val, valid;
};

struct sym_t{
  // type = 0 => 常量
  // type = 1 => 变量
  int val_t, type;
};