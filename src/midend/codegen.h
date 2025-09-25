#pragma once

#include "ast.h"
#include <stdio.h>

typedef struct {
    FILE *output;           // 输出流
    int indent_level;       // 当前缩进
} CodeGenerator;

/**
 * 生成完整Koopa IR
 * @param gen 代码生成器实例
 * @param output 输出流
 * @param ast 程序的根AST节点
 */
void codegen_program(CodeGenerator *gen, FILE *output, const BaseAST *ast);

// ========================================
// 各AST节点类型的代码生成函数
// ========================================

/**
 * 生成编译单元IR
 * @param gen 代码生成器实例
 * @param ast CompUnitAST节点
 */
void codegen_comp_unit(CodeGenerator *gen, const CompUnitAST *ast);

/**
 * 生成函数定义IR
 * @param gen 代码生成器实例
 * @param ast FuncDefAST节点
 */
void codegen_func_def(CodeGenerator *gen, const FuncDefAST *ast);

/**
 * 生成函数类型IR
 * @param gen 代码生成器实例
 * @param ast FuncTypeAST节点
 */
void codegen_func_type(CodeGenerator *gen, const FuncTypeAST *ast);

/**
 * 生成代码块IR
 * @param gen 代码生成器实例
 * @param ast BlockAST节点
 */
void codegen_block(CodeGenerator *gen, const BlockAST *ast);

/**
 * 生成语句IR
 * @param gen 代码生成器实例
 * @param ast StmtAST节点
 */
void codegen_stmt(CodeGenerator *gen, const StmtAST *ast);

// 计算表达式常量值
int eval_const_expr(const BaseAST *expr, int *out);

/**
 * 简化的代码生成接口，直接输出到stdout
 * @param ast 要生成IR的AST根节点
 */
void generate_koopa_ir(const BaseAST *ast);