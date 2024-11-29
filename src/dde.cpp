#include "pin.H"
#include <cassert>
#include <iostream>
#include <map>
#include <string>

#include "graph_utils.h"

struct MEM {
  REG reg;
  int8_t disp;

  uint64_t get_effective_addr(CONTEXT *ctx) {
    uint64_t ef_addr;
    PIN_GetContextRegval(ctx, reg, (uint8_t *)&ef_addr);
    ef_addr += disp;
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

BOOL is_img_main(RTN rtn) {
  if (!RTN_Valid(rtn))
    return false;

  SEC sec = RTN_Sec(rtn);
  if (!SEC_Valid(sec))
    return false;

  IMG img = SEC_Img(sec);
  return IMG_IsMainExecutable(img);
}

BOOL is_img_main(INS ins) { return is_img_main(INS_Rtn(ins)); }

BOOL is_main_rtn(INS ins) {
  RTN rtn = INS_Rtn(ins);
  return RTN_Valid(rtn) && RTN_Name(rtn) == "main";
}

namespace analysis {
void track_mem_upd_mov(CONTEXT *ctx, binary_op::ctx *mov_ctx) {
#ifdef VERBOSE
  show_operand(ctx, mov_ctx->dest_type, mov_ctx->dest);
  std::cout << " <-- ";
  show_operand(ctx, mov_ctx->src_type, mov_ctx->src);
  std::cout << std::endl;
#endif

  if (mov_ctx->dest_type == binary_op::type::MEM) {
    uint64_t ef_addr = mov_ctx->dest.mem.get_effective_addr(ctx);
    if (mov_ctx->src_type == binary_op::type::IMM) {
      mem::insert_node(ef_addr, new node(mov_ctx->src.imm));
    } else if (mov_ctx->src_type == binary_op::type::REG) {
      reg::write_to_mem(mov_ctx->src.reg, ef_addr);
    }
  } else if (mov_ctx->dest_type == binary_op::type::REG) {
    if (mov_ctx->src_type == binary_op::type::IMM) {
      reg::insert_node(mov_ctx->dest.reg, new node(mov_ctx->src.imm));
    } else if (mov_ctx->src_type == binary_op::type::REG) {
      reg::write_to_other_reg(mov_ctx->src.reg, mov_ctx->dest.reg);
    } else if (mov_ctx->src_type == binary_op::type::MEM) {
      mem::write_to_reg(mov_ctx->src.mem.get_effective_addr(ctx),
                        mov_ctx->dest.reg);
    }
  } else {
    assert(false);
  }
}

void track_mem_upd_add(CONTEXT *ctx, binary_op::ctx *add_ctx) {
#ifdef VERBOSE
  show_operand(ctx, add_ctx->dest_type, add_ctx->dest);
  std::cout << " <-- ";
  show_operand(ctx, add_ctx->src_type, add_ctx->src);
  std::cout << " + ";
  show_operand(ctx, add_ctx->dest_type, add_ctx->dest);
  std::cout << std::endl;
#endif

  // It is implied that the 2nd operand is a register
  int sum;
  node *src_node;
  PIN_GetContextRegval(ctx, add_ctx->dest.reg, (uint8_t *)&sum);

  if (add_ctx->src_type == binary_op::type::IMM) {
    src_node = new node(add_ctx->src.imm);
    src_node->is_active = false;
  } else {
    src_node = add_ctx->src_type == binary_op::type::REG
                   ? reg::expect_node(add_ctx->src.reg)
                   : mem::expect_node(add_ctx->src.mem.get_effective_addr(ctx));
  }

  sum += src_node->value;
  node **oprs = new node *[] { reg::expect_node(add_ctx->dest.reg), src_node };
  reg::insert_node(add_ctx->dest.reg,
                   new node(sum, 2, oprs, transformation::ADD));
}

void track_mem_upd_mul(CONTEXT *ctx, binary_op::ctx *mul_ctx) {
#ifdef VERBOSE
  show_operand(ctx, mul_ctx->dest_type, mul_ctx->dest);
  std::cout << " <-- ";
  show_operand(ctx, mul_ctx->src_type, mul_ctx->src);
  std::cout << " * ";
  show_operand(ctx, mul_ctx->dest_type, mul_ctx->dest);
  std::cout << std::endl;
#endif

  // It is implied that the 2nd operand is a register
  int prod;
  node *src_node;
  PIN_GetContextRegval(ctx, mul_ctx->dest.reg, (uint8_t *)&prod);

  if (mul_ctx->src_type == binary_op::type::IMM) {
    src_node = new node(mul_ctx->src.imm);
    src_node->is_active = false;
  } else {
    src_node = mul_ctx->src_type == binary_op::type::REG
                   ? reg::expect_node(mul_ctx->src.reg)
                   : mem::expect_node(mul_ctx->src.mem.get_effective_addr(ctx));
  }

  prod *= src_node->value;
  node **oprs = new node *[] { reg::expect_node(mul_ctx->dest.reg), src_node };
  reg::insert_node(mul_ctx->dest.reg,
                   new node(prod, 2, oprs, transformation::MUL));
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

void handle_mov(INS ins) {
  binary_op::ctx *mov_ctx = handle_bop(ins);

  // Ignore stack pointer stuff
  if (mov_ctx->src_type == binary_op::type::REG && mov_ctx->src.reg == REG_RSP)
    return;

  INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_mem_upd_mov,
                 IARG_CONST_CONTEXT, IARG_PTR, mov_ctx, IARG_END);
}

void handle_add(INS ins) {
  binary_op::ctx *add_ctx = handle_bop(ins);
  INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_mem_upd_add,
                 IARG_CONST_CONTEXT, IARG_PTR, add_ctx, IARG_END);
}
void handle_mul(INS ins) {
  binary_op::ctx *mul_ctx = handle_bop(ins);
  INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_mem_upd_mul,
                 IARG_CONST_CONTEXT, IARG_PTR, mul_ctx, IARG_END);
}
} // namespace inst_handler

VOID instruction(INS ins, VOID *v) {
  if (!is_main_rtn(ins))
    return;

  if (INS_IsMov(ins)) {
    inst_handler::handle_mov(ins);
    return;
  }

  if (INS_Mnemonic(ins) == "IMUL") {
    inst_handler::handle_mul(ins);
    return;
  }

  if (INS_Mnemonic(ins) == "ADD") {
    inst_handler::handle_add(ins);
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
