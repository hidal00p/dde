#ifndef DDE_INSTRUMENTATION_H
#define DDE_INSTRUMENTATION_H

#include "transform_ctx.h"

namespace analysis {
void track_mem_upd_mov(CONTEXT *ctx, binary_op::ctx *mov_ctx, bool is_pop,
                       ADDRINT ea);

void track_mem_upd_add(CONTEXT *ctx, binary_op::ctx *add_ctx, bool is_pop,
                       ADDRINT ea);

void track_mem_upd_mul(CONTEXT *ctx, binary_op::ctx *mul_ctx, bool is_pop,
                       ADDRINT ea);

void track_mem_upd_div(CONTEXT *ctx, binary_op::ctx *div_ctx, bool is_pop,
                       ADDRINT ea);

void track_mem_upd_sub(CONTEXT *ctx, binary_op::ctx *sub_ctx, bool is_pop,
                       ADDRINT ea);

void track_mem_upd_sch();

} // namespace analysis

namespace instrumentation {
binary_op::ctx *get_bop_operands(INS ins);

void handle_bop(INS ins, binary_op::ctx *bop_ctx, AFUNPTR func, bool is_pop);

void handle_mov(INS ins, bool is_pop = false);

void handle_mov_const(INS ins, uint8_t constant);

void handle_add(INS ins, bool is_pop = false);

void handle_mul(INS ins, bool is_pop = false);

void handle_div(INS ins, bool is_pop = false);

void handle_sub(INS ins, bool is_pop = false);

void handle_sign_change(INS ins);
} // namespace instrumentation

#endif
