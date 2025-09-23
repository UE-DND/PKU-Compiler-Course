#include "riscv_gen.h"
#include <assert.h>
#include <stdbool.h>

// 访问指令
static void visit_value(FILE *output, koopa_raw_value_t value);

// 访问 return 指令
static void visit_return(FILE *output, koopa_raw_return_t ret) {
  koopa_raw_value_t ret_value = ret.value;
  if (ret_value != NULL) {
    // 处理返回值
    koopa_raw_value_kind_t kind = ret_value->kind;
    if (kind.tag == KOOPA_RVT_INTEGER) {
      // 返回整数常量
      int32_t int_val = kind.data.integer.value;
      fprintf(output, "  li a0, %d\n", int_val);
    }
  }
  fprintf(output, "  ret\n");
}

// 访问指令
static void visit_value(FILE *output, koopa_raw_value_t value) {
  koopa_raw_value_kind_t kind = value->kind;
  switch (kind.tag) {
    case KOOPA_RVT_RETURN:
      visit_return(output, kind.data.ret);
      break;
    case KOOPA_RVT_INTEGER:
      // 整数值不需要单独处理，在使用时处理
      break;
    default:
      // 其他类型暂时遇不到
      assert(false);
  }
}

// 访问基本块
static void visit_basic_block(FILE *output, koopa_raw_basic_block_t bb) {
  // 访问所有指令
  for (size_t i = 0; i < bb->insts.len; ++i) {
    koopa_raw_value_t value = (koopa_raw_value_t) bb->insts.buffer[i];
    visit_value(output, value);
  }
}

// 访问函数
static void visit_function(FILE *output, koopa_raw_function_t func) {
  // 函数名去掉 @ 前缀
  const char *func_name = func->name + 1;
  
  // 生成函数标签和全局声明
  fprintf(output, "  .text\n");
  fprintf(output, "  .globl %s\n", func_name);
  fprintf(output, "%s:\n", func_name);
  
  // 访问所有基本块
  for (size_t i = 0; i < func->bbs.len; ++i) {
    koopa_raw_basic_block_t bb = (koopa_raw_basic_block_t) func->bbs.buffer[i];
    visit_basic_block(output, bb);
  }
}

// 从 raw program 生成 RISC-V 汇编代码
void generate_riscv_from_raw_program(FILE *output, koopa_raw_program_t raw) {
  // 访问所有函数
  for (size_t i = 0; i < raw.funcs.len; ++i) {
    koopa_raw_function_t func = (koopa_raw_function_t) raw.funcs.buffer[i];
    visit_function(output, func);
  }
}