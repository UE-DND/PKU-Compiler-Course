#pragma once

#include <stdio.h>
#include "koopa.h"

#ifdef __cplusplus
extern "C" {
#endif

// 从 raw program 生成 RISC-V 汇编代码
void generate_riscv_from_raw_program(FILE *output, koopa_raw_program_t raw);

#ifdef __cplusplus
}
#endif