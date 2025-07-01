#include "pin.H"
#include "transform_ctx.h"

#include "graph.h"
#include "handlers.h"
#include "params.h"

#include <cassert>
#define NDEBUG

bool is_abi_reg(REG reg) {
  bool xmm_reg = reg >= REG_XMM_BASE && reg <= REG_XMM_SSE_LAST;
  bool rax_reg = reg == REG_RAX;
  return xmm_reg || rax_reg;
}

namespace analysis {
namespace mov {
void track_marked_mem_reg(ADDRINT read_ea, REG write_reg) {
  double value;
  PIN_SafeCopy((void *)&value, (void *)read_ea, sizeof(value));

  NodePtr node = std::make_shared<Node>(value);
  node->uuid = var_marking_ctx.mark;
  node->output = var_marking_ctx.output;

  reg::insert_node(write_reg, node);
}
void track_mem_reg(ADDRINT read_ea, REG write_reg) {
  if (!mem::is_node_recorded(read_ea)) {
    reg::clean_reg(write_reg);
    return;
  }

  mem::write_to_reg(read_ea, write_reg);
}
void track_reg_mem(REG read_reg, ADDRINT write_ea) {
  if (!reg::is_node_recorded(read_reg)) {
    return;
  }
  reg::write_to_mem(read_reg, write_ea);
}
void track_reg_reg(REG read_reg, REG write_reg) {
  if (!reg::is_node_recorded(read_reg)) {
    reg::clean_reg(write_reg);
    return;
  }

  reg::write_to_other_reg(read_reg, write_reg);
}

void track_imm_reg(REG write_reg) { reg::clean_reg(write_reg); }
} // namespace mov
} // namespace analysis

#ifndef TEST_MODE
namespace instrumentation {

void handle_mov(INS ins) {
  binary_op::ctx *mov_ctx = binary_op::get_bop_operands(ins);

  if (var_marking_ctx.is_var_marked && mov_ctx->src.type == OprType::MEM) {
    INS_InsertCall(
        ins, IPOINT_BEFORE, (AFUNPTR)analysis::mov::track_marked_mem_reg,
        IARG_MEMORYREAD_EA, IARG_UINT32, mov_ctx->dest.origin.reg, IARG_END);
  } else if (mov_ctx->src.type == OprType::MEM) {
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::mov::track_mem_reg,
                   IARG_MEMORYREAD_EA, IARG_UINT32, mov_ctx->dest.origin.reg,
                   IARG_END);
  } else if (mov_ctx->dest.type == OprType::MEM) {
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::mov::track_reg_mem,
                   IARG_UINT32, mov_ctx->src.origin.reg, IARG_MEMORYWRITE_EA,
                   IARG_END);
  } else if (mov_ctx->dest.type == OprType::REGSTR &&
             mov_ctx->src.type == OprType::REGSTR) {
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::mov::track_reg_reg,
                   IARG_UINT32, mov_ctx->src.origin.reg, IARG_UINT32,
                   mov_ctx->dest.origin.reg, IARG_END);
  } else {
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::mov::track_imm_reg,
                   IARG_UINT32, mov_ctx->dest.origin.reg, IARG_END);
  }
}
} // namespace instrumentation
#endif
