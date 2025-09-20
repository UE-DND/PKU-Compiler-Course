#pragma once

#include <stdio.h>
#include "koopa.h"

#ifdef __cplusplus
extern "C" {
#endif

// 从 Koopa IR 文本解析为 raw program；成功返回 0，失败返回非 0
int koopa_parse_ir_from_string(const char *ir_text, koopa_raw_program_builder_t *out_builder, koopa_raw_program_t *out_raw);

// 遍历 raw program，打印每个函数名到 stderr
void koopa_dump_functions_to_stderr(koopa_raw_program_t raw);

#ifdef __cplusplus
}
#endif
