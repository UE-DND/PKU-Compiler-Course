#include "codegen.h"
#include <assert.h>

void codegen_init(CodeGenerator *gen, FILE *output) {
    assert(gen);
    assert(output);
    
    gen->output = output;
    gen->indent_level = 0;
}

void codegen_program(CodeGenerator *gen, const BaseAST *ast) {
    assert(gen);
    assert(ast);
    assert(ast->type == AST_COMP_UNIT);
    
    codegen_comp_unit(gen, (const CompUnitAST *)ast);
}

void codegen_comp_unit(CodeGenerator *gen, const CompUnitAST *ast) {
    assert(gen);
    assert(ast);
    assert(ast->func_def);
    assert(ast->func_def->type == AST_FUNC_DEF);

    codegen_func_def(gen, (const FuncDefAST *)ast->func_def);
}

void codegen_func_def(CodeGenerator *gen, const FuncDefAST *ast) {
    assert(gen);
    assert(ast);
    assert(ast->ident);
    assert(ast->block);
    assert(ast->block->type == AST_BLOCK);
    
    // 输出函数签名：fun @main(): i32 {
    fprintf(gen->output, "fun @%s(): i32 {\n", ast->ident);
    
    // 输出入口基本块标签
    fprintf(gen->output, "%%entry:\n");
    
    // 生成函数体
    gen->indent_level++;
    codegen_block(gen, (const BlockAST *)ast->block);
    gen->indent_level--;

    fprintf(gen->output, "}\n");
}

void codegen_func_type(CodeGenerator *gen, const FuncTypeAST *ast) {
    // 目前函数类型在函数定义中已经处理，预留使用
}

void codegen_block(CodeGenerator *gen, const BlockAST *ast) {
    assert(gen);
    assert(ast);
    assert(ast->stmt);
    assert(ast->stmt->type == AST_STMT);

    codegen_stmt(gen, (const StmtAST *)ast->stmt);
}

void codegen_stmt(CodeGenerator *gen, const StmtAST *ast) {
    assert(gen);
    assert(ast);
    assert(ast->number);
    assert(ast->number->type == AST_NUMBER);

    for (int i = 0; i < gen->indent_level; i++) {
        fprintf(gen->output, "  ");
    }
    
    fprintf(gen->output, "ret ");
    codegen_number(gen, (const NumberAST *)ast->number);
    fprintf(gen->output, "\n");
}

void codegen_number(CodeGenerator *gen, const NumberAST *ast) {
    assert(gen);
    assert(ast);
    
    fprintf(gen->output, "%d", ast->value);
}

void generate_koopa_ir(const BaseAST *ast) {
    CodeGenerator gen;
    codegen_init(&gen, stdout);
    codegen_program(&gen, ast);
}