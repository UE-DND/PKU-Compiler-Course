#include <assert.h>                               // 断言库，用于终止程序
#include <stdio.h>                                // 文件操作和输出
#include <stdlib.h>                               // 内存管理
#include <string.h>                               // 字符串操作
#include "ast.h"
#include "codegen.h"                              // IR代码生成器
#include "koopa_ir.h"

extern FILE *yyin;                                // Flex生成的全局指针，指向输入文本
extern int yyparse(BaseAST **ast);                // Bison生成的全局指针，指向解析结果

int main(int argc, const char *argv[]) {
  assert(argc == 5);                              // 检查命令行参数必须为5个
  const char *mode = argv[1];                     // 模式 (-koopa)
  const char *input = argv[2];                    // 输入文件
  const char *output = argv[4];                   // 输出文件

  yyin = fopen(input, "r");                       // 打开输入文件
  assert(yyin);                                   // 断言用于检测打开有效性，失败则终止

  BaseAST *ast = NULL;
  int ret = yyparse(&ast);                        // yyparse解析成功返回0
  if (ret) {
    fprintf(stderr, "Parse error\n");
    return 1;
  }

  // 根据模式输出不同内容
  if (strcmp(mode, "-koopa") == 0) {
    // 生成 Koopa IR 到内存字符串
    char *ir_buf = NULL;
    size_t ir_size = 0;
    FILE *mem = open_memstream(&ir_buf, &ir_size);
    if (!mem) {
      fprintf(stderr, "Failed to open memstream\n");
      destroy_ast(ast);
      return 1;
    }
    CodeGenerator gen_mem;
    codegen_init(&gen_mem, mem);
    codegen_program(&gen_mem, ast);
    fclose(mem);

    // 将 IR 写入目标输出文件
    FILE *output_file = fopen(output, "w");
    if (!output_file) {
      fprintf(stderr, "Failed to open output file: %s\n", output);
      free(ir_buf);
      destroy_ast(ast);
      return 1;
    }
    fwrite(ir_buf, 1, ir_size, output_file);
    fclose(output_file);

    // 使用 libkoopa 解析文本 IR 为 raw program
    koopa_raw_program_builder_t builder;
    koopa_raw_program_t raw;
    if (koopa_parse_ir_from_string(ir_buf, &builder, &raw) == 0) {
      koopa_dump_functions_to_stderr(raw);
      koopa_delete_raw_program_builder(builder);
    } else {
      fprintf(stderr, "Failed to parse Koopa IR\n");
    }
    free(ir_buf);
  } else {
    dump_ast(ast);
    printf("\n");
  }

  destroy_ast(ast);
  
  return 0;
}
