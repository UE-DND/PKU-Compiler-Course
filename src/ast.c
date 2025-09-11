/**
 * AST节点的具体实现文件
 * 
 */

#include "ast.h"

// ========================================
// AST节点dump函数实现 - 用于打印AST结构
// ========================================

/**
 * 编译单元dump函数
 * 输出格式：CompUnitAST { [函数定义] }
 */
static void comp_unit_dump(const BaseAST *self) {
    const CompUnitAST *comp_unit = (const CompUnitAST *)self;
    printf("CompUnitAST { ");
    comp_unit->func_def->dump(comp_unit->func_def);
    printf(" }");
}

/**
 * 函数定义dump函数
 * 输出格式：FuncDefAST { [函数类型], [函数名], [函数体] }
 */
static void func_def_dump(const BaseAST *self) {
    const FuncDefAST *func_def = (const FuncDefAST *)self;
    printf("FuncDefAST { ");
    func_def->func_type->dump(func_def->func_type);
    printf(", %s, ", func_def->ident);
    func_def->block->dump(func_def->block);
    printf(" }");
}

/**
 * 函数类型dump函数
 * 输出格式：FuncTypeAST { int }
 */
static void func_type_dump(const BaseAST *self) {
    printf("FuncTypeAST { int }");
}

/**
 * 代码块dump函数
 * 输出格式：BlockAST { [语句] }
 */
static void block_dump(const BaseAST *self) {
    const BlockAST *block = (const BlockAST *)self;
    printf("BlockAST { ");
    block->stmt->dump(block->stmt);
    printf(" }");
}

/**
 * 语句dump函数
 * 输出格式：StmtAST { [数值] }
 */
static void stmt_dump(const BaseAST *self) {
    const StmtAST *stmt = (const StmtAST *)self;
    printf("StmtAST { ");
    stmt->number->dump(stmt->number);
    printf(" }");
}

/**
 * 数字字面量dump函数
 * 输出格式：直接输出数值
 */
static void number_dump(const BaseAST *self) {
    const NumberAST *number = (const NumberAST *)self;
    printf("%d", number->value);
}

// ========================================
// AST节点destroy函数实现 - 用于释放内存
// ========================================

/**
 * 编译单元destroy函数
 * 递归销毁函数定义节点，然后释放自身内存
 */
static void comp_unit_destroy(BaseAST *self) {
    CompUnitAST *comp_unit = (CompUnitAST *)self;
    if (comp_unit->func_def) {
        comp_unit->func_def->destroy(comp_unit->func_def);
    }
    free(comp_unit);
}

/**
 * 函数定义destroy函数
 * 递归销毁函数类型、函数名、函数体节点，然后释放自身内存
 */
static void func_def_destroy(BaseAST *self) {
    FuncDefAST *func_def = (FuncDefAST *)self;
    if (func_def->func_type) {
        func_def->func_type->destroy(func_def->func_type);
    }
    if (func_def->ident) {
        free(func_def->ident);  // 释放函数名字符串
    }
    if (func_def->block) {
        func_def->block->destroy(func_def->block);
    }
    free(func_def);
}

/**
 * 函数类型destroy函数
 * 只需释放自身内存（没有子节点）
 */
static void func_type_destroy(BaseAST *self) {
    free(self);
}

/**
 * 代码块destroy函数
 * 递归销毁语句节点，然后释放自身内存
 */
static void block_destroy(BaseAST *self) {
    BlockAST *block = (BlockAST *)self;
    if (block->stmt) {
        block->stmt->destroy(block->stmt);
    }
    free(block);
}

/**
 * 语句destroy函数
 * 递归销毁数值节点，然后释放自身内存
 */
static void stmt_destroy(BaseAST *self) {
    StmtAST *stmt = (StmtAST *)self;
    if (stmt->number) {
        stmt->number->destroy(stmt->number);
    }
    free(stmt);
}

/**
 * 数字字面量destroy函数
 * 只需释放自身内存（没有子节点）
 */
static void number_destroy(BaseAST *self) {
    free(self);
}

// ========================================
// AST节点构造函数实现 - 用于创建AST节点
// ========================================

/**
 * 创建编译单元AST节点
 * 分配内存，初始化虚函数表，设置函数定义子节点
 */
BaseAST* create_comp_unit_ast(BaseAST *func_def) {
    CompUnitAST *comp_unit = malloc(sizeof(CompUnitAST));
    comp_unit->base.type = AST_COMP_UNIT;
    comp_unit->base.dump = comp_unit_dump;
    comp_unit->base.destroy = comp_unit_destroy;
    comp_unit->func_def = func_def;
    return (BaseAST *)comp_unit;
}

/**
 * 创建函数定义AST节点
 * 分配内存，初始化虚函数表，设置函数类型、函数名、函数体子节点
 */
BaseAST* create_func_def_ast(BaseAST *func_type, char *ident, BaseAST *block) {
    FuncDefAST *func_def = malloc(sizeof(FuncDefAST));
    func_def->base.type = AST_FUNC_DEF;
    func_def->base.dump = func_def_dump;
    func_def->base.destroy = func_def_destroy;
    func_def->func_type = func_type;
    func_def->ident = ident;  // 注意：这里直接使用传入的字符串指针
    func_def->block = block;
    return (BaseAST *)func_def;
}

/**
 * 创建函数类型AST节点
 * 分配内存，初始化虚函数表（目前只支持int类型）
 */
BaseAST* create_func_type_ast(void) {
    FuncTypeAST *func_type = malloc(sizeof(FuncTypeAST));
    func_type->base.type = AST_FUNC_TYPE;
    func_type->base.dump = func_type_dump;
    func_type->base.destroy = func_type_destroy;
    return (BaseAST *)func_type;
}

/**
 * 创建代码块AST节点
 * 分配内存，初始化虚函数表，设置语句子节点
 */
BaseAST* create_block_ast(BaseAST *stmt) {
    BlockAST *block = malloc(sizeof(BlockAST));
    block->base.type = AST_BLOCK;
    block->base.dump = block_dump;
    block->base.destroy = block_destroy;
    block->stmt = stmt;
    return (BaseAST *)block;
}

/**
 * 创建语句AST节点
 * 分配内存，初始化虚函数表，设置数值子节点
 */
BaseAST* create_stmt_ast(BaseAST *number) {
    StmtAST *stmt = malloc(sizeof(StmtAST));
    stmt->base.type = AST_STMT;
    stmt->base.dump = stmt_dump;
    stmt->base.destroy = stmt_destroy;
    stmt->number = number;
    return (BaseAST *)stmt;
}

/**
 * 创建数字字面量AST节点
 * 分配内存，初始化虚函数表，设置整数值
 */
BaseAST* create_number_ast(int value) {
    NumberAST *number = malloc(sizeof(NumberAST));
    number->base.type = AST_NUMBER;
    number->base.dump = number_dump;
    number->base.destroy = number_destroy;
    number->value = value;
    return (BaseAST *)number;
}

// ========================================
// 全局AST操作函数实现
// ========================================

/**
 * 销毁AST树的全局函数
 * 通过虚函数表调用对应节点的destroy函数
 * @param ast 要销毁的AST根节点
 */
void destroy_ast(BaseAST *ast) {
    if (ast) {
        ast->destroy(ast);  // 调用虚函数表中的destroy函数
    }
}

/**
 * 打印AST结构的全局函数
 * 通过虚函数表调用对应节点的dump函数
 * @param ast 要打印的AST节点
 */
void dump_ast(const BaseAST *ast) {
    if (ast) {
        ast->dump(ast);  // 调用虚函数表中的dump函数
    }
}
