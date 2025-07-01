#include "dde/graph.h"
#include "dde/handlers.h"
#include "dde/params.h"

#include <cassert>
#include <functional>
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
  if (opcode == XED_ICLASS_MOVSD_XMM || opcode == XED_ICLASS_MOVSD ||
      opcode == XED_ICLASS_MOV || opcode == XED_ICLASS_MOVQ) {
    instrumentation::handle_mov(ins);
    return;
  }

  if (opcode == XED_ICLASS_ADDSD) {
    instrumentation::handle_binary<std::plus<double>, Transformation::ADD>(ins);
    return;
  }

  if (opcode == XED_ICLASS_MULSD) {
    instrumentation::handle_binary<std::multiplies<double>,
                                   Transformation::MUL>(ins);
    return;
  }

  if (opcode == XED_ICLASS_SUBSD) {
    instrumentation::handle_binary<std::minus<double>, Transformation::SUB>(
        ins);
    return;
  }

  if (opcode == XED_ICLASS_DIVSD) {
    instrumentation::handle_binary<std::divides<double>, Transformation::DIV>(
        ins);
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

void final_processing(INT32 code, VOID *v) {
  std::cout << "Mem state:" << std::endl;
  for (const auto &[addr, n] : mem_map) {
    show_node(n, "");
  }

  std::cout << std::endl << "Reg state:" << std::endl;
  for (const auto &[addr, n] : reg_map) {
    show_node(n, "");
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

int main(int argc, char *argv[]) {
  PIN_InitSymbols();
  if (PIN_Init(argc, argv)) {
    return usage();
  }

  RTN_AddInstrumentFunction(routine, 0);
  INS_AddInstrumentFunction(instruction, 0);
  PIN_AddFiniFunction(final_processing, 0);

  PIN_StartProgram();

  return 0;
}
