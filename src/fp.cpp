#include "pin_utils.h"
#include <cassert>
#include <iostream>
#include <map>
#include <string>

struct m {
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

VOID analyze(CONTEXT *ctx, m *mem_ctx) {

  double val = *((double *)mem_ctx->get_effective_addr(ctx));
  PIN_SafeCopy(&val, (void *)mem_ctx->get_effective_addr(ctx), 8);
  std::cout << val << std::endl;
}

VOID instruction(INS ins, VOID *v) {
  if (!is_main_rtn(ins))
    return;

  if (INS_Opcode(ins) == XED_ICLASS_FLD) {
    m *mem_ctx = new m;
    mem_ctx->reg = INS_OperandMemoryBaseReg(ins, 1);
    mem_ctx->disp = INS_OperandMemoryDisplacement(ins, 1);
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)analyze, IARG_CONTEXT, IARG_PTR,
                   mem_ctx, IARG_MEMORYREAD_EA, IARG_INST_PTR, IARG_END);
  }

  //  if (INS_Opcode(ins) == XED_ICLASS_FADDP) {
  //    std::cout << REG_StringShort(INS_OperandReg(ins, 1)) << " -> "
  //              << REG_StringShort(INS_OperandReg(ins, 0)) << std::endl;
  //  }

  //  if (INS_Opcode(ins) == XED_ICLASS_FSTP) {
  //    std::cout << REG_StringShort(INS_OperandReg(ins, 1)) << " -> "
  //              << REG_StringShort(INS_OperandMemoryBaseReg(ins, 0)) <<
  //              std::endl;
  //  }
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
