#include "dde/graph.h"
#include "dde/handlers.h"
#include "dde/params.h"

#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>

VOID instruction(INS ins, VOID *v) {
  if (INS_IsRet(ins) && !call_pair.empty()) {
    instrumentation::handle_ret(ins);
    return;
  }

  if (dde_state.within_instrinsic)
    return;

  if (INS_IsCall(ins)) {
    instrumentation::handle_call(ins);
    return;
  }

  OPCODE opcode = INS_Opcode(ins);
  // Register read and write
  if (opcode == XED_ICLASS_MOVSD_XMM || opcode == XED_ICLASS_MOVSD ||
      opcode == XED_ICLASS_MOV || opcode == XED_ICLASS_MOVQ ||
      opcode == XED_ICLASS_MOVAPD) {
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

  if (INS_OperandCount(ins) == 2 && INS_OperandIsReg(ins, DEST_IDX)) {
    instrumentation::handle_clear_reg(ins);
    return;
  }
}

VOID trace(TRACE tr, VOID *v) {
  if (!dde_state.instr_active) {
    return;
  }

  for (BBL bbl = TRACE_BblHead(tr); BBL_Valid(bbl); bbl = BBL_Next(bbl)) {
    for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins)) {
      instruction(ins, v);
    }
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
  // std::cout << "Mem state:" << std::endl;
  // for (const auto &[addr, n] : mem_map) {
  //   show_node(n, "");
  // }

  // std::cout << std::endl << "Reg state:" << std::endl;
  // for (const auto &[addr, n] : reg_map) {
  //   show_node(n, "");
  // }
}

void dump_graph() {
  std::ofstream graph_file("/tmp/prog.gr");
  show_mem_map(graph_file);
  graph_file.close();

  clean_mem_map();
}

void start_instr() { dde_state.instr_active = true; }
void stop_instr() { dde_state.instr_active = false; }

void mark_var(double *x, const char *mark, int ordinal) {
  double value;
  PIN_SafeCopy(&value, x, sizeof(double));

  NodePtr n = std::make_shared<Node>(value);
  n->output = false;
  n->uuid = ordinal < 0 ? mark : mark + std::to_string(ordinal);

  mem::insert_node((uint64_t)x, n);
}

void mark_output(double *y, const char *mark) {
  NodePtr n = mem::expect_node((uint64_t)y);
  n->output = true;
  n->uuid = mark;
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

  bool dde_var = rtn_name.find("var") != std::string::npos;
  if (dde_var) {
    RTN_Open(rtn);
    RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)mark_var,
                   IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                   IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
                   IARG_FUNCARG_ENTRYPOINT_VALUE, 2, IARG_END);
    RTN_Close(rtn);
    return;
  }

  bool dde_output = rtn_name.find("output") != std::string::npos;
  if (dde_output) {
    RTN_Open(rtn);
    RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)mark_output,
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
  TRACE_AddInstrumentFunction(trace, 0);
  PIN_AddFiniFunction(final_processing, 0);

  PIN_StartProgram();

  return 0;
}
