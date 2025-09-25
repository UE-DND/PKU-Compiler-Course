#include "codegen.h"
#include <assert.h>

void codegen_program(CodeGenerator *gen, FILE *output, const BaseAST *ast) {
    assert(gen);
    assert(output);
    assert(ast);
    assert(ast->type == AST_COMP_UNIT);
    
    // 初始化代码生成器
    gen->output = output;
    gen->indent_level = 0;
    
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
    assert(ast->expr);

    for (int i = 0; i < gen->indent_level; i++) {
        fprintf(gen->output, "  ");
    }
    
    int val = 0;
    int ok = eval_const_expr(ast->expr, &val);
    assert(ok && "Only constant expressions supported in lv3.1");
    fprintf(gen->output, "ret %d\n", val);
}

void generate_koopa_ir(const BaseAST *ast) {
    CodeGenerator gen;
    codegen_program(&gen, stdout, ast);
}

int eval_const_expr(const BaseAST *expr, int *out) {
    if (!expr || !out) return 0;
    switch (expr->type) {
        case AST_NUMBER: {
            const NumberAST *n = (const NumberAST *)expr;
            *out = n->value;
            return 1;
        }
        case AST_UNARY: {
            const UnaryAST *u = (const UnaryAST *)expr;
            int v = 0;
            if (!eval_const_expr(u->operand, &v)) return 0;
            switch (u->op) {
                case '+': *out = +v; return 1;
                case '-': *out = -v; return 1;
                case '!': *out = (v == 0); return 1; // C 语义：真为1，假为0
                default: return 0;
            }
        }
        default:
            return 0;
    }
}