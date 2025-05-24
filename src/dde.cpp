#include "dde/graph.h"
#include "dde/handlers.h"
#include "dde/params.h"

#include <cassert>
#include <iostream>

VOID instruction(INS ins, VOID *v) {
  if (INS_IsRet(ins) && !call_pair.empty()) {
    instrumentation::handle_ret(ins);
    return;
  }

  if (!dde_state.to_instrument)
    return;

  if (INS_IsCall(ins)) {
    instrumentation::handle_call(ins);
    return;
  }

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
    std::string sec_name = SEC_Name(sec);

    if (sec_name == ".rodata") {
      sec_info.rodata.start = SEC_Address(sec);
      sec_info.rodata.end = sec_info.rodata.start + SEC_Size(sec);
    }

    if (sec_name == ".data") {
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
  var_marking_ctx.is_var_marked = true;
  var_marking_ctx.output = output;
  var_marking_ctx.mark = mark; // this should be a deep copy
}

void stop_marking() {
  var_marking_ctx.is_var_marked = false;
  var_marking_ctx.output = false;
  var_marking_ctx.mark.clear();
}

VOID routine(RTN rtn, VOID *v) {
  std::string rtn_name = RTN_Name(rtn);

  bool dde_namespace = rtn_name.find("dde") != std::string::npos;
  if (!dde_namespace)
    return;

  bool dde_start = rtn_name.find("start") != std::string::npos;
  if (dde_start) {
    RTN_Open(rtn);
    RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)start_instr, IARG_END);
    RTN_Close(rtn);
    return;
  }

  bool dde_stop = rtn_name.find("stop") != std::string::npos;
  if (dde_stop) {
    RTN_Open(rtn);
    RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)stop_instr, IARG_END);
    RTN_Close(rtn);
    return;
  }

  bool dde_endvar = rtn_name.find("endvar") != std::string::npos;
  if (dde_endvar) {
    RTN_Open(rtn);
    RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)stop_marking, IARG_END);
    RTN_Close(rtn);
    return;
  }

  bool dde_var = rtn_name.find("var") != std::string::npos;
  if (dde_var) {
    RTN_Open(rtn);
    RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)start_marking,
                   IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                   IARG_FUNCARG_ENTRYPOINT_VALUE, 1, IARG_END);
    RTN_Close(rtn);
    return;
  }

  bool dde_dump_graph = rtn_name.find("dump_graph") != std::string::npos;
  if (dde_dump_graph) {
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

bool parse_args(int argc, char *argv[]) {
  constexpr uint8_t pin_reserved_args = 7;

  if (argc == pin_reserved_args)
    return false;

  if (argc > pin_reserved_args + 1)
    return true;

  std::string arg_path(argv[pin_reserved_args]);
  graph_path = arg_path;

  return false;
}

int main(int argc, char *argv[]) {
  // Initialize pin and symbols
  PIN_InitSymbols();
  if (PIN_Init(argc, argv))
    return usage();

  // Register Instruction to be called to instrument instructions
  IMG_AddInstrumentFunction(image, 0);
  RTN_AddInstrumentFunction(routine, 0);
  INS_AddInstrumentFunction(instruction, 0);
  PIN_AddFiniFunction(final_processing, 0);

  // Start the program, never returns
  PIN_StartProgram();

  return 0;
}
