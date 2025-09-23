#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "ast.h"
#include "codegen.h"
#include "koopa_ir.h"
#include "riscv_gen.h"

extern FILE *yyin;                                // Flex生成的全局指针，指向输入文本
extern int yyparse(BaseAST **ast);                // Bison生成的全局指针，指向解析结果

// 生成Koopa IR到字符串
static char* generate_ir_to_string(BaseAST *ast, size_t *ir_size) {
  char temp_filename[256];
  snprintf(temp_filename, sizeof(temp_filename), "/tmp/koopa_ir_%ld.tmp", time(NULL));
  
  FILE *temp_file = fopen(temp_filename, "w+");
  if (!temp_file) {
    fprintf(stderr, "Failed to create temp file: %s\n", temp_filename);
    return NULL;
  }
  
  // 生成IR到临时文件
  CodeGenerator gen_mem;
  codegen_program(&gen_mem, temp_file, ast);
  
  // 获取文件大小
  fseek(temp_file, 0, SEEK_END);
  long file_size = ftell(temp_file);
  fseek(temp_file, 0, SEEK_SET);
  
  if (file_size < 0) {
    fprintf(stderr, "Failed to get file size\n");
    fclose(temp_file);
    unlink(temp_filename);
    return NULL;
  }
  
  // 分配内存并读取文件内容
  char *ir_buf = malloc(file_size + 1);
  if (!ir_buf) {
    fprintf(stderr, "Failed to allocate memory\n");
    fclose(temp_file);
    unlink(temp_filename);
    return NULL;
  }
  
  size_t read_size = fread(ir_buf, 1, file_size, temp_file);
  ir_buf[read_size] = '\0';
  *ir_size = read_size;
  
  // 销毁临时文件
  fclose(temp_file);
  unlink(temp_filename);
  
  return ir_buf;
}

// 将字符串内容写入文件
static int write_to_file(const char *filename, const char *content, size_t size) {
  FILE *output_file = fopen(filename, "w");
  if (!output_file) {
    fprintf(stderr, "Failed to open output file: %s\n", filename);
    return -1;
  }
  
  fwrite(content, 1, size, output_file);
  fclose(output_file);
  return 0;
}

// 解析Koopa IR为raw program
static int parse_ir_to_raw_program(const char *ir_buf, koopa_raw_program_builder_t *builder, koopa_raw_program_t *raw) {
  if (parse_ir_from_string(ir_buf, builder, raw) != 0) {
    fprintf(stderr, "Failed to parse Koopa IR\n");
    return -1;
  }
  return 0;
}

int main(int argc, const char *argv[]) {
  assert(argc == 5);
  const char *mode = argv[1];
  const char *input = argv[2];
  const char *output = argv[4];

  yyin = fopen(input, "r");                       // 打开输入文件
  assert(yyin);                                   // 断言用于检测打开有效性，失败则终止

  BaseAST *ast = NULL;
  int ret = yyparse(&ast);                        // yyparse解析成功返回0
  if (ret) {
    fprintf(stderr, "Parse error\n");
    return 1;
  }

  if (strcmp(mode, "-koopa") == 0) {
    // 生成 Koopa IR
    size_t ir_size = 0;
    char *ir_buf = generate_ir_to_string(ast, &ir_size);
    if (!ir_buf) {
      destroy_ast(ast);
      return 1;
    }

    // 将 IR 写入目标输出文件
    if (write_to_file(output, ir_buf, ir_size) != 0) {
      free(ir_buf);
      destroy_ast(ast);
      return 1;
    }
  } else if (strcmp(mode, "-riscv") == 0) {
    // 生成 Koopa IR
    size_t ir_size = 0;
    char *ir_buf = generate_ir_to_string(ast, &ir_size);
    if (!ir_buf) {
      destroy_ast(ast);
      return 1;
    }

    // 解析 Koopa IR 为 raw program
    koopa_raw_program_builder_t builder;
    koopa_raw_program_t raw;
    if (parse_ir_to_raw_program(ir_buf, &builder, &raw) != 0) {
      free(ir_buf);
      destroy_ast(ast);
      return 1;
    }

    // 生成 RISC-V 汇编代码到文件
    FILE *output_file = fopen(output, "w");
    if (!output_file) {
      fprintf(stderr, "Failed to open output file: %s\n", output);
      koopa_delete_raw_program_builder(builder);
      free(ir_buf);
      destroy_ast(ast);
      return 1;
    }
    
    generate_riscv_from_raw_program(output_file, raw);
    fclose(output_file);
    
    koopa_delete_raw_program_builder(builder);
    free(ir_buf);
  } else if (strcmp(mode, "-ast") == 0){
    dump_ast(ast);
    printf("\n");
    destroy_ast(ast);
  } else {
    printf("Unknown command\n");
  }
  
  return 0;
}
