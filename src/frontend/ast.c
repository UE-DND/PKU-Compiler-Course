#include "ast.h"

static void comp_unit_dump(const BaseAST *self) {
    const CompUnitAST *comp_unit = (const CompUnitAST *)self;
    printf("CompUnitAST { ");
    comp_unit->func_def->dump(comp_unit->func_def);
    printf(" }");
}

static void func_def_dump(const BaseAST *self) {
    const FuncDefAST *func_def = (const FuncDefAST *)self;
    printf("FuncDefAST { ");
    func_def->func_type->dump(func_def->func_type);
    printf(", %s, ", func_def->ident);
    func_def->block->dump(func_def->block);
    printf(" }");
}

static void func_type_dump(const BaseAST *self) {
    printf("FuncTypeAST { int }");
}

static void block_dump(const BaseAST *self) {
    const BlockAST *block = (const BlockAST *)self;
    printf("BlockAST { ");
    block->stmt->dump(block->stmt);
    printf(" }");
}

static void stmt_dump(const BaseAST *self) {
    const StmtAST *stmt = (const StmtAST *)self;
    printf("StmtAST { ");
    stmt->expr->dump(stmt->expr);
    printf(" }");
}

static void number_dump(const BaseAST *self) {
    const NumberAST *number = (const NumberAST *)self;
    printf("%d", number->value);
}

static void unary_dump(const BaseAST *self) {
    const UnaryAST *unary = (const UnaryAST *)self;
    printf("UnaryAST { %c, ", unary->op);
    unary->operand->dump(unary->operand);
    printf(" }");
}

static void comp_unit_destroy(BaseAST *self) {
    CompUnitAST *comp_unit = (CompUnitAST *)self;
    if (comp_unit->func_def) {
        comp_unit->func_def->destroy(comp_unit->func_def);
    }
    free(comp_unit);
}

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

static void func_type_destroy(BaseAST *self) {
    free(self);
}

static void block_destroy(BaseAST *self) {
    BlockAST *block = (BlockAST *)self;
    if (block->stmt) {
        block->stmt->destroy(block->stmt);
    }
    free(block);
}

static void stmt_destroy(BaseAST *self) {
    StmtAST *stmt = (StmtAST *)self;
    if (stmt->expr) {
        stmt->expr->destroy(stmt->expr);
    }
    free(stmt);
}

static void number_destroy(BaseAST *self) {
    free(self);
}

static void unary_destroy(BaseAST *self) {
    UnaryAST *unary = (UnaryAST *)self;
    if (unary->operand) unary->operand->destroy(unary->operand);
    free(unary);
}

BaseAST* create_comp_unit_ast(BaseAST *func_def) {
    CompUnitAST *comp_unit = malloc(sizeof(CompUnitAST));
    comp_unit->base.type = AST_COMP_UNIT;
    comp_unit->base.dump = comp_unit_dump;
    comp_unit->base.destroy = comp_unit_destroy;
    comp_unit->func_def = func_def;
    return (BaseAST *)comp_unit;
}

BaseAST* create_func_def_ast(BaseAST *func_type, char *ident, BaseAST *block) {
    FuncDefAST *func_def = malloc(sizeof(FuncDefAST));
    func_def->base.type = AST_FUNC_DEF;
    func_def->base.dump = func_def_dump;
    func_def->base.destroy = func_def_destroy;
    func_def->func_type = func_type;
    func_def->ident = ident;  // 这里直接使用传入的字符串指针
    func_def->block = block;
    return (BaseAST *)func_def;
}

BaseAST* create_func_type_ast(void) {
    FuncTypeAST *func_type = malloc(sizeof(FuncTypeAST));
    func_type->base.type = AST_FUNC_TYPE;
    func_type->base.dump = func_type_dump;
    func_type->base.destroy = func_type_destroy;
    return (BaseAST *)func_type;
}

BaseAST* create_block_ast(BaseAST *stmt) {
    BlockAST *block = malloc(sizeof(BlockAST));
    block->base.type = AST_BLOCK;
    block->base.dump = block_dump;
    block->base.destroy = block_destroy;
    block->stmt = stmt;
    return (BaseAST *)block;
}

BaseAST* create_stmt_ast(BaseAST *number) {
    StmtAST *stmt = malloc(sizeof(StmtAST));
    stmt->base.type = AST_STMT;
    stmt->base.dump = stmt_dump;
    stmt->base.destroy = stmt_destroy;
    stmt->expr = number;
    return (BaseAST *)stmt;
}

BaseAST* create_number_ast(int value) {
    NumberAST *number = malloc(sizeof(NumberAST));
    number->base.type = AST_NUMBER;
    number->base.dump = number_dump;
    number->base.destroy = number_destroy;
    number->value = value;
    return (BaseAST *)number;
}

void destroy_ast(BaseAST *ast) {
    if (ast) {
        ast->destroy(ast);
    }
}


void dump_ast(const BaseAST *ast) {
    if (ast) {
        ast->dump(ast);
    }
}

BaseAST* create_unary_ast(char op, BaseAST *operand) {
    UnaryAST *u = malloc(sizeof(UnaryAST));
    u->base.type = AST_UNARY;
    u->base.dump = unary_dump;
    u->base.destroy = unary_destroy;
    u->op = op;
    u->operand = operand;
    return (BaseAST *)u;
}
