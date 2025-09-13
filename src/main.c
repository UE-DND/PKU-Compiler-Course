#include <assert.h>                               // 断言库，用于终止程序
#include <stdio.h>                                // 文件操作和输出
#include <stdlib.h>                               // 内存管理
#include <string.h>                               // 字符串操作
#include "ast.h"
#include "codegen.h"                              // IR代码生成器

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
  assert(!ret);

  // 根据模式输出不同内容
  if (strcmp(mode, "-koopa") == 0) {
    // 使用独立的IR生成器输出Koopa IR
    generate_koopa_ir(ast);
  } else {
    // 默认输出AST结构（调试用）
    dump_ast(ast);
    printf("\n");
  }

  destroy_ast(ast);
  
  return 0;
}
