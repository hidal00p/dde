#include "pin_utils.h"
#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <stdio.h>

#include "graph_utils.h"

struct MEM {
  REG reg;
  int64_t disp;

  uint64_t get_effective_addr(CONTEXT *ctx) {
    uint64_t ef_addr;
    PIN_GetContextRegval(ctx, reg, (uint8_t *)&ef_addr);
    ef_addr += disp;
    ef_addr += reg == REG_RIP ? 8 - ef_addr % 8 : 0;
    return ef_addr;
  }
};

namespace binary_op {
typedef union {
  REG reg;
  uint64_t imm;
  MEM mem;
} operand;

enum type { IMM, REG, MEM };
struct ctx {
  type src_type;
  operand src;

  type dest_type;
  operand dest;
};

void show_operand(CONTEXT *ctx, type t, operand opr) {
  switch (t) {
  case type::IMM:
    std::cout << opr.imm;
    break;
  case type::REG:
    std::cout << REG_StringShort(opr.reg);
    break;
  case type::MEM:
    uint64_t eff_addr;
    PIN_GetContextRegval(ctx, opr.mem.reg, (uint8_t *)&eff_addr);
    eff_addr += opr.mem.disp;
    std::cout << "0x" << std::hex << eff_addr;
    break;
  }
}
} // namespace binary_op

namespace analysis {
void track_mem_upd_mov(CONTEXT *ctx, binary_op::ctx *mov_ctx, bool is_pop,
                       ADDRINT ea) {
#ifdef VERBOSE
  show_operand(ctx, mov_ctx->dest_type, mov_ctx->dest);
  std::cout << " <-- ";
  show_operand(ctx, mov_ctx->src_type, mov_ctx->src);
  std::cout << std::endl;
#endif

  if (mov_ctx->src_type == binary_op::type::MEM) {
    uint64_t src_ef_addr = mov_ctx->src.mem.get_effective_addr(ctx);

    if (!mem::is_node_recorded(src_ef_addr)) {
      double value;
      PIN_SafeCopy((void *)&value, (void *)ea, sizeof(double));
      mem::insert_node(src_ef_addr, new node(value));
    }

    if (mov_ctx->dest_type == binary_op::type::MEM) {
      uint64_t dest_ef_addr = mov_ctx->dest.mem.get_effective_addr(ctx);
      mem::write_to_mem(dest_ef_addr, src_ef_addr);
    } else if (mov_ctx->dest_type == binary_op::type::REG) {
      // Checking for pop here is meaningless
      stack::push(mem::expect_node(src_ef_addr));
    } else
      assert(false);
  } else if (mov_ctx->src_type == binary_op::type::REG) {
    assert(mov_ctx->dest_type == binary_op::type::MEM);
    mem::insert_node(mov_ctx->dest.mem.get_effective_addr(ctx), stack::back());
    if (is_pop)
      stack::pop();
  } else
    assert(false);
}

void track_mem_upd_add(CONTEXT *ctx, binary_op::ctx *add_ctx, bool is_pop) {
#ifdef VERBOSE
  show_operand(ctx, add_ctx->dest_type, add_ctx->dest);
  std::cout << " <-- ";
  show_operand(ctx, add_ctx->src_type, add_ctx->src);
  std::cout << " + ";
  show_operand(ctx, add_ctx->dest_type, add_ctx->dest);
  std::cout << std::endl;
#endif

  // It is implied that the 2nd operand is a register

  node *src_node, *dest_node;

  if (add_ctx->src_type == binary_op::type::MEM) {
    assert(!is_pop);
    src_node = mem::expect_node(add_ctx->src.mem.get_effective_addr(ctx));
    dest_node = stack::pop();
    stack::push(new node(
        src_node->value + dest_node->value, 2,
        new node *[] { src_node, dest_node }, transformation::ADD));
  } else if (add_ctx->src_type == binary_op::type::REG) {
    uint8_t src_idx =
        stack::size() - 1 - get_fpu_stack_idx_from_st(add_ctx->src.reg);
    uint8_t dest_idx =
        stack::size() - 1 - get_fpu_stack_idx_from_st(add_ctx->dest.reg);

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

void track_mem_upd_mul(CONTEXT *ctx, binary_op::ctx *mul_ctx, bool is_pop) {
#ifdef VERBOSE
  show_operand(ctx, mul_ctx->dest_type, mul_ctx->dest);
  std::cout << " <-- ";
  show_operand(ctx, mul_ctx->src_type, mul_ctx->src);
  std::cout << " * ";
  show_operand(ctx, mul_ctx->dest_type, mul_ctx->dest);
  std::cout << std::endl;
#endif

  // It is implied that the 2nd operand is a register

  node *src_node, *dest_node;

  if (mul_ctx->src_type == binary_op::type::MEM) {
    assert(!is_pop);
    src_node = mem::expect_node(mul_ctx->src.mem.get_effective_addr(ctx));
    dest_node = stack::pop();
    stack::push(new node(
        src_node->value * dest_node->value, 2,
        new node *[] { src_node, dest_node }, transformation::MUL));
  } else if (mul_ctx->src_type == binary_op::type::REG) {
    uint8_t src_idx =
        stack::size() - 1 - get_fpu_stack_idx_from_st(mul_ctx->src.reg);
    uint8_t dest_idx =
        stack::size() - 1 - get_fpu_stack_idx_from_st(mul_ctx->dest.reg);

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

} // namespace analysis

namespace inst_handler {
binary_op::ctx *handle_bop(INS ins) {
  static const uint8_t SRC_IDX = 1, DEST_IDX = 0;
  binary_op::ctx *bop_ctx = new binary_op::ctx();

  if (INS_OperandIsMemory(ins, DEST_IDX)) {
    bop_ctx->dest_type = binary_op::type::MEM;
    bop_ctx->dest = {
        .mem = {INS_OperandMemoryBaseReg(ins, DEST_IDX),
                (int8_t)INS_OperandMemoryDisplacement(ins, DEST_IDX)}};
  } else if (INS_OperandIsReg(ins, DEST_IDX)) {
    bop_ctx->dest_type = binary_op::type::REG;
    bop_ctx->dest = {.reg = INS_OperandReg(ins, DEST_IDX)};
  } else {
    assert(false);
  }

  if (INS_OperandIsImmediate(ins, SRC_IDX)) {
    bop_ctx->src_type = binary_op::type::IMM;
    bop_ctx->src = {.imm = INS_OperandImmediate(ins, SRC_IDX)};
  } else if (INS_OperandIsReg(ins, SRC_IDX)) {
    bop_ctx->src_type = binary_op::type::REG;
    bop_ctx->src = {.reg = INS_OperandReg(ins, SRC_IDX)};
  } else if (INS_OperandIsMemory(ins, SRC_IDX)) {
    bop_ctx->src_type = binary_op::type::MEM;
    bop_ctx->src = {
        .mem = {INS_OperandMemoryBaseReg(ins, SRC_IDX),
                (int8_t)INS_OperandMemoryDisplacement(ins, SRC_IDX)}};
  } else {
    assert(false);
  }

  return bop_ctx;
}

void handle_mov(INS ins, bool is_pop) {
  binary_op::ctx *mov_ctx = handle_bop(ins);
  INS_IsMemoryRead(ins)
      ? INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_mem_upd_mov,
                       IARG_CONST_CONTEXT, IARG_PTR, mov_ctx, IARG_BOOL, is_pop,
                       IARG_MEMORYREAD_EA, IARG_END)
      : INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_mem_upd_mov,
                       IARG_CONST_CONTEXT, IARG_PTR, mov_ctx, IARG_BOOL, is_pop,
                       IARG_ADDRINT, 5, IARG_END);
}

void handle_load_onto_stack(INS ins) { handle_mov(ins, false); }

void handle_pop_off_stack(INS ins) { handle_mov(ins, true); }

void handle_add(INS ins, bool is_pop = false) {
  binary_op::ctx *add_ctx = handle_bop(ins);
  INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_mem_upd_add,
                 IARG_CONST_CONTEXT, IARG_PTR, add_ctx, IARG_BOOL, is_pop,
                 IARG_END);
}
void handle_mul(INS ins, bool is_pop = false) {
  binary_op::ctx *mul_ctx = handle_bop(ins);
  INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_mem_upd_mul,
                 IARG_CONST_CONTEXT, IARG_PTR, mul_ctx, IARG_BOOL, is_pop,
                 IARG_END);
}
} // namespace inst_handler

VOID instruction(INS ins, VOID *v) {
  if (!is_main_rtn(ins))
    return;

  OPCODE opcode = INS_Opcode(ins);

  if (opcode == XED_ICLASS_FLD) {
    inst_handler::handle_load_onto_stack(ins);
    return;
  }

  if (opcode == XED_ICLASS_FSTP) {
    inst_handler::handle_pop_off_stack(ins);
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
