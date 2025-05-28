#include "pin_utils.h"

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
void track_reg_mov(binary_op::ctx *mov_ctx, ADDRINT ea) {
  // Primarily to handle parameter passing with
  // x86 calling convention, so we assume all the nodes exist.

  if (mov_ctx->src.type == OprType::MEM) {
    assert(mov_ctx->dest.type == OprType::REGSTR);
    mem::write_to_reg(ea, mov_ctx->dest.origin.reg);

  } else if (mov_ctx->src.type == OprType::REGSTR) {
    if (mov_ctx->dest.type == OprType::REGSTR) {
      reg::write_to_other_reg(mov_ctx->src.origin.reg,
                              mov_ctx->dest.origin.reg);
    }

    else if (mov_ctx->dest.type == OprType::MEM) {
      if (mem::is_node_recorded(ea) && mem::expect_node(ea)->output) {
        reg::expect_node(mov_ctx->src.origin.reg)->uuid =
            mem::expect_node(ea)->uuid;
        reg::expect_node(mov_ctx->src.origin.reg)->output = true;
      }

      reg::write_to_mem(mov_ctx->src.origin.reg, ea);
    }

    else {
      assert(false);
    }

  } else {
    assert(false);
  }
}

void track_fpu_mov(binary_op::ctx *mov_ctx, bool is_pop, ADDRINT ea) {
  if (mov_ctx->src.type == OprType::MEM) {
    // We anticipate a load onto an FPU stack
    assert(mov_ctx->dest.type == OprType::REGSTR);

    NodePtr n;
    bool from_data_sec = sec_info.rodata.within_range(ea);
    bool node_recorded = mem::is_node_recorded(ea);

    if (from_data_sec || !node_recorded) {
      double value;
      PIN_SafeCopy((void *)&value, (void *)ea, sizeof(double));
      n = std::make_shared<Node>(value);

      if (!node_recorded && !from_data_sec) {
        mem::insert_node(ea, n);
      }

      if (var_marking_ctx.is_var_marked) {
        n->uuid = var_marking_ctx.mark;
        n->output = var_marking_ctx.output;
      }

    } else {
      n = mem::expect_node(ea);
    }

    stack::push(n);

  } else if (mov_ctx->src.type == OprType::REGSTR) {

    if (mov_ctx->dest.type == OprType::MEM) {

      if (mem::is_node_recorded(ea) && mem::expect_node(ea)->output) {
        stack::top()->uuid = mem::expect_node(ea)->uuid;
        stack::top()->output = true;
      }

      mem::insert_node(ea, stack::top());
      if (is_pop) {
        stack::pop();
      }

    } else if (mov_ctx->dest.type == OprType::REGSTR) {
      stack::push(stack::top());
    }

    else {
      assert(false);
    }

  } else {
    assert(mov_ctx->src.type == OprType::IMM &&
           mov_ctx->dest.type == OprType::REGSTR);
    NodePtr n = std::make_shared<Node>(mov_ctx->src.origin.imm);

    if (var_marking_ctx.is_var_marked) {
      n->uuid = var_marking_ctx.mark;
      n->output = var_marking_ctx.output;
    }

    stack::push(n);
  }
}

void track_add(binary_op::ctx *add_ctx, bool is_pop, ADDRINT ea) {
  // It is implied that the 2nd operand is a register

  NodePtr src_node, dest_node;

  if (add_ctx->src.type == OprType::MEM) {
    assert(!is_pop);

    src_node = mem::expect_node(ea);
    dest_node = stack::pop();

    NodePtr new_node = std::make_shared<Node>(
        src_node->value + dest_node->value, (NodePtrVec){src_node, dest_node},
        transformation::ADD);
    stack::push(new_node);

  } else if (add_ctx->src.type == OprType::REGSTR) {
    uint8_t src_idx =
        stack::size() - 1 - get_fpu_stack_idx_from_st(add_ctx->src.origin.reg);
    uint8_t dest_idx =
        stack::size() - 1 - get_fpu_stack_idx_from_st(add_ctx->dest.origin.reg);

    src_node = stack::at(src_idx);
    dest_node = stack::at(dest_idx);

    NodePtr new_node = std::make_shared<Node>(
        src_node->value + dest_node->value, (NodePtrVec){src_node, dest_node},
        transformation::ADD);
    stack::at(dest_idx, new_node);

    if (is_pop) {
      assert(stack::pop()->uuid == src_node->uuid);
    }
  } else {
    assert(false);
  }
}

void track_mul(binary_op::ctx *mul_ctx, bool is_pop, ADDRINT ea) {
  // It is implied that the 2nd operand is a register

  NodePtr src_node, dest_node;

  if (mul_ctx->src.type == OprType::MEM) {
    assert(!is_pop);

    src_node = mem::expect_node(ea);
    dest_node = stack::pop();

    NodePtr new_node = std::make_shared<Node>(
        src_node->value * dest_node->value, (NodePtrVec){src_node, dest_node},
        transformation::MUL);
    stack::push(new_node);

  } else if (mul_ctx->src.type == OprType::REGSTR) {
    uint8_t src_idx =
        stack::size() - 1 - get_fpu_stack_idx_from_st(mul_ctx->src.origin.reg);
    uint8_t dest_idx =
        stack::size() - 1 - get_fpu_stack_idx_from_st(mul_ctx->dest.origin.reg);

    src_node = stack::at(src_idx);
    dest_node = stack::at(dest_idx);

    NodePtr new_node = std::make_shared<Node>(
        src_node->value * dest_node->value, (NodePtrVec){src_node, dest_node},
        transformation::MUL);

    stack::at(dest_idx, new_node); // this does not make sense. I could
                                   // just compute derivatives right here.

    if (is_pop) {
      assert(stack::pop()->uuid == src_node->uuid);
    }
  } else {
    assert(false);
  }
}

void track_div(binary_op::ctx *div_ctx, bool is_pop, bool is_reverse,
               ADDRINT ea) {
  // It is implied that the 2nd operand is a register

  NodePtr src_node, dest_node;

  if (div_ctx->src.type == OprType::MEM) {
    assert(!is_pop);
    src_node = mem::expect_node(ea);
    dest_node = stack::pop();
    double value = is_reverse ? src_node->value / dest_node->value
                              : dest_node->value / src_node->value;

    NodePtrVec operands;

    if (is_reverse) {
      operands = {src_node, dest_node};
    } else {
      operands = {dest_node, src_node};
    }

    NodePtr new_node =
        std::make_shared<Node>(value, operands, transformation::DIV);
    stack::push(new_node);

  } else if (div_ctx->src.type == OprType::REGSTR) {
    uint8_t src_idx =
        stack::size() - 1 - get_fpu_stack_idx_from_st(div_ctx->src.origin.reg);
    uint8_t dest_idx =
        stack::size() - 1 - get_fpu_stack_idx_from_st(div_ctx->dest.origin.reg);

    src_node = stack::at(src_idx);
    dest_node = stack::at(dest_idx);
    double value = is_reverse ? src_node->value / dest_node->value
                              : dest_node->value / src_node->value;

    NodePtrVec operands;

    if (is_reverse) {
      operands = {src_node, dest_node};
    } else {
      operands = {dest_node, src_node};
    }

    NodePtr new_node =
        std::make_shared<Node>(value, operands, transformation::DIV);
    stack::at(dest_idx, new_node);

    if (is_pop) {
      assert(stack::pop()->uuid == src_node->uuid);
    }
  } else {
    assert(false);
  }
}

void track_sub(binary_op::ctx *sub_ctx, bool is_pop, bool is_reverse,
               ADDRINT ea) {
  // It is implied that the 2nd operand is a register

  NodePtr src_node, dest_node;

  if (sub_ctx->src.type == OprType::MEM) {
    assert(!is_pop);
    src_node = mem::expect_node(ea);
    dest_node = stack::pop();
    double value = is_reverse ? src_node->value - dest_node->value
                              : dest_node->value - src_node->value;

    NodePtrVec operands;

    if (is_reverse) {
      operands = {src_node, dest_node};
    } else {
      operands = {dest_node, src_node};
    }

    NodePtr new_node =
        std::make_shared<Node>(value, operands, transformation::SUB);
    stack::push(new_node);

  } else if (sub_ctx->src.type == OprType::REGSTR) {
    uint8_t src_idx =
        stack::size() - 1 - get_fpu_stack_idx_from_st(sub_ctx->src.origin.reg);
    uint8_t dest_idx =
        stack::size() - 1 - get_fpu_stack_idx_from_st(sub_ctx->dest.origin.reg);

    src_node = stack::at(src_idx);
    dest_node = stack::at(dest_idx);

    double value = is_reverse ? src_node->value - dest_node->value
                              : dest_node->value - src_node->value;
    NodePtrVec operands;

    if (is_reverse) {
      operands = {src_node, dest_node};
    } else {
      operands = {dest_node, src_node};
    }

    NodePtr new_node =
        std::make_shared<Node>(value, operands, transformation::SUB);
    stack::at(dest_idx, new_node);

    if (is_pop) {
      assert(stack::pop()->uuid == src_node->uuid);
    }
  } else {
    assert(false);
  }
}

void track_sch() {
  NodePtr src_node = stack::pop();
  NodePtr new_node(
      new Node(-1 * src_node->value, {src_node}, transformation::CHS));
  stack::push(new_node);
}

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

  NodePtr n = reg::expect_node(REG_XMM0);
  NodePtr y = std::make_shared<Node>(intr.intrinsic_call(n->value),
                                     (NodePtrVec){n}, intr.transf);

  reg::insert_node(REG_XMM0, y);
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
} // namespace analysis

#ifndef TEST_MODE
namespace instrumentation {
void handle_commut_bop(INS ins, binary_op::ctx *bop_ctx, AFUNPTR func,
                       bool is_pop) {

  // For FP instructions we assume that memory can only be a source operand.
  // This is a valid assumption, since all the FPU arithmetic operations are
  // proxied via the FPU stack registers.
  if (bop_ctx->src.type == OprType::MEM) {
    INS_InsertCall(ins, IPOINT_BEFORE, func, IARG_PTR, bop_ctx, IARG_BOOL,
                   is_pop, IARG_MEMORYREAD_EA, IARG_END);
  } else if (bop_ctx->src.type == OprType::REGSTR) {
    INS_InsertCall(ins, IPOINT_BEFORE, func, IARG_PTR, bop_ctx, IARG_BOOL,
                   is_pop, IARG_ADDRINT, 0, IARG_END);
  } else {
    assert(false);
  }
}

void handle_non_commut_bop(INS ins, binary_op::ctx *bop_ctx, AFUNPTR func,
                           bool is_pop, bool is_reverse) {

  // For FP instructions we assume that memory can only be a source operand.
  // This is a valid assumption, since all the FPU arithmetic operations are
  // proxied via the FPU stack registers.
  if (bop_ctx->src.type == OprType::MEM) {
    INS_InsertCall(ins, IPOINT_BEFORE, func, IARG_PTR, bop_ctx, IARG_BOOL,
                   is_pop, IARG_BOOL, is_reverse, IARG_MEMORYREAD_EA, IARG_END);
  } else if (bop_ctx->src.type == OprType::REGSTR) {
    INS_InsertCall(ins, IPOINT_BEFORE, func, IARG_PTR, bop_ctx, IARG_BOOL,
                   is_pop, IARG_BOOL, is_reverse, IARG_ADDRINT, 0, IARG_END);
  } else {
    assert(false);
  }
}

void handle_reg_mov(INS ins) {
  binary_op::ctx *mov_ctx = binary_op::get_bop_operands(ins);
  // We assume that there is no mem to mem IO
  if (mov_ctx->src.type == OprType::MEM) {
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_reg_mov,
                   IARG_PTR, mov_ctx, IARG_MEMORYREAD_EA, IARG_END);
  } else if (mov_ctx->dest.type == OprType::MEM) {
    REG src_reg = mov_ctx->src.origin.reg;

    if (!is_abi_reg(src_reg)) {
      return;
    }

    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_reg_mov,
                   IARG_PTR, mov_ctx, IARG_MEMORYWRITE_EA, IARG_END);

  } else {
    // Implicit reg to reg
    REG dest_reg = mov_ctx->dest.origin.reg;
    REG src_reg = mov_ctx->src.origin.reg;

    if (!is_abi_reg(dest_reg) || !is_abi_reg(src_reg)) {
      return;
    }

    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_reg_mov,
                   IARG_PTR, mov_ctx, IARG_UINT32, 0, IARG_END);
  }
}

void handle_fpu_mov(INS ins, bool is_pop) {
  binary_op::ctx *mov_ctx = binary_op::get_bop_operands(ins);

  // FPU-wise memory can either be written to or read from
  // at the same time, these two do not happen as a part of
  // the same op.
  if (mov_ctx->src.type == OprType::MEM) {
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_fpu_mov,
                   IARG_PTR, mov_ctx, IARG_BOOL, is_pop, IARG_MEMORYREAD_EA,
                   IARG_END);
  } else if (mov_ctx->dest.type == OprType::MEM) {
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_fpu_mov,
                   IARG_PTR, mov_ctx, IARG_BOOL, is_pop, IARG_MEMORYWRITE_EA,
                   IARG_END);
  } else {
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_fpu_mov,
                   IARG_PTR, mov_ctx, IARG_BOOL, is_pop, IARG_UINT32, 0,
                   IARG_END);
  }
}

void handle_fpu_const_load(INS ins, uint8_t constant) {
  constexpr uint8_t DEST_IDX = 0;

  // Manually construct a move context for instructions like FLDZ
  binary_op::ctx *mov_ctx = new binary_op::ctx();
  mov_ctx->dest = {.origin = {.reg = INS_OperandReg(ins, DEST_IDX)},
                   .type = OprType::REGSTR};
  mov_ctx->src = {.origin = {.imm = constant}, .type = OprType::IMM};

  INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_fpu_mov, IARG_PTR,
                 mov_ctx, IARG_BOOL, false, IARG_UINT32, 0, IARG_END);
}

void handle_add(INS ins, bool is_pop) {
  handle_commut_bop(ins, binary_op::get_bop_operands(ins),
                    (AFUNPTR)analysis::track_add, is_pop);
}

void handle_mul(INS ins, bool is_pop) {
  handle_commut_bop(ins, binary_op::get_bop_operands(ins),
                    (AFUNPTR)analysis::track_mul, is_pop);
}

void handle_div(INS ins, bool is_pop, bool is_reverse) {
  handle_non_commut_bop(ins, binary_op::get_bop_operands(ins),
                        (AFUNPTR)analysis::track_div, is_pop, is_reverse);
}

void handle_sub(INS ins, bool is_pop, bool is_reverse) {
  handle_non_commut_bop(ins, binary_op::get_bop_operands(ins),
                        (AFUNPTR)analysis::track_sub, is_pop, is_reverse);
}

void handle_sign_change(INS ins) {
  INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_sch, IARG_END);
}

void handle_call(INS ins) {
  ADDRINT callee_addr = INS_Address(ins);
  INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_call_to_intrinsic,
                 IARG_BRANCH_TARGET_ADDR, IARG_ADDRINT, callee_addr, IARG_END);
}

void handle_ret(INS ins) {
  ADDRINT callee_addr = INS_Address(ins);
  INS_InsertCall(ins, IPOINT_BEFORE,
                 (AFUNPTR)analysis::track_ret_from_intrinsic,
                 IARG_BRANCH_TARGET_ADDR, IARG_ADDRINT, callee_addr, IARG_END);
  return;
}
} // namespace instrumentation
#endif
