#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    AST_COMP_UNIT,
    AST_FUNC_DEF,
    AST_FUNC_TYPE,
    AST_BLOCK,
    AST_STMT,
    AST_NUMBER
} ASTNodeType;

/**
 * 基础AST节点结构体
 * 所有AST节点都继承自这个结构体，包含：
 * - type: 节点类型标识符
 * - dump: 虚函数，用于打印AST结构
 * - destroy: 虚函数，用于释放内存
 */
typedef struct BaseAST BaseAST;
struct BaseAST {
    ASTNodeType type;
    void (*dump)(const BaseAST *self);
    void (*destroy)(BaseAST *self);
};

/**
 * 编译单元AST节点
 * CompUnit ::= FuncDef;
 * 表示整个程序的根节点，包含一个函数定义
 */
typedef struct {
    BaseAST base;
    BaseAST *func_def;  // 函数定义节点
} CompUnitAST;

/**
 * 函数定义AST节点
 * FuncDef ::= FuncType IDENT "(" ")" Block;
 * 表示函数定义，包含函数类型、函数名和函数体
 */
typedef struct {
    BaseAST base;
    BaseAST *func_type;  // 函数返回类型
    char *ident;         // 函数名
    BaseAST *block;      // 函数体代码块
} FuncDefAST;

/**
 * 函数类型AST节点
 * FuncType ::= "int";
 * 表示函数的返回类型，目前只支持int类型
 */
typedef struct {
    BaseAST base;
    // 目前只有int类型
} FuncTypeAST;

/**
 * 代码块AST节点
 * Block ::= "{" Stmt "}";
 * 表示一对大括号包围的代码块，包含一条语句
 */
typedef struct {
    BaseAST base;
    BaseAST *stmt;  // 代码块中的语句
} BlockAST;

/**
 * 语句AST节点
 * Stmt ::= "return" Number ";";
 * 表示return语句，包含要返回的数值表达式
 */
typedef struct {
    BaseAST base;
    BaseAST *number;  // 要返回的数值
} StmtAST;

/**
 * 数字字面量AST节点
 * Number ::= INT_CONST;
 * 表示整数常量，存储具体的数值
 */
typedef struct {
    BaseAST base;
    int value;  // 整数值
} NumberAST;

// ========================================
// AST节点创建函数
// ========================================

/**
 * 创建编译单元AST节点
 * @param func_def 函数定义节点
 * @return 新创建的CompUnitAST节点
 */
BaseAST* create_comp_unit_ast(BaseAST *func_def);

/**
 * 创建函数定义AST节点
 * @param func_type 函数类型节点
 * @param ident 函数名字符串
 * @param block 函数体代码块节点
 * @return 新创建的FuncDefAST节点
 */
BaseAST* create_func_def_ast(BaseAST *func_type, char *ident, BaseAST *block);

/**
 * 创建函数类型AST节点
 * @return 新创建的FuncTypeAST节点（int类型）
 */
BaseAST* create_func_type_ast(void);

/**
 * 创建代码块AST节点
 * @param stmt 代码块中的语句节点
 * @return 新创建的BlockAST节点
 */
BaseAST* create_block_ast(BaseAST *stmt);

/**
 * 创建语句AST节点
 * @param number 要返回的数值节点
 * @return 新创建的StmtAST节点
 */
BaseAST* create_stmt_ast(BaseAST *number);

/**
 * 创建数字字面量AST节点
 * @param value 整数值
 * @return 新创建的NumberAST节点
 */
BaseAST* create_number_ast(int value);

// ========================================
// AST操作函数
// ========================================

/**
 * 销毁AST树，释放所有动态分配的内存
 * @param ast 要销毁的AST根节点
 */
void destroy_ast(BaseAST *ast);

/**
 * 打印AST结构到标准输出
 * @param ast 要打印的AST节点
 */
void dump_ast(const BaseAST *ast);
