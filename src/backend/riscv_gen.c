#include "riscv_gen.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// 简单的寄存器分配，为临时变量按顺序分配 t0, t1
// t2, t3 作为运算时的临时寄存器
static const char* temp_regs[] = {"t0", "t1"};
static int reg_counter = 0;

// 存储值到寄存器的映射
#define MAX_VALUES 1000
static koopa_raw_value_t value_map[MAX_VALUES];
static int value_reg_map[MAX_VALUES];
static int value_count = 0;

// 访问指令
static void visit_value(FILE *output, koopa_raw_value_t value);

// 获取值对应的寄存器索引，不存在则分配新的
static int get_value_reg(koopa_raw_value_t value) {
    for (int i = 0; i < value_count; i++) {
        if (value_map[i] == value) {
            return value_reg_map[i];
        }
    }
    // 分配新寄存器
    int reg_idx = reg_counter % 2;
    value_map[value_count] = value;
    value_reg_map[value_count] = reg_idx;
    value_count++;
    reg_counter++;
    return reg_idx;
}

// 加载值到指定寄存器
static void load_value_to_reg(FILE *output, koopa_raw_value_t value, const char* reg) {
    if (value->kind.tag == KOOPA_RVT_INTEGER) {
        fprintf(output, "  li %s, %d\n", reg, value->kind.data.integer.value);
    } else {
        int val_reg = get_value_reg(value);
        const char* val_reg_name = temp_regs[val_reg];
        if (strcmp(val_reg_name, reg) != 0) {
            fprintf(output, "  mv %s, %s\n", reg, val_reg_name);
        }
    }
}

// 访问 return 指令
static void visit_return(FILE *output, koopa_raw_return_t ret) {
    koopa_raw_value_t ret_value = ret.value;
    if (ret_value != NULL) {
        if (ret_value->kind.tag == KOOPA_RVT_INTEGER) {
            // 返回整数常量
            fprintf(output, "  li a0, %d\n", ret_value->kind.data.integer.value);
        } else {
            // 返回临时变量
            int ret_reg = get_value_reg(ret_value);
            fprintf(output, "  mv a0, %s\n", temp_regs[ret_reg]);
        }
    }
    fprintf(output, "  ret\n");
}

// 访问二元运算指令
static void visit_binary(FILE *output, koopa_raw_value_t value, koopa_raw_binary_t binary) {
    koopa_raw_value_t lhs = binary.lhs;
    koopa_raw_value_t rhs = binary.rhs;
    
    // 分配结果至寄存器
    int result_reg = get_value_reg(value);
    const char* target_reg = temp_regs[result_reg];
    
    // 分别载入左右操作数
    load_value_to_reg(output, lhs, "t2");
    load_value_to_reg(output, rhs, "t3");
    
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
        case KOOPA_RBO_LT:
            fprintf(output, "  slt %s, t2, t3\n", target_reg);
            break;
        case KOOPA_RBO_GT:
            fprintf(output, "  sgt %s, t2, t3\n", target_reg);
            break;
        case KOOPA_RBO_LE:
            fprintf(output, "  sgt %s, t2, t3\n", target_reg);
            fprintf(output, "  seqz %s, %s\n", target_reg, target_reg);
            break;
        case KOOPA_RBO_GE:
            fprintf(output, "  slt %s, t2, t3\n", target_reg);
            fprintf(output, "  seqz %s, %s\n", target_reg, target_reg);
            break;
        case KOOPA_RBO_EQ:
            fprintf(output, "  xor %s, t2, t3\n", target_reg);
            fprintf(output, "  seqz %s, %s\n", target_reg, target_reg);
            break;
        case KOOPA_RBO_NOT_EQ:
            fprintf(output, "  xor %s, t2, t3\n", target_reg);
            fprintf(output, "  snez %s, %s\n", target_reg, target_reg);
            break;
        case KOOPA_RBO_AND:
            fprintf(output, "  and %s, t2, t3\n", target_reg);
            break;
        case KOOPA_RBO_OR:
            fprintf(output, "  or %s, t2, t3\n", target_reg);
            break;
        default:
            assert(false && "Unsupported binary operation");
    }
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
  // 重置寄存器
  reg_counter = 0;
  value_count = 0;
  
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