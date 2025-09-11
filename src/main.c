#include <assert.h>                               // 断言库，用于终止程序
#include <stdio.h>                                // 文件操作和输出
#include <stdlib.h>                               // 内存管理
#include "ast.h"

extern FILE *yyin;                                // Flex生成的全局指针，用于指向输入文本
extern int yyparse(BaseAST **ast);                // Bison生成的全局指针，用于指向解析结果

int main(int argc, const char *argv[]) {
  assert(argc == 2);                              // 检查命令行参数必须为2
  const char *input = argv[1];                    // 参数1: 输入文件

  yyin = fopen(input, "r");                       // 打开输入文件
  assert(yyin);                                   // 断言用于检测打开有效性，失败则终止

  BaseAST *ast = NULL;
  int ret = yyparse(&ast);                        // yyparse解析成功返回0
  assert(!ret);

  dump_ast(ast);
  printf("\n");

  destroy_ast(ast);
  
  return 0;
}
