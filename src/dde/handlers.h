#ifndef DDE_INSTRUMENTATION_H
#define DDE_INSTRUMENTATION_H

#include "transform_ctx.h"

namespace analysis {
// void track_fpu_mov(binary_op::ctx *mov_ctx, bool is_pop, ADDRINT ea);

void track_reg_mov(binary_op::ctx *mov_ctx, ADDRINT ea);

// void track_add(binary_op::ctx *add_ctx, bool is_pop, ADDRINT ea);
//
// void track_mul(binary_op::ctx *mul_ctx, bool is_pop, ADDRINT ea);
//
// void track_div(binary_op::ctx *div_ctx, bool is_pop, bool is_reverse,
//                ADDRINT ea);
//
// void track_sub(binary_op::ctx *sub_ctx, bool is_pop, bool is_reverse,
//                ADDRINT ea);
//
// void track_sch();
// void track_call_to_intrinsic(ADDRINT branch_addr, ADDRINT callee_addr);
// void track_ret_from_intrinsic(ADDRINT branch_addr, ADDRINT callee_addr);
} // namespace analysis

#ifndef TEST_MODE
namespace instrumentation {
void handle_commut_bop(INS ins, binary_op::ctx *bop_ctx, AFUNPTR func,
                       bool is_pop);

void handle_non_commut_bop(INS ins, binary_op::ctx *bop_ctx, AFUNPTR func,
                           bool is_pop, bool is_reverse);

void handle_reg_mov(INS ins);

// void handle_fpu_mov(INS ins, bool is_pop = false);
//
// void handle_fpu_const_load(INS ins, uint8_t constant);
//
// void handle_add(INS ins, bool is_pop = false);
//
// void handle_mul(INS ins, bool is_pop = false);
//
// void handle_div(INS ins, bool is_pop = false, bool is_reverse = false);
//
// void handle_sub(INS ins, bool is_pop = false, bool is_reverse = false);
//
// void handle_sign_change(INS ins);
//
// void handle_call(INS ins);
//
// void handle_ret(INS ins);
} // namespace instrumentation
#endif

#endif
