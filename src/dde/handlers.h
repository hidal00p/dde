#ifndef DDE_INSTRUMENTATION_H
#define DDE_INSTRUMENTATION_H

#include "pin.H"

namespace analysis {
// void track_fpu_mov(binary_op::ctx *mov_ctx, bool is_pop, ADDRINT ea);
namespace mov {
void track_marked_mem_reg(ADDRINT read_ea, REG write_reg);
void track_mem_reg(ADDRINT read_ea, REG write_reg);
void track_reg_mem(REG read_reg, ADDRINT write_ea);
void track_reg_reg(REG read_reg, REG write_reg);
void track_imm_reg(REG write_reg);
} // namespace mov
namespace binary {
template <typename op>
void track_mem_reg(CONTEXT *ctx, ADDRINT read_ea, REG write_reg);
template <typename op>
void track_reg_mem(CONTEXT *ctx, REG read_reg, ADDRINT write_ea);
template <typename op>
void track_reg_reg(CONTEXT *ctx, REG read_reg, REG write_reg);
} // namespace binary
namespace add {
void track_mem_reg(CONTEXT *ctx, ADDRINT read_ea, REG write_reg);
void track_reg_mem(CONTEXT *ctx, REG read_reg, ADDRINT write_ea);
void track_reg_reg(CONTEXT *ctx, REG read_reg, REG write_reg);
} // namespace add
namespace mul {
void track_mem_reg(CONTEXT *ctx, ADDRINT read_ea, REG write_reg);
void track_reg_mem(CONTEXT *ctx, REG read_reg, ADDRINT write_ea);
void track_reg_reg(CONTEXT *ctx, REG read_reg, REG write_reg);
} // namespace mul
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
// void handle_commut_bop(INS ins, binary_op::ctx *bop_ctx, AFUNPTR func,
//                        bool is_pop);
//
// void handle_non_commut_bop(INS ins, binary_op::ctx *bop_ctx, AFUNPTR func,
//                            bool is_pop, bool is_reverse);

template <typename op> void handle_binary(INS ins);
void handle_mov(INS ins);
void handle_add(INS ins);
void handle_mul(INS ins);

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
