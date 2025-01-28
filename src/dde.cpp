#include "dde/pin_utils.h"

#include "dde/graph.h"
#include "dde/handlers.h"
#include "dde/params.h"

#include <cassert>
#include <iostream>

VOID instruction(INS ins, VOID *v) {
  if (!dde_state.to_instrument)
    return;

  OPCODE opcode = INS_Opcode(ins);

  // Register read and write
  if (opcode == XED_ICLASS_MOVSD_XMM || opcode == XED_ICLASS_MOV ||
      opcode == XED_ICLASS_MOVAPD || opcode == XED_ICLASS_MOVQ) {
    instrumentation::handle_reg_mov(ins);
    return;
  }

  // FPU stack read and write
  if (opcode == XED_ICLASS_FLD || opcode == XED_ICLASS_FST) {
    instrumentation::handle_fpu_mov(ins);
    return;
  } else if (opcode == XED_ICLASS_FSTP) {
    instrumentation::handle_fpu_mov(ins, true);
    return;
  } else if (opcode == XED_ICLASS_FLDZ) {
    instrumentation::handle_fpu_const_load(ins, 0);
    return;
  } else if (opcode == XED_ICLASS_FLD1) {
    instrumentation::handle_fpu_const_load(ins, 1.0);
    return;
  }

  // We anticipate all arithmetic to
  // interact with the FPU stack.
  if (opcode == XED_ICLASS_FCHS) {
    instrumentation::handle_sign_change(ins);
    return;
  }

  if (opcode == XED_ICLASS_FMUL) {
    instrumentation::handle_mul(ins);
    return;
  } else if (opcode == XED_ICLASS_FMULP) {
    instrumentation::handle_mul(ins, true);
    return;
  }

  if (opcode == XED_ICLASS_FADD) {
    instrumentation::handle_add(ins);
    return;
  } else if (opcode == XED_ICLASS_FADDP) {
    instrumentation::handle_add(ins, true);
    return;
  }

  if (opcode == XED_ICLASS_FDIV) {
    instrumentation::handle_div(ins);
    return;
  } else if (opcode == XED_ICLASS_FDIVP) {
    instrumentation::handle_div(ins, true);
    return;
  } else if (opcode == XED_ICLASS_FDIVR) {
    instrumentation::handle_div(ins, false, true);
    return;
  } else if (opcode == XED_ICLASS_FDIVR) {
    instrumentation::handle_div(ins, true, true);
    return;
  }

  if (opcode == XED_ICLASS_FSUB) {
    instrumentation::handle_sub(ins);
    return;
  } else if (opcode == XED_ICLASS_FSUBP) {
    instrumentation::handle_sub(ins, true);
    return;
  } else if (opcode == XED_ICLASS_FSUBR) {
    instrumentation::handle_sub(ins, false, true);
    return;
  } else if (opcode == XED_ICLASS_FSUBRP) {
    instrumentation::handle_sub(ins, true, true);
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

void final_processing(INT32 code, VOID *v) {}

void image(IMG img, void *v) {
  if (!IMG_IsMainExecutable(img))
    return;

  for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec)) {
    if (SEC_Name(sec) == ".rodata") {
      sec_info.rodata.start = SEC_Address(sec);
      sec_info.rodata.end = sec_info.rodata.start + SEC_Size(sec);
    }

    if (SEC_Name(sec) == ".data") {
      sec_info.data.start = SEC_Address(sec);
      sec_info.data.end = sec_info.rodata.start + SEC_Size(sec);
    }
  }
}

void dump_graph() {
  show_mem_map();
  clean_mem_map();
}

void start_instr() { dde_state.to_instrument = true; }
void stop_instr() { dde_state.to_instrument = false; }

void start_marking(const char *mark, bool output) {
  vm_ctx.is_var_marked = true;
  vm_ctx.output = output;
  vm_ctx.var_mark_buffer[0] = mark[0];
}

void stop_marking() {
  vm_ctx.is_var_marked = false;
  vm_ctx.output = false;
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
                   IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                   IARG_FUNCARG_ENTRYPOINT_VALUE, 1, IARG_END);
    RTN_Close(rtn);
    return;
  }

  if (RTN_Name(rtn).find("__dde_mark_stop") != std::string::npos) {
    RTN_Open(rtn);
    RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)stop_marking, IARG_END);
    RTN_Close(rtn);
    return;
  }

  if (RTN_Name(rtn).find("__dde_dump_graph") != std::string::npos) {
    RTN_Open(rtn);
    RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)dump_graph, IARG_END);
    RTN_Close(rtn);
    return;
  }
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */
/*   argc, argv are the entire command line: pin -t <toolname> -- ...    */
/* ===================================================================== */

#define NDEBUG
namespace test {
void instruction(INS ins, void *v) {
  if (!dde_state.to_instrument)
    return;

  OPCODE opcode = INS_Opcode(ins);
  if (opcode == XED_ICLASS_FLD)
    show_ins(ins);
}
} // namespace test

int main(int argc, char *argv[]) {
  // Initialize pin and symbols
  PIN_InitSymbols();
  if (PIN_Init(argc, argv))
    return usage();

#ifndef DEBUG
  // Register Instruction to be called to instrument instructions
  IMG_AddInstrumentFunction(image, 0);
  RTN_AddInstrumentFunction(routine, 0);
  INS_AddInstrumentFunction(instruction, 0);

  // Final graph processing
  PIN_AddFiniFunction(final_processing, 0);
#else
  RTN_AddInstrumentFunction(routine, 0);
  INS_AddInstrumentFunction(test::instruction, 0);
  PIN_AddFiniFunction(final_processing, 0);
#endif

  // Start the program, never returns
  PIN_StartProgram();

  return 0;
}
