#include "codegen.h"
#include <assert.h>
#include <string.h>

void codegen_program(CodeGenerator *gen, FILE *output, const BaseAST *ast) {
    assert(gen);
    assert(output);
    assert(ast);
    assert(ast->type == AST_COMP_UNIT);
    
    // 初始化代码生成器
    gen->output = output;
    gen->indent_level = 0;
    gen->temp_counter = 0;
    
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
    
    // 重置临时变量计数器
    gen->temp_counter = 0;
    
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

    // 生成表达式的IR代码
    char *result = codegen_expr(gen, ast->expr);
    
    // 生成return语句
    for (int i = 0; i < gen->indent_level; i++) {
        fprintf(gen->output, "  ");
    }
    fprintf(gen->output, "ret %s\n", result);
    
    free(result);
}

char* codegen_expr(CodeGenerator *gen, const BaseAST *expr) {
    assert(gen);
    assert(expr);
    
    char *result = malloc(16);
    
    switch (expr->type) {
        case AST_NUMBER: {
            const NumberAST *n = (const NumberAST *)expr;
            snprintf(result, 16, "%d", n->value);
            return result;
        }
        
        case AST_UNARY: {
            const UnaryAST *u = (const UnaryAST *)expr;
            char *operand = codegen_expr(gen, u->operand);
            
            switch (u->op) {
                case '+': 
                    // 一元加号不生成任何代码，直接返回操作数
                    free(result);
                    return operand;
                    
                case '-': {
                    for (int i = 0; i < gen->indent_level; i++) {
                        fprintf(gen->output, "  ");
                    }
                    snprintf(result, 16, "%%%d", gen->temp_counter);
                    fprintf(gen->output, "%s = sub 0, %s\n", result, operand);
                    gen->temp_counter++;
                    break;
                }
                
                case '!': {
                    for (int i = 0; i < gen->indent_level; i++) {
                        fprintf(gen->output, "  ");
                    }
                    snprintf(result, 16, "%%%d", gen->temp_counter);
                    fprintf(gen->output, "%s = eq %s, 0\n", result, operand);
                    gen->temp_counter++;
                    break;
                }
                
                default:
                    assert(0 && "Unknown unary operator");
            }
            
            free(operand);
            return result;
        }
        
        case AST_BINARY: {
            const BinaryAST *b = (const BinaryAST *)expr;
            
            if (b->op == '&' || b->op == '|') {
                
                char *left = codegen_expr(gen, b->left);
                
                // 创建临时变量存储结果
                snprintf(result, 16, "%%%d", gen->temp_counter);
                int result_var = gen->temp_counter++;
                
                // 检查左操作数是否为0
                for (int i = 0; i < gen->indent_level; i++) {
                    fprintf(gen->output, "  ");
                }
                int left_bool = gen->temp_counter++;
                fprintf(gen->output, "%%%d = ne %s, 0\n", left_bool, left);
                
                if (b->op == '&') {
                    // 逻辑与
                    char *right = codegen_expr(gen, b->right);
                    for (int i = 0; i < gen->indent_level; i++) {
                        fprintf(gen->output, "  ");
                    }
                    int right_bool = gen->temp_counter++;
                    fprintf(gen->output, "%%%d = ne %s, 0\n", right_bool, right);
                    
                    for (int i = 0; i < gen->indent_level; i++) {
                        fprintf(gen->output, "  ");
                    }
                    fprintf(gen->output, "%%%d = and %%%d, %%%d\n", result_var, left_bool, right_bool);
                    free(right);
                } else {
                    // 逻辑或
                    char *right = codegen_expr(gen, b->right);
                    for (int i = 0; i < gen->indent_level; i++) {
                        fprintf(gen->output, "  ");
                    }
                    int right_bool = gen->temp_counter++;
                    fprintf(gen->output, "%%%d = ne %s, 0\n", right_bool, right);
                    
                    for (int i = 0; i < gen->indent_level; i++) {
                        fprintf(gen->output, "  ");
                    }
                    fprintf(gen->output, "%%%d = or %%%d, %%%d\n", result_var, left_bool, right_bool);
                    free(right);
                }
                
                free(left);
                return result;
            }
            
            // 二元运算
            char *left = codegen_expr(gen, b->left);
            char *right = codegen_expr(gen, b->right);
            
            const char *koopa_op = NULL;
            switch (b->op) {
                case '+': koopa_op = "add"; break;
                case '-': koopa_op = "sub"; break;
                case '*': koopa_op = "mul"; break;
                case '/': koopa_op = "div"; break;
                case '%': koopa_op = "mod"; break;
                case '<': koopa_op = "lt"; break;
                case '>': koopa_op = "gt"; break;
                case 'l': koopa_op = "le"; break;
                case 'g': koopa_op = "ge"; break;
                case 'e': koopa_op = "eq"; break;
                case 'n': koopa_op = "ne"; break;
                default: assert(0 && "Unknown binary operator");
            }
            
            for (int i = 0; i < gen->indent_level; i++) {
                fprintf(gen->output, "  ");
            }
            snprintf(result, 16, "%%%d", gen->temp_counter);
            fprintf(gen->output, "%s = %s %s, %s\n", result, koopa_op, left, right);
            gen->temp_counter++;
            
            free(left);
            free(right);
            return result;
        }
        
        default:
            assert(0 && "Unknown expression type");
            free(result);
            return NULL;
    }
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
                case '!': *out = (v == 0); return 1;
                default: return 0;
            }
        }
        case AST_BINARY: {
            const BinaryAST *b = (const BinaryAST *)expr;
            int left_val = 0, right_val = 0;
            if (!eval_const_expr(b->left, &left_val)) return 0;
            if (!eval_const_expr(b->right, &right_val)) return 0;
            switch (b->op) {
                case '+': *out = left_val + right_val; return 1;
                case '-': *out = left_val - right_val; return 1;
                case '*': *out = left_val * right_val; return 1;
                case '/': 
                    if (right_val == 0) return 0;
                    *out = left_val / right_val; 
                    return 1;
                case '%': 
                    if (right_val == 0) return 0;
                    *out = left_val % right_val; 
                    return 1;
                case '<': *out = (left_val < right_val); return 1;
                case '>': *out = (left_val > right_val); return 1;
                case 'l': *out = (left_val <= right_val); return 1;  // <=
                case 'g': *out = (left_val >= right_val); return 1;  // >=
                case 'e': *out = (left_val == right_val); return 1;  // ==
                case 'n': *out = (left_val != right_val); return 1;  // !=
                case '&': *out = (left_val && right_val); return 1;  // &&
                case '|': *out = (left_val || right_val); return 1;  // ||
                default: return 0;
            }
        }
        default:
            return 0;
    }
}