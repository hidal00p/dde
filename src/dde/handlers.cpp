#include "pin_utils.h"

#include "graph.h"
#include "handlers.h"
#include "params.h"

#include <iostream>

namespace analysis {
void track_reg_mov(CONTEXT *ctx, binary_op::ctx *mov_ctx, ADDRINT ea) {
  // Primarily to handle parameter
  // passing ABI, so we assume all the nodes exist

  if (mov_ctx->src.type == OprType::MEM) {
    assert(mov_ctx->dest.type == OprType::REGSTR);
    mem::write_to_reg(ea, mov_ctx->dest.origin.reg);

  } else if (mov_ctx->src.type == OprType::REGSTR) {
    if (mov_ctx->dest.type == OprType::REGSTR)
      reg::write_to_other_reg(mov_ctx->src.origin.reg,
                              mov_ctx->dest.origin.reg);

    else if (mov_ctx->dest.type == OprType::MEM)
      reg::write_to_mem(mov_ctx->src.origin.reg, ea);

    else
      assert(false);

  } else
    assert(false);
}

void track_fpu_mov(CONTEXT *ctx, binary_op::ctx *mov_ctx, bool is_pop,
                   ADDRINT ea) {
  if (mov_ctx->src.type == OprType::MEM) {
    // We anticipate a load onto an FPU stack
    assert(mov_ctx->dest.type == OprType::REGSTR);

    node *n;
    bool from_data_sec = sec_info.rodata.within_range(ea);
    bool node_recorded = mem::is_node_recorded(ea);

    if (from_data_sec || !node_recorded) {
      double value;
      PIN_SafeCopy((void *)&value, (void *)ea, sizeof(double));
      n = new node(value);

      if (!node_recorded && !from_data_sec)
        mem::insert_node(ea, n);

      if (vm_ctx.is_var_marked) {
        std::string s(1, vm_ctx.var_mark_buffer[0]);
        n->uuid = s;
      }

    } else
      n = mem::expect_node(ea);

    stack::push(n);

  } else if (mov_ctx->src.type == OprType::REGSTR) {

    if (mov_ctx->dest.type == OprType::MEM) {
      mem::insert_node(ea, stack::top());
      if (is_pop)
        stack::pop();

    } else if (mov_ctx->dest.type == OprType::REGSTR)
      stack::push(stack::top());

    else
      assert(false);

  } else {
    assert(mov_ctx->src.type == OprType::IMM &&
           mov_ctx->dest.type == OprType::REGSTR);
    node *n = new node(mov_ctx->src.origin.imm);
    if (vm_ctx.is_var_marked) {
      std::string s(1, vm_ctx.var_mark_buffer[0]);
      n->uuid = s;
    }
    stack::push(n);
  }
}

void track_add(CONTEXT *ctx, binary_op::ctx *add_ctx, bool is_pop, ADDRINT ea) {
  // It is implied that the 2nd operand is a register

  node *src_node, *dest_node;

  if (add_ctx->src.type == OprType::MEM) {
    assert(!is_pop);
    src_node = mem::expect_node(ea);
    dest_node = stack::pop();
    stack::push(new node(
        src_node->value + dest_node->value, 2,
        new node *[] { src_node, dest_node }, transformation::ADD));
  } else if (add_ctx->src.type == OprType::REGSTR) {
    uint8_t src_idx =
        stack::size() - 1 - get_fpu_stack_idx_from_st(add_ctx->src.origin.reg);
    uint8_t dest_idx =
        stack::size() - 1 - get_fpu_stack_idx_from_st(add_ctx->dest.origin.reg);

    src_node = stack::at(src_idx);
    dest_node = stack::at(dest_idx);

    stack::at(dest_idx,
              new node(
                  src_node->value + dest_node->value, 2,
                  new node *[] { src_node, dest_node }, transformation::ADD));

    if (is_pop)
      assert(stack::pop()->uuid == src_node->uuid);
  } else {
    assert(false);
  }
}

void track_mul(CONTEXT *ctx, binary_op::ctx *mul_ctx, bool is_pop, ADDRINT ea) {
  // It is implied that the 2nd operand is a register

  node *src_node, *dest_node;

  if (mul_ctx->src.type == OprType::MEM) {
    assert(!is_pop);
    src_node = mem::expect_node(ea);
    dest_node = stack::pop();
    stack::push(new node(
        src_node->value * dest_node->value, 2,
        new node *[] { src_node, dest_node }, transformation::MUL));
  } else if (mul_ctx->src.type == OprType::REGSTR) {
    uint8_t src_idx =
        stack::size() - 1 - get_fpu_stack_idx_from_st(mul_ctx->src.origin.reg);
    uint8_t dest_idx =
        stack::size() - 1 - get_fpu_stack_idx_from_st(mul_ctx->dest.origin.reg);

    src_node = stack::at(src_idx);
    dest_node = stack::at(dest_idx);

    stack::at(dest_idx,
              new node(
                  src_node->value * dest_node->value, 2,
                  new node *[] { src_node, dest_node }, transformation::MUL));

    if (is_pop)
      assert(stack::pop()->uuid == src_node->uuid);
  } else {
    assert(false);
  }
}

void track_div(CONTEXT *ctx, binary_op::ctx *div_ctx, bool is_pop, ADDRINT ea) {
  // It is implied that the 2nd operand is a register

  node *src_node, *dest_node;

  if (div_ctx->src.type == OprType::MEM) {
    assert(!is_pop);
    src_node = mem::expect_node(ea);
    dest_node = stack::pop();
    stack::push(new node(
        dest_node->value / src_node->value, 2,
        new node *[] { dest_node, src_node }, transformation::DIV));
  } else if (div_ctx->src.type == OprType::REGSTR) {
    uint8_t src_idx =
        stack::size() - 1 - get_fpu_stack_idx_from_st(div_ctx->src.origin.reg);
    uint8_t dest_idx =
        stack::size() - 1 - get_fpu_stack_idx_from_st(div_ctx->dest.origin.reg);

    src_node = stack::at(src_idx);
    dest_node = stack::at(dest_idx);

    stack::at(dest_idx,
              new node(
                  dest_node->value / src_node->value, 2,
                  new node *[] { dest_node, src_node }, transformation::DIV));

    if (is_pop)
      assert(stack::pop()->uuid == src_node->uuid);
  } else {
    assert(false);
  }
}

void track_sub(CONTEXT *ctx, binary_op::ctx *sub_ctx, bool is_pop, ADDRINT ea) {
  // It is implied that the 2nd operand is a register

  node *src_node, *dest_node;

  if (sub_ctx->src.type == OprType::MEM) {
    assert(!is_pop);
    src_node = mem::expect_node(ea);
    dest_node = stack::pop();
    stack::push(new node(
        dest_node->value - src_node->value, 2,
        new node *[] { dest_node, src_node }, transformation::SUB));
  } else if (sub_ctx->src.type == OprType::REGSTR) {
    uint8_t src_idx =
        stack::size() - 1 - get_fpu_stack_idx_from_st(sub_ctx->src.origin.reg);
    uint8_t dest_idx =
        stack::size() - 1 - get_fpu_stack_idx_from_st(sub_ctx->dest.origin.reg);

    src_node = stack::at(src_idx);
    dest_node = stack::at(dest_idx);

    stack::at(dest_idx,
              new node(
                  dest_node->value - src_node->value, 2,
                  new node *[] { dest_node, src_node }, transformation::SUB));

    if (is_pop)
      assert(stack::pop()->uuid == src_node->uuid);
  } else {
    assert(false);
  }
}

void track_sch() {
  node *src_node = stack::pop();
  stack::push(new node(
      -1 * src_node->value, 1, new node *[] { src_node }, transformation::CHS));
}

} // namespace analysis

namespace instrumentation {
binary_op::ctx *get_bop_operands(INS ins) {
  static const uint8_t SRC_IDX = 1, DEST_IDX = 0;
  binary_op::ctx *bop_ctx = new binary_op::ctx();

  if (INS_OperandIsMemory(ins, DEST_IDX)) {
    bop_ctx->dest = {
        .origin = {.mem = {INS_OperandMemoryBaseReg(ins, DEST_IDX),
                           (int8_t)INS_OperandMemoryDisplacement(ins,
                                                                 DEST_IDX)}},
        .type = OprType::MEM,
    };
  } else if (INS_OperandIsReg(ins, DEST_IDX)) {
    bop_ctx->dest = {.origin = {.reg = INS_OperandReg(ins, DEST_IDX)},
                     .type = OprType::REGSTR};
  } else {
    assert(false);
  }

  if (INS_OperandIsImmediate(ins, SRC_IDX)) {
    bop_ctx->src = {.origin = {.imm = INS_OperandImmediate(ins, SRC_IDX)},
                    .type = OprType::IMM};
  } else if (INS_OperandIsReg(ins, SRC_IDX)) {
    bop_ctx->src = {.origin = {.reg = INS_OperandReg(ins, SRC_IDX)},
                    .type = OprType::REGSTR};
  } else if (INS_OperandIsMemory(ins, SRC_IDX)) {
    bop_ctx->src = {.origin = {.mem = {INS_OperandMemoryBaseReg(ins, SRC_IDX),
                                       (int8_t)INS_OperandMemoryDisplacement(
                                           ins, SRC_IDX)}},
                    .type = OprType::MEM};
  } else {
    assert(false);
  }

  return bop_ctx;
}

void handle_bop(INS ins, binary_op::ctx *bop_ctx, AFUNPTR func, bool is_pop) {

  // For FP instructions we assume that memory can only be a source operand.
  // This is a valid assumption, since all the FPU arithmetic operations are
  // proxied via the FPU stack registers.
  if (bop_ctx->src.type == OprType::MEM)
    INS_InsertCall(ins, IPOINT_BEFORE, func, IARG_CONST_CONTEXT, IARG_PTR,
                   bop_ctx, IARG_BOOL, is_pop, IARG_MEMORYREAD_EA, IARG_END);
  else if (bop_ctx->src.type == OprType::REGSTR)
    INS_InsertCall(ins, IPOINT_BEFORE, func, IARG_CONST_CONTEXT, IARG_PTR,
                   bop_ctx, IARG_BOOL, is_pop, IARG_ADDRINT, 0, IARG_END);
  else
    assert(false);
}

void handle_reg_mov(INS ins) {
  binary_op::ctx *mov_ctx = get_bop_operands(ins);
  // We assume that there is no mem to mem IO
  if (mov_ctx->src.type == OprType::MEM)
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_reg_mov,
                   IARG_CONST_CONTEXT, IARG_PTR, mov_ctx, IARG_MEMORYREAD_EA,
                   IARG_END);

  else if (mov_ctx->dest.type == OprType::MEM) {
    REG src_reg = mov_ctx->src.origin.reg;
    if (src_reg == REG_RBP || src_reg == REG_RDI)
      return;

    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_reg_mov,
                   IARG_CONST_CONTEXT, IARG_PTR, mov_ctx, IARG_MEMORYWRITE_EA,
                   IARG_END);

  } else {
    // Implicit reg to reg
    REG dest_reg = mov_ctx->dest.origin.reg;
    if (dest_reg == REG_RBP || dest_reg == REG_RDI || dest_reg == REG_EAX)
      return;

    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_reg_mov,
                   IARG_CONST_CONTEXT, IARG_PTR, mov_ctx, IARG_UINT32, 0,
                   IARG_END);
  }
}

void handle_fpu_mov(INS ins, bool is_pop) {
  binary_op::ctx *mov_ctx = get_bop_operands(ins);

  // FPU-wise memory can either be written to or read from
  // at the same time, these two do not happen as a part of
  // the same op.
  if (mov_ctx->src.type == OprType::MEM)
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_fpu_mov,
                   IARG_CONST_CONTEXT, IARG_PTR, mov_ctx, IARG_BOOL, is_pop,
                   IARG_MEMORYREAD_EA, IARG_END);

  else if (mov_ctx->dest.type == OprType::MEM)
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_fpu_mov,
                   IARG_CONST_CONTEXT, IARG_PTR, mov_ctx, IARG_BOOL, is_pop,
                   IARG_MEMORYWRITE_EA, IARG_END);

  else
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_fpu_mov,
                   IARG_CONST_CONTEXT, IARG_PTR, mov_ctx, IARG_BOOL, is_pop,
                   IARG_UINT32, 0, IARG_END);
}

void handle_fpu_const_load(INS ins, uint8_t constant) {
  static const uint8_t DEST_IDX = 0;

  // Manually construct a move context for instructions like FLDZ
  binary_op::ctx *mov_ctx = new binary_op::ctx();
  mov_ctx->dest = {.origin = {.reg = INS_OperandReg(ins, DEST_IDX)},
                   .type = OprType::REGSTR};
  mov_ctx->src = {.origin = {.imm = constant}, .type = OprType::IMM};

  INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_fpu_mov,
                 IARG_CONST_CONTEXT, IARG_PTR, mov_ctx, IARG_BOOL, false,
                 IARG_UINT32, 0, IARG_END);
}

void handle_add(INS ins, bool is_pop) {
  handle_bop(ins, get_bop_operands(ins), (AFUNPTR)analysis::track_add, is_pop);
}

void handle_mul(INS ins, bool is_pop) {
  handle_bop(ins, get_bop_operands(ins), (AFUNPTR)analysis::track_mul, is_pop);
}

void handle_div(INS ins, bool is_pop) {
  handle_bop(ins, get_bop_operands(ins), (AFUNPTR)analysis::track_div, is_pop);
}

void handle_sub(INS ins, bool is_pop) {
  handle_bop(ins, get_bop_operands(ins), (AFUNPTR)analysis::track_sub, is_pop);
}

void handle_sign_change(INS ins) {
  INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_sch, IARG_END);
}
} // namespace instrumentation
