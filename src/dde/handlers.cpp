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

namespace binary {
template <typename op>
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

  op f;
  NodePtr res_node = std::make_shared<Node>(
      f(src_node->value, dest_node->value), (NodePtrVec){src_node, dest_node},
      Transformation::ADD);
  reg::insert_node(write_reg, res_node);
}
template <typename op>
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

  op f;
  NodePtr res_node = std::make_shared<Node>(
      f(src_node->value, dest_node->value), (NodePtrVec){src_node, dest_node},
      Transformation::ADD);
  mem::insert_node(write_ea, res_node);
}
template <typename op>
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

  op f;
  NodePtr res_node = std::make_shared<Node>(
      f(src_node->value, dest_node->value), (NodePtrVec){src_node, dest_node},
      Transformation::ADD);
  reg::insert_node(write_reg, res_node);
}
} // namespace binary
namespace add {
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

  NodePtr res_node = std::make_shared<Node>(src_node->value + dest_node->value,
                                            (NodePtrVec){src_node, dest_node},
                                            Transformation::ADD);
  reg::insert_node(write_reg, res_node);
}
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

  NodePtr res_node = std::make_shared<Node>(src_node->value + dest_node->value,
                                            (NodePtrVec){src_node, dest_node},
                                            Transformation::ADD);
  mem::insert_node(write_ea, res_node);
}
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

  NodePtr res_node = std::make_shared<Node>(src_node->value + dest_node->value,
                                            (NodePtrVec){src_node, dest_node},
                                            Transformation::ADD);
  reg::insert_node(write_reg, res_node);
}
} // namespace add

namespace mul {
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

  NodePtr res_node = std::make_shared<Node>(src_node->value * dest_node->value,
                                            (NodePtrVec){src_node, dest_node},
                                            Transformation::MUL);
  reg::insert_node(write_reg, res_node);
}
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

  NodePtr res_node = std::make_shared<Node>(src_node->value * dest_node->value,
                                            (NodePtrVec){src_node, dest_node},
                                            Transformation::MUL);
  mem::insert_node(write_ea, res_node);
}
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

  NodePtr res_node = std::make_shared<Node>(src_node->value * dest_node->value,
                                            (NodePtrVec){src_node, dest_node},
                                            Transformation::MUL);
  reg::insert_node(write_reg, res_node);
}
} // namespace mul

// void track_div(binary_op::ctx *div_ctx, bool is_pop, bool is_reverse,
//                ADDRINT ea) {
//   // It is implied that the 2nd operand is a register
//
//   NodePtr src_node, dest_node;
//
//   if (div_ctx->src.type == OprType::MEM) {
//     assert(!is_pop);
//     src_node = mem::expect_node(ea);
//     dest_node = stack::pop();
//     double value = is_reverse ? src_node->value / dest_node->value
//                               : dest_node->value / src_node->value;
//
//     NodePtrVec operands;
//
//     if (is_reverse) {
//       operands = {src_node, dest_node};
//     } else {
//       operands = {dest_node, src_node};
//     }
//
//     NodePtr new_node =
//         std::make_shared<Node>(value, operands, Transformation::DIV);
//     stack::push(new_node);
//
//   } else if (div_ctx->src.type == OprType::REGSTR) {
//     uint8_t src_idx =
//         stack::size() - 1 -
//         get_fpu_stack_idx_from_st(div_ctx->src.origin.reg);
//     uint8_t dest_idx =
//         stack::size() - 1 -
//         get_fpu_stack_idx_from_st(div_ctx->dest.origin.reg);
//
//     src_node = stack::at(src_idx);
//     dest_node = stack::at(dest_idx);
//     double value = is_reverse ? src_node->value / dest_node->value
//                               : dest_node->value / src_node->value;
//
//     NodePtrVec operands;
//
//     if (is_reverse) {
//       operands = {src_node, dest_node};
//     } else {
//       operands = {dest_node, src_node};
//     }
//
//     NodePtr new_node =
//         std::make_shared<Node>(value, operands, Transformation::DIV);
//     stack::at(dest_idx, new_node);
//
//     if (is_pop) {
//       assert(stack::pop()->uuid == src_node->uuid);
//     }
//   } else {
//     assert(false);
//   }
// }
//
// void track_sub(binary_op::ctx *sub_ctx, bool is_pop, bool is_reverse,
//                ADDRINT ea) {
//   // It is implied that the 2nd operand is a register
//
//   NodePtr src_node, dest_node;
//
//   if (sub_ctx->src.type == OprType::MEM) {
//     assert(!is_pop);
//     src_node = mem::expect_node(ea);
//     dest_node = stack::pop();
//     double value = is_reverse ? src_node->value - dest_node->value
//                               : dest_node->value - src_node->value;
//
//     NodePtrVec operands;
//
//     if (is_reverse) {
//       operands = {src_node, dest_node};
//     } else {
//       operands = {dest_node, src_node};
//     }
//
//     NodePtr new_node =
//         std::make_shared<Node>(value, operands, Transformation::SUB);
//     stack::push(new_node);
//
//   } else if (sub_ctx->src.type == OprType::REGSTR) {
//     uint8_t src_idx =
//         stack::size() - 1 -
//         get_fpu_stack_idx_from_st(sub_ctx->src.origin.reg);
//     uint8_t dest_idx =
//         stack::size() - 1 -
//         get_fpu_stack_idx_from_st(sub_ctx->dest.origin.reg);
//
//     src_node = stack::at(src_idx);
//     dest_node = stack::at(dest_idx);
//
//     double value = is_reverse ? src_node->value - dest_node->value
//                               : dest_node->value - src_node->value;
//     NodePtrVec operands;
//
//     if (is_reverse) {
//       operands = {src_node, dest_node};
//     } else {
//       operands = {dest_node, src_node};
//     }
//
//     NodePtr new_node =
//         std::make_shared<Node>(value, operands, Transformation::SUB);
//     stack::at(dest_idx, new_node);
//
//     if (is_pop) {
//       assert(stack::pop()->uuid == src_node->uuid);
//     }
//   } else {
//     assert(false);
//   }
// }
//
// void track_sch() {
//   NodePtr src_node = stack::pop();
//   NodePtr new_node(
//       new Node(-1 * src_node->value, {src_node}, Transformation::CHS));
//   stack::push(new_node);
// }
//
// void track_call_to_intrinsic(ADDRINT branch_addr, ADDRINT callee_addr) {
//   std::string branch_name = RTN_FindNameByAddress(branch_addr);
//   std::string callee_name = RTN_FindNameByAddress(callee_addr);
//
// #ifdef DEBUG
//   std::cout << callee_name + " -> " + branch_name << std::endl;
// #endif
//
//   if (!rtn_is_valid_transform(branch_name) &&
//       !rtn_is_valid_transform(callee_name)) {
//     return;
//   }
//
//   call_pair.to = branch_name;
//   call_pair.from = callee_name;
//   dde_state.to_instrument = false;
// #ifndef DEBUG
//   // TODO: this is dangerous, what if nullopt
//   Intrinsic intr = get_intrinsic_from_rtn_name(branch_name).value();
//
//   NodePtr n = reg::expect_node(REG_XMM0);
//   NodePtr y = std::make_shared<Node>(intr.intrinsic_call(n->value),
//                                      (NodePtrVec){n}, intr.transf);
//
//   reg::insert_node(REG_XMM0, y);
// #endif
// }
//
// void track_ret_from_intrinsic(ADDRINT branch_addr, ADDRINT callee_addr) {
//
//   std::string branch_name = RTN_FindNameByAddress(branch_addr);
//   std::string callee_name = RTN_FindNameByAddress(callee_addr);
//
// #ifdef DEBUG
//   std::cout << callee_name + " -> " + branch_name << std::endl;
// #endif
//
//   if (!rtn_is_valid_transform(branch_name) &&
//       !rtn_is_valid_transform(callee_name)) {
//     return;
//   }
//
//   if (call_pair.reversed(branch_name, callee_name)) {
//     call_pair.to.clear();
//     call_pair.from.clear();
//     dde_state.to_instrument = true;
//   }
// }
} // namespace analysis

#ifndef TEST_MODE
namespace instrumentation {
// void handle_commut_bop(INS ins, binary_op::ctx *bop_ctx, AFUNPTR func) {
//
//   // For FP instructions we assume that memory can only be a source operand.
//   // This is a valid assumption, since all the FPU arithmetic operations are
//   // proxied via the FPU stack registers.
//   if (bop_ctx->src.type == OprType::MEM) {
//     INS_InsertCall(ins, IPOINT_BEFORE, func, IARG_CONTEXT, IARG_PTR, bop_ctx,
//                    IARG_BOOL, IARG_MEMORYREAD_EA, IARG_END);
//   } else if (bop_ctx->src.type == OprType::REGSTR) {
//     INS_InsertCall(ins, IPOINT_BEFORE, func, IARG_PTR, bop_ctx, IARG_BOOL,
//                    IARG_ADDRINT, 0, IARG_END);
//   } else {
//     assert(false);
//   }
// }
//
// void handle_non_commut_bop(INS ins, binary_op::ctx *bop_ctx, AFUNPTR func,
//                            bool is_pop, bool is_reverse) {
//
//   // For FP instructions we assume that memory can only be a source operand.
//   // This is a valid assumption, since all the FPU arithmetic operations are
//   // proxied via the FPU stack registers.
//   if (bop_ctx->src.type == OprType::MEM) {
//     INS_InsertCall(ins, IPOINT_BEFORE, func, IARG_PTR, bop_ctx, IARG_BOOL,
//                    is_pop, IARG_BOOL, is_reverse, IARG_MEMORYREAD_EA,
//                    IARG_END);
//   } else if (bop_ctx->src.type == OprType::REGSTR) {
//     INS_InsertCall(ins, IPOINT_BEFORE, func, IARG_PTR, bop_ctx, IARG_BOOL,
//                    is_pop, IARG_BOOL, is_reverse, IARG_ADDRINT, 0, IARG_END);
//   } else {
//     assert(false);
//   }
// }

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

void handle_add(INS ins) {
  binary_op::ctx *add_ctx = binary_op::get_bop_operands(ins);

  if (add_ctx->src.type == OprType::MEM) {
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::add::track_mem_reg,
                   IARG_CONTEXT, IARG_MEMORYREAD_EA, IARG_UINT32,
                   add_ctx->dest.origin.reg, IARG_END);
  } else if (add_ctx->dest.type == OprType::MEM) {
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::add::track_mem_reg,
                   IARG_CONTEXT, IARG_UINT32, add_ctx->src.origin.reg,
                   IARG_MEMORYWRITE_EA, IARG_END);
  } else {
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::add::track_mem_reg,
                   IARG_CONTEXT, IARG_UINT32, add_ctx->src.origin.reg,
                   IARG_UINT32, add_ctx->dest.origin.reg, IARG_END);
  }
}

void handle_mul(INS ins) {
  binary_op::ctx *mul_ctx = binary_op::get_bop_operands(ins);

  if (mul_ctx->src.type == OprType::MEM) {
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::mul::track_mem_reg,
                   IARG_CONTEXT, IARG_MEMORYREAD_EA, IARG_UINT32,
                   mul_ctx->dest.origin.reg, IARG_END);
  } else if (mul_ctx->dest.type == OprType::MEM) {
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::mul::track_mem_reg,
                   IARG_CONTEXT, IARG_UINT32, mul_ctx->src.origin.reg,
                   IARG_MEMORYWRITE_EA, IARG_END);
  } else {
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::mul::track_mem_reg,
                   IARG_CONTEXT, IARG_UINT32, mul_ctx->src.origin.reg,
                   IARG_UINT32, mul_ctx->dest.origin.reg, IARG_END);
  }
}


//
// void handle_div(INS ins, bool is_pop, bool is_reverse) {
//   dst = dst / src
//   handle_non_commut_bop(ins, binary_op::get_bop_operands(ins),
//                         (AFUNPTR)analysis::track_div, is_pop, is_reverse);
// }
//
// void handle_sub(INS ins, bool is_pop, bool is_reverse) {
//   handle_non_commut_bop(ins, binary_op::get_bop_operands(ins),
//                         (AFUNPTR)analysis::track_sub, is_pop, is_reverse);
// }
//
// void handle_sign_change(INS ins) {
//   INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_sch, IARG_END);
// }
//
// void handle_call(INS ins) {
//   ADDRINT callee_addr = INS_Address(ins);
//   INS_InsertCall(ins, IPOINT_BEFORE,
//   (AFUNPTR)analysis::track_call_to_intrinsic,
//                  IARG_BRANCH_TARGET_ADDR, IARG_ADDRINT, callee_addr,
//                  IARG_END);
// }
//
// void handle_ret(INS ins) {
//   ADDRINT callee_addr = INS_Address(ins);
//   INS_InsertCall(ins, IPOINT_BEFORE,
//                  (AFUNPTR)analysis::track_ret_from_intrinsic,
//                  IARG_BRANCH_TARGET_ADDR, IARG_ADDRINT, callee_addr,
//                  IARG_END);
//   return;
// }
} // namespace instrumentation
#endif
