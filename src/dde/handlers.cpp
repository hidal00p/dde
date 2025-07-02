#include "pin.H"
#include "transform_ctx.h"

#include "graph.h"
#include "handlers.h"
#include "params.h"

#include <cassert>
#define NDEBUG

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
void track_marked_reg_mem(CONTEXT *ctx, REG read_reg, ADDRINT write_ea) {
  if (reg::is_node_recorded(read_reg)) {
    reg::write_to_mem(read_reg, write_ea);
    return;
  }

  double value;
  PIN_GetContextRegval(ctx, read_reg, (uint8_t *)&value);
  NodePtr node = std::make_shared<Node>(value);
  node->uuid = var_marking_ctx.mark;
  node->output = var_marking_ctx.output;
  mem::insert_node(write_ea, node);
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

  if (mem::is_node_recorded(write_ea)) {
    NodePtr output_candidate = mem::expect_node(write_ea);
    if (output_candidate->output) {
      NodePtr transfer_node = reg::expect_node(read_reg);
      transfer_node->uuid = output_candidate->uuid;
      transfer_node->output = true;
    }
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
namespace call {
void track_call_to_intrinsic(ADDRINT branch_addr, ADDRINT callee_addr) {
  std::string branch_name = RTN_FindNameByAddress(branch_addr);
  std::string callee_name = RTN_FindNameByAddress(callee_addr);

#ifdef DEBUG
  std::cout << callee_name + " -> " + branch_name << std::endl;
#endif

  if (!rtn_is_valid_transform(branch_name) &&
      !rtn_is_valid_transform(callee_name)) {
    return;
  }

  call_pair.to = branch_name;
  call_pair.from = callee_name;
  dde_state.to_instrument = false;
#ifndef DEBUG
  // TODO: this is dangerous, what if nullopt
  Intrinsic intr = get_intrinsic_from_rtn_name(branch_name).value();

  NodePtr arg_node = reg::expect_node(REG_XMM0);
  NodePtr res_node =
      std::make_shared<Node>(intr.intrinsic_call(arg_node->value),
                             (NodePtrVec){arg_node}, intr.transf);

  reg::insert_node(REG_XMM0, res_node);
#endif
}

void track_ret_from_intrinsic(ADDRINT branch_addr, ADDRINT callee_addr) {

  std::string branch_name = RTN_FindNameByAddress(branch_addr);
  std::string callee_name = RTN_FindNameByAddress(callee_addr);

#ifdef DEBUG
  std::cout << callee_name + " -> " + branch_name << std::endl;
#endif

  if (!rtn_is_valid_transform(branch_name) &&
      !rtn_is_valid_transform(callee_name)) {
    return;
  }

  if (call_pair.reversed(branch_name, callee_name)) {
    call_pair.to.clear();
    call_pair.from.clear();
    dde_state.to_instrument = true;
  }
}
} // namespace call
} // namespace analysis

#ifndef TEST_MODE
namespace instrumentation {
void handle_clear_reg(INS ins) {
  INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)reg::clean_reg, IARG_UINT32,
                 INS_OperandReg(ins, 0), IARG_END);
}
void handle_mov(INS ins) {
  binary_op::ctx *mov_ctx = binary_op::get_bop_operands(ins);

  if (var_marking_ctx.is_var_marked && mov_ctx->src.type == OprType::MEM) {
    INS_InsertCall(
        ins, IPOINT_BEFORE, (AFUNPTR)analysis::mov::track_marked_mem_reg,
        IARG_MEMORYREAD_EA, IARG_UINT32, mov_ctx->dest.origin.reg, IARG_END);
  } else if (var_marking_ctx.is_var_marked &&
             mov_ctx->dest.type == OprType::MEM) {
    INS_InsertCall(ins, IPOINT_BEFORE,
                   (AFUNPTR)analysis::mov::track_marked_reg_mem, IARG_CONTEXT,
                   IARG_UINT32, mov_ctx->src.origin.reg, IARG_MEMORYWRITE_EA,
                   IARG_END);
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

void handle_call(INS ins) {
  ADDRINT callee_addr = INS_Address(ins);
  INS_InsertCall(ins, IPOINT_BEFORE,
                 (AFUNPTR)analysis::call::track_call_to_intrinsic,
                 IARG_BRANCH_TARGET_ADDR, IARG_ADDRINT, callee_addr, IARG_END);
}

void handle_ret(INS ins) {
  ADDRINT callee_addr = INS_Address(ins);
  INS_InsertCall(ins, IPOINT_BEFORE,
                 (AFUNPTR)analysis::call::track_ret_from_intrinsic,
                 IARG_BRANCH_TARGET_ADDR, IARG_ADDRINT, callee_addr, IARG_END);
  return;
}
} // namespace instrumentation
#endif
