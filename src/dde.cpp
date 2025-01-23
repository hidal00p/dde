#include "pin_utils.h"
#include <cassert>
#include <iostream>

#include "graph_utils.h"
#include "transform_ctx.h"

#define SHOW_INST(ins) std::cout << INS_Disassemble(ins) << std::endl;

uint64_t data_start = 0x0;
uint64_t data_end = 0x0;

bool is_data_section(ADDRINT ea) { return ea >= data_start && ea <= data_end; }

struct var_mark_ctx {
  bool is_var_marked = false;
  char var_mark_buffer[1] = {0};
};

var_mark_ctx vm_ctx;

namespace analysis {
void track_mem_upd_mov(CONTEXT *ctx, binary_op::ctx *mov_ctx, bool is_pop,
                       ADDRINT ea) {
  if (mov_ctx->src.type == OprType::MEM) {
    node *n;
    double value;
    PIN_SafeCopy((void *)&value, (void *)ea, sizeof(double));

    if (is_data_section(ea)) {
      n = new node(value);
      if (vm_ctx.is_var_marked) {
        std::string s(1, vm_ctx.var_mark_buffer[0]);
        n->uuid = s;
      }
    } else if (!mem::is_node_recorded(ea)) {
      n = new node(value);
      mem::insert_node(ea, n);
      if (vm_ctx.is_var_marked) {
        std::string s(1, vm_ctx.var_mark_buffer[0]);
        n->uuid = s;
      }
    } else {
      n = mem::expect_node(ea);
    }

    // We anticipate a load onto an FPU stack
    assert(mov_ctx->dest.type == OprType::REGSTR);
    stack::push(n);

  } else if (mov_ctx->src.type == OprType::REGSTR) {

    if (mov_ctx->dest.type == OprType::MEM) {
      mem::insert_node(ea, stack::top());
      if (is_pop)
        stack::pop();
    } else if (mov_ctx->dest.type == OprType::REGSTR) {
      stack::push(stack::top());
    } else
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

void track_mem_upd_add(CONTEXT *ctx, binary_op::ctx *add_ctx, bool is_pop,
                       ADDRINT ea) {
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

void track_mem_upd_sch() {
  node *src_node = stack::pop();
  stack::push(new node(
      -1 * src_node->value, 1, new node *[] { src_node }, transformation::CHS));
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
  else {
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_mem_upd_mov,
                   IARG_CONST_CONTEXT, IARG_PTR, mov_ctx, IARG_BOOL, is_pop,
                   IARG_UINT32, 0, IARG_END);
  }
}

void handle_mov_const(INS ins, uint8_t constant) {
  static const uint8_t DEST_IDX = 0;

  // Manually construct a move context for instructions like FLDZ
  binary_op::ctx *mov_ctx = new binary_op::ctx();
  mov_ctx->dest = {.origin = {.reg = INS_OperandReg(ins, DEST_IDX)},
                   .type = OprType::REGSTR};
  mov_ctx->src = {.origin = {.imm = constant}, .type = OprType::IMM};

  INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_mem_upd_mov,
                 IARG_CONST_CONTEXT, IARG_PTR, mov_ctx, IARG_BOOL, false,
                 IARG_UINT32, 0, IARG_END);
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

void handle_sign_change(INS ins) {
  INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analysis::track_mem_upd_sch,
                 IARG_END);
}
} // namespace inst_handler

bool active_instrumentation = false;

void start_instr() { active_instrumentation = true; }
void stop_instr() { active_instrumentation = false; }

VOID routine(RTN rtn, VOID *v) {
  if (RTN_Name(rtn).find("__dde_start") != std::string::npos) {
    RTN_Open(rtn);
    RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)start_instr, IARG_END);
    RTN_Close(rtn);
    return;
  }

  if (RTN_Name(rtn).find("__dde_stop") != std::string::npos) {
    RTN_Open(rtn);
    RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)stop_instr, IARG_END);
    RTN_Close(rtn);
    return;
  }
}

VOID instruction(INS ins, VOID *v) {
  if (!active_instrumentation)
    return;

  OPCODE opcode = INS_Opcode(ins);

  if (opcode == XED_ICLASS_FLD || opcode == XED_ICLASS_FST) {
    inst_handler::handle_mov(ins);
    return;
  } else if (opcode == XED_ICLASS_FSTP) {
    inst_handler::handle_mov(ins, true);
    return;
  } else if (opcode == XED_ICLASS_FLDZ) {
    inst_handler::handle_mov_const(ins, 0);
    return;
  }

  if (opcode == XED_ICLASS_FCHS) {
    inst_handler::handle_sign_change(ins);
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

namespace test {
void image(IMG img, void *v) {
  if (!IMG_IsMainExecutable(img))
    return;

  for (SEC section = IMG_SecHead(img); SEC_Valid(section);
       section = SEC_Next(section)) {
    if (SEC_Name(section) == ".rodata") {
      data_start = SEC_Address(section);
      data_end = data_start + SEC_Size(section);
      break;
    }
  }
}

void instruction(INS ins, VOID *v) {
  if (!active_instrumentation)
    return;

  std::cout << INS_Disassemble(ins) << std::endl;
}

void start_instr() { active_instrumentation = true; }
void stop_instr() { active_instrumentation = false; }

void start_marking(const char *mark) {
  vm_ctx.is_var_marked = true;
  vm_ctx.var_mark_buffer[0] = mark[0];
}
void stop_marking() {
  vm_ctx.is_var_marked = false;
  vm_ctx.var_mark_buffer[0] = 0;
}

VOID routine(RTN rtn, VOID *v) {
  if (RTN_Name(rtn).find("__dde_start") != std::string::npos) {
    RTN_Open(rtn);
    RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)start_instr, IARG_END);
    RTN_Close(rtn);
    return;
  }

  if (RTN_Name(rtn).find("__dde_stop") != std::string::npos) {
    RTN_Open(rtn);
    RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)stop_instr, IARG_END);
    RTN_Close(rtn);
    return;
  }

  if (RTN_Name(rtn).find("__dde_mark_start") != std::string::npos) {
    RTN_Open(rtn);
    RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)start_marking,
                   IARG_FUNCARG_ENTRYPOINT_VALUE, 0, IARG_END);
    RTN_Close(rtn);
    return;
  }

  if (RTN_Name(rtn).find("__dde_mark_stop") != std::string::npos) {
    RTN_Open(rtn);
    RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)stop_marking, IARG_END);
    RTN_Close(rtn);
    return;
  }
}

struct inst_ctx {
  std::string dis;
  inst_ctx(std::string dis) : dis(dis) {}
};

void in(inst_ctx *ctx) { std::cout << ctx->dis << std::endl; }
void final_processing(INT32 code, VOID *v) {}

void trace(TRACE trc, VOID *v) {
  RTN rtn = TRACE_Rtn(trc);
  if (!RTN_Valid(rtn) || RTN_Name(rtn) != "main")
    return;

  int i = 1;
  for (BBL bbl = TRACE_BblHead(trc); BBL_Valid(bbl); bbl = BBL_Next(bbl)) {
    std::cout << "==== BBL " << i << " ====" << std::endl;
    for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins)) {
      INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)in, IARG_PTR,
                     new inst_ctx(INS_Disassemble(ins)), IARG_END);
      std::cout << INS_Disassemble(ins) << std::endl;
    }
    std::cout << "========" << std::endl;
    i++;
  }
  std::cout << std::endl;
}
} // namespace test

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

#ifndef DEBUG
  // Register Instruction to be called to instrument instructions
  IMG_AddInstrumentFunction(test::image, 0);
  RTN_AddInstrumentFunction(test::routine, 0);
  INS_AddInstrumentFunction(instruction, 0);

  // Final graph processing
  PIN_AddFiniFunction(final_processing, 0);
#else
  RTN_AddInstrumentFunction(test::routine, 0);
  TRACE_AddInstrumentFunction(test::trace, 0);
  // INS_AddInstrumentFunction(test::instruction, 0);
  PIN_AddFiniFunction(test::final_processing, 0);
#endif

  // Start the program, never returns
  PIN_StartProgram();

  return 0;
}
