#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
extern FILE *yyin;
extern int yyparse(char **ast);

int main(int argc, const char *argv[]) {
  assert(argc == 5);
  const char *mode = argv[1];
  const char *input = argv[2];
  const char *output = argv[4];

  yyin = fopen(input, "r");
  assert(yyin);

  char *ast = NULL;
  int ret = yyparse(&ast);
  assert(!ret);

  printf("%s\n", ast);
  
  if (ast) {
    free(ast);
  }
  
  return 0;
}
