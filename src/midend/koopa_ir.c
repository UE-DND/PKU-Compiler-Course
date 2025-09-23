#include "koopa_ir.h"
#include <assert.h>
#include <string.h>

// 从字符串解析 Koopa IR 代码并构建原始程序表示
int parse_ir_from_string(const char *ir_text, koopa_raw_program_builder_t *out_builder, koopa_raw_program_t *out_raw) {
  // 检查参数有效性
  if (!ir_text || !out_builder || !out_raw) return -1;

  // 解析 IR 字符串为程序对象
  koopa_program_t program;
  koopa_error_code_t ret = koopa_parse_from_string(ir_text, &program);
  if (ret != KOOPA_EC_SUCCESS) {
    return -2;
  }
  
  koopa_raw_program_builder_t builder = koopa_new_raw_program_builder();  // 创建原始程序构建器
  
  koopa_raw_program_t raw = koopa_build_raw_program(builder, program);    // 构建原始程序表示
  
  koopa_delete_program(program);                                          // 清理程序对象

  // 返回构建结果
  *out_builder = builder;
  *out_raw = raw;
  return 0;
}

// 将程序中的所有函数名称输出到标准错误流
void dump_functions_to_stderr(koopa_raw_program_t raw) {

  const koopa_raw_slice_t funcs = raw.funcs;                          // 获取程序中的函数列表
  
  // 遍历所有函数
  for (size_t i = 0; i < funcs.len; ++i) {
    const koopa_raw_function_t f = (const koopa_raw_function_t)funcs.buffer[i];  // 获取当前函数
    if (!f || !f->name) continue;                                     // 检查函数和函数名是否有效
    fprintf(stderr, "[koopa] func: %s\n", f->name);                   // 输出函数名到标准错误流
  }
}
