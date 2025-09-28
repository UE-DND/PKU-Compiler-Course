#include "riscv_gen.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// 简单的寄存器分配，为临时变量按顺序分配 t0, t1
// t2, t3 作为运算时的临时寄存器
static const char* temp_regs[] = {"t0", "t1"};
static int reg_counter = 0;

// 访问指令
static void visit_value(FILE *output, koopa_raw_value_t value);

// 访问 return 指令
static void visit_return(FILE *output, koopa_raw_return_t ret) {
    koopa_raw_value_t ret_value = ret.value;
    if (ret_value != NULL) {
        if (ret_value->kind.tag == KOOPA_RVT_INTEGER) {
            // 返回整数常量
            fprintf(output, "  li a0, %d\n", ret_value->kind.data.integer.value);
        } else {
            // 返回临时变量，应该在最后使用的寄存器中
            const char* last_reg = temp_regs[(reg_counter - 1) % 2];
            fprintf(output, "  mv a0, %s\n", last_reg);
        }
    }
    fprintf(output, "  ret\n");
}

// 访问二元运算指令
static void visit_binary(FILE *output, koopa_raw_value_t value, koopa_raw_binary_t binary) {
    koopa_raw_value_t lhs = binary.lhs;
    koopa_raw_value_t rhs = binary.rhs;
    
    // 目标寄存器，为当前指令分配一个新的临时寄存器
    const char* target_reg = temp_regs[reg_counter % 2];
    
    // 处理左操作数
    if (lhs->kind.tag == KOOPA_RVT_INTEGER) {
        // 左操作数是立即数，加载到一个不会冲突的寄存器
        // 使用 t2 作为临时寄存器来避免覆盖之前的结果
        fprintf(output, "  li t2, %d\n", lhs->kind.data.integer.value);
    } else {
        // 左操作数是之前计算的结果，应该在前一个临时寄存器中
        const char* lhs_reg = temp_regs[(reg_counter - 1) % 2];
        // 把左操作数移动到 t2
        fprintf(output, "  mv t2, %s\n", lhs_reg);
    }
    
    // 处理右操作数  
    if (rhs->kind.tag == KOOPA_RVT_INTEGER) {
        // 右操作数是立即数，加载到 t3
        fprintf(output, "  li t3, %d\n", rhs->kind.data.integer.value);
    } else {
        // 右操作数是之前计算的结果
        const char* rhs_reg = temp_regs[(reg_counter - 1) % 2];
        // 把右操作数移动到 t3
        fprintf(output, "  mv t3, %s\n", rhs_reg);
    }
    
    // 执行运算，使用 t2 和 t3 作为操作数，结果存储到目标寄存器
    switch (binary.op) {
        case KOOPA_RBO_ADD:
            fprintf(output, "  add %s, t2, t3\n", target_reg);
            break;
        case KOOPA_RBO_SUB:
            fprintf(output, "  sub %s, t2, t3\n", target_reg);
            break;
        case KOOPA_RBO_MUL:
            fprintf(output, "  mul %s, t2, t3\n", target_reg);
            break;
        case KOOPA_RBO_DIV:
            fprintf(output, "  div %s, t2, t3\n", target_reg);
            break;
        case KOOPA_RBO_MOD:
            fprintf(output, "  rem %s, t2, t3\n", target_reg);
            break;
        default:
            assert(false && "Unsupported binary operation");
    }
    
    reg_counter++;
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
        case KOOPA_RVT_BINARY:
            visit_binary(output, value, kind.data.binary);
            break;
        default:
            assert(false && "Unsupported value type");
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