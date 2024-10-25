/*
 * Copyright (C) 2004-2021 Intel Corporation.
 * SPDX-License-Identifier: MIT
 */

#include "pin.H"
#include <iostream>

// The running count of instructions is kept here
// make it static to help the compiler optimize docount
static UINT64 icount = 0;

// This function is called before every instruction is executed
VOID increment_ins_counter() { icount++; }

// Pin calls this function every time a new instruction is encountered
VOID instruction(INS ins, VOID *v) {
  // Insert a call to docount before every instruction, no arguments are passed

  RTN rtn = INS_Rtn(ins);
  if (!RTN_Valid(rtn) || RTN_Name(rtn) != "main")
    return;

  std::cout << INS_Mnemonic(ins) << std::endl;
  INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)increment_ins_counter, IARG_END);
}

VOID fini(INT32 code, VOID *v) { std::cout << icount << std::endl; }

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 usage() {
  std::cout << "Read Intel Pin instruction manual to learn how to properly "
               "execute pin tools."
            << std::endl;
  return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */
/*   argc, argv are the entire command line: pin -t <toolname> -- ...    */
/* ===================================================================== */

int main(int argc, char *argv[]) {
  // Initialize pin
  PIN_InitSymbols();
  if (PIN_Init(argc, argv))
    return usage();

  // Register Instruction to be called to instrument instructions
  INS_AddInstrumentFunction(instruction, 0);

  // Register Fini to be called when the application exits
  PIN_AddFiniFunction(fini, 0);

  // Start the program, never returns
  PIN_StartProgram();

  return 0;
}
