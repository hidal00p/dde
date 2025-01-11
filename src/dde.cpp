#include "pin_utils.h"
#include <cassert>
#include <iostream>

#include "graph_utils.h"
#include "transform_ctx.h"

namespace analysis {
void track_mem_upd_mov(CONTEXT *ctx, binary_op::ctx *mov_ctx, bool is_pop,
                       ADDRINT ea) {
#ifdef DEBUG
  show_operand(ctx, mov_ctx->dest_type, mov_ctx->dest);
  std::cout << " <-- ";
  show_operand(ctx, mov_ctx->src_type, mov_ctx->src);
  std::cout << std::endl;
#endif

  if (mov_ctx->src.type == OprType::MEM) {

    if (!mem::is_node_recorded(ea)) {
      double value;
      PIN_SafeCopy((void *)&value, (void *)ea, sizeof(double));
      mem::insert_node(ea, new node(value));
    }

    // We anticipate a load onto an FPU stack
    assert(mov_ctx->dest.type == OprType::REGSTR);
    stack::push(mem::expect_node(ea));

  } else if (mov_ctx->src.type == OprType::REGSTR) {

    // We anticipate a pop off stack into memory
    assert(mov_ctx->dest.type == OprType::MEM);
    mem::insert_node(ea, stack::top());

    if (is_pop)
      stack::pop();

  } else
    assert(false);
}

void track_mem_upd_add(CONTEXT *ctx, binary_op::ctx *add_ctx, bool is_pop,
                       ADDRINT ea) {
#ifdef DEBUG
  show_operand(ctx, add_ctx->dest_type, add_ctx->dest);
  std::cout << " <-- ";
  show_operand(ctx, add_ctx->src_type, add_ctx->src);
  std::cout << " + ";
  show_operand(ctx, add_ctx->dest_type, add_ctx->dest);
  std::cout << std::endl;
#endif

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

void track_mem_upd_mul(CONTEXT *ctx, binary_op::ctx *mul_ctx, bool is_pop,
                       ADDRINT ea) {
#ifdef DEBUG
  show_operand(ctx, mul_ctx->dest_type, mul_ctx->dest);
  std::cout << " <-- ";
  show_operand(ctx, mul_ctx->src_type, mul_ctx->src);
  std::cout << " * ";
  show_operand(ctx, mul_ctx->dest_type, mul_ctx->dest);
  std::cout << std::endl;
#endif

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

void track_mem_upd_div(CONTEXT *ctx, binary_op::ctx *div_ctx, bool is_pop,
                       ADDRINT ea) {
#ifdef DEBUG
  show_operand(ctx, mul_ctx->dest_type, mul_ctx->dest);
  std::cout << " <-- ";
  show_operand(ctx, mul_ctx->src_type, mul_ctx->src);
  std::cout << " / ";
  show_operand(ctx, mul_ctx->dest_type, mul_ctx->dest);
  std::cout << std::endl;
#endif

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

void track_mem_upd_sub(CONTEXT *ctx, binary_op::ctx *sub_ctx, bool is_pop,
                       ADDRINT ea) {
#ifdef DEBUG
  show_operand(ctx, mul_ctx->dest_type, mul_ctx->dest);
  std::cout << " <-- ";
  show_operand(ctx, mul_ctx->src_type, mul_ctx->src);
  std::cout << " / ";
  show_operand(ctx, mul_ctx->dest_type, mul_ctx->dest);
  std::cout << std::endl;
#endif

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

} // namespace analysis

namespace inst_handler {
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

void handle_mov(INS ins, bool is_pop = false) {
  binary_op::ctx *mov_ctx = get_bop_operands(ins);

  // FPU-wise memory can either be written to or read from
  // at the same time, these two do not happen as a part of
  // the same op.
  if (mov_ctx->src.type == OprType::MEM)
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_mem_upd_mov,
                   IARG_CONST_CONTEXT, IARG_PTR, mov_ctx, IARG_BOOL, is_pop,
                   IARG_MEMORYREAD_EA, IARG_END);
  else if (mov_ctx->dest.type == OprType::MEM)
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_mem_upd_mov,
                   IARG_CONST_CONTEXT, IARG_PTR, mov_ctx, IARG_BOOL, is_pop,
                   IARG_MEMORYWRITE_EA, IARG_END);
  else
    assert(false);
}

void handle_add(INS ins, bool is_pop = false) {
  handle_bop(ins, get_bop_operands(ins), (AFUNPTR)analysis::track_mem_upd_add,
             is_pop);
}

void handle_mul(INS ins, bool is_pop = false) {
  handle_bop(ins, get_bop_operands(ins), (AFUNPTR)analysis::track_mem_upd_mul,
             is_pop);
}

void handle_div(INS ins, bool is_pop = false) {
  handle_bop(ins, get_bop_operands(ins), (AFUNPTR)analysis::track_mem_upd_div,
             is_pop);
}

void handle_sub(INS ins, bool is_pop = false) {
  handle_bop(ins, get_bop_operands(ins), (AFUNPTR)analysis::track_mem_upd_sub,
             is_pop);
}
} // namespace inst_handler

VOID instruction(INS ins, VOID *v) {
  if (!is_main_rtn(ins))
    return;

  OPCODE opcode = INS_Opcode(ins);

  if (opcode == XED_ICLASS_FLD || opcode == XED_ICLASS_FST) {
    inst_handler::handle_mov(ins);
    return;
  } else if (opcode == XED_ICLASS_FSTP) {
    inst_handler::handle_mov(ins, true);
    return;
  }

  if (opcode == XED_ICLASS_FMUL) {
    inst_handler::handle_mul(ins);
    return;
  } else if (opcode == XED_ICLASS_FMULP) {
    inst_handler::handle_mul(ins, true);
    return;
  }

  if (opcode == XED_ICLASS_FADD) {
    inst_handler::handle_add(ins);
    return;
  } else if (opcode == XED_ICLASS_FADDP) {
    inst_handler::handle_add(ins, true);
    return;
  }

  if (opcode == XED_ICLASS_FDIV) {
    inst_handler::handle_div(ins);
    return;
  } else if (opcode == XED_ICLASS_FDIVP) {
    inst_handler::handle_div(ins, true);
    return;
  }

  if (opcode == XED_ICLASS_FSUB) {
    inst_handler::handle_sub(ins);
    return;
  } else if (opcode == XED_ICLASS_FSUBP) {
    inst_handler::handle_sub(ins, true);
    return;
  }
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 usage() {
  std::cout << "Read Intel Pin instruction manual to learn how to properly "
               "execute pin tools."
            << std::endl;
  return -1;
}

void final_processing(INT32 code, VOID *v) { show_mem_map(); }

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */
/*   argc, argv are the entire command line: pin -t <toolname> -- ...    */
/* ===================================================================== */

int main(int argc, char *argv[]) {
  // Initialize pin and symbols
  PIN_InitSymbols();
  if (PIN_Init(argc, argv))
    return usage();

  // Register Instruction to be called to instrument instructions
  INS_AddInstrumentFunction(instruction, 0);

  // Final graph processing
  PIN_AddFiniFunction(final_processing, 0);

  // Start the program, never returns
  PIN_StartProgram();

  return 0;
}
