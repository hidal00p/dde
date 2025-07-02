#pragma once

#include "graph.h"
#include "pin.H"
#include "transform_ctx.h"

namespace analysis {
namespace mov {
void track_marked_mem_reg(ADDRINT read_ea, REG write_reg);
void track_marked_reg_mem(CONTEXT *ctx, REG read_reg, ADDRINT write_ea);
void track_mem_reg(ADDRINT read_ea, REG write_reg);
void track_reg_mem(REG read_reg, ADDRINT write_ea);
void track_reg_reg(REG read_reg, REG write_reg);
void track_imm_reg(REG write_reg);
} // namespace mov
namespace binary {
template <typename Operation, Transformation Transform>
void track_mem_reg(CONTEXT *ctx, ADDRINT read_ea, REG write_reg) {
  bool node_in_reg = reg::is_node_recorded(write_reg);
  bool node_in_mem = mem::is_node_recorded(read_ea);

  if (!(node_in_reg || node_in_mem))
    return;

  NodePtr src_node;
  if (node_in_mem) {
    src_node = mem::expect_node(read_ea);
  } else {
    double value;
    PIN_SafeCopy((void *)&value, (void *)read_ea, sizeof(double));

    src_node = std::make_shared<Node>(value);
  }

  NodePtr dest_node;
  if (node_in_reg) {
    dest_node = reg::expect_node(write_reg);
  } else {
    double value;
    PIN_GetContextRegval(ctx, write_reg, (uint8_t *)&value);

    dest_node = std::make_shared<Node>(value);
  }

  Operation op;
  NodePtr res_node =
      std::make_shared<Node>(op(dest_node->value, src_node->value),
                             (NodePtrVec){dest_node, src_node}, Transform);
  reg::insert_node(write_reg, res_node);
}
template <typename Operation, Transformation Transform>
void track_reg_mem(CONTEXT *ctx, REG read_reg, ADDRINT write_ea) {
  bool node_in_reg = reg::is_node_recorded(read_reg);
  bool node_in_mem = mem::is_node_recorded(write_ea);

  if (!(node_in_reg || node_in_mem))
    return;

  NodePtr src_node;
  if (node_in_reg) {
    src_node = reg::expect_node(read_reg);
  } else {
    double value;
    PIN_GetContextRegval(ctx, read_reg, (uint8_t *)&value);
    src_node = std::make_shared<Node>(value);
  }

  NodePtr dest_node;
  if (node_in_mem) {
    dest_node = mem::expect_node(write_ea);
  } else {
    double value;
    PIN_SafeCopy((void *)&value, (void *)write_ea, sizeof(double));

    dest_node = std::make_shared<Node>(value);
  }

  Operation op;
  NodePtr res_node =
      std::make_shared<Node>(op(dest_node->value, src_node->value),
                             (NodePtrVec){dest_node, src_node}, Transform);
  mem::insert_node(write_ea, res_node);
}
template <typename Operation, Transformation Transform>
void track_reg_reg(CONTEXT *ctx, REG read_reg, REG write_reg) {
  bool node_in_src_reg = reg::is_node_recorded(read_reg);
  bool node_in_dest_reg = reg::is_node_recorded(write_reg);

  if (!(node_in_src_reg || node_in_dest_reg))
    return;

  NodePtr src_node;
  if (node_in_src_reg) {
    src_node = reg::expect_node(read_reg);
  } else {
    double value;
    PIN_GetContextRegval(ctx, read_reg, (uint8_t *)&value);
    src_node = std::make_shared<Node>(value);
  }

  NodePtr dest_node;
  if (node_in_dest_reg) {
    dest_node = reg::expect_node(write_reg);
  } else {
    double value;
    PIN_GetContextRegval(ctx, write_reg, (uint8_t *)&value);
    dest_node = std::make_shared<Node>(value);
  }

  Operation op;
  NodePtr res_node =
      std::make_shared<Node>(op(dest_node->value, src_node->value),
                             (NodePtrVec){dest_node, src_node}, Transform);
  reg::insert_node(write_reg, res_node);
}
} // namespace binary
namespace call {
void track_call_to_intrinsic(ADDRINT branch_addr, ADDRINT callee_addr);
void track_ret_from_intrinsic(ADDRINT branch_addr, ADDRINT callee_addr);
} // namespace call
} // namespace analysis

#ifndef TEST_MODE
namespace instrumentation {
void handle_clear_reg(INS ins);
void handle_mov(INS ins);
template <typename Operation, Transformation Transform>
void handle_binary(INS ins) {
  binary_op::ctx *binary_ctx = binary_op::get_bop_operands(ins);

  if (binary_ctx->src.type == OprType::MEM) {
    INS_InsertCall(
        ins, IPOINT_BEFORE,
        (AFUNPTR)analysis::binary::track_mem_reg<Operation, Transform>,
        IARG_CONTEXT, IARG_MEMORYREAD_EA, IARG_UINT32,
        binary_ctx->dest.origin.reg, IARG_END);
  } else if (binary_ctx->dest.type == OprType::MEM) {
    INS_InsertCall(
        ins, IPOINT_BEFORE,
        (AFUNPTR)analysis::binary::track_reg_mem<Operation, Transform>,
        IARG_CONTEXT, IARG_UINT32, binary_ctx->src.origin.reg,
        IARG_MEMORYWRITE_EA, IARG_END);
  } else {
    INS_InsertCall(
        ins, IPOINT_BEFORE,
        (AFUNPTR)analysis::binary::track_reg_reg<Operation, Transform>,
        IARG_CONTEXT, IARG_UINT32, binary_ctx->src.origin.reg, IARG_UINT32,
        binary_ctx->dest.origin.reg, IARG_END);
  }
}

void handle_call(INS ins);
void handle_ret(INS ins);
} // namespace instrumentation
#endif
