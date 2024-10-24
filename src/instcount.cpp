/*
 * Copyright (C) 2004-2021 Intel Corporation.
 * SPDX-License-Identifier: MIT
 */

#include "pin.H"
#include <fstream>

// The running count of instructions is kept here
// make it static to help the compiler optimize docount
static UINT64 icount = 0;

// This function is called before every instruction is executed
VOID docount() { icount++; }

// Pin calls this function every time a new instruction is encountered
VOID Instruction(INS ins, VOID *v) {
  // Insert a call to docount before every instruction, no arguments are passed
  INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)docount, IARG_END);
}

VOID Fini(INT32 code, VOID *v) { 
  std::ofstream res_file;
  res_file.open("res.txt");
  res_file << icount << std::endl;
  res_file.close();
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage() {
  puts("This tool counts the number of instructions executed.\n");
  puts("Read Intel Pin instruction manual to learn how to properly "
       "execute pin tools.");
  return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */
/*   argc, argv are the entire command line: pin -t <toolname> -- ...    */
/* ===================================================================== */

int main(int argc, char *argv[]) {
  // Initialize pin
  if (PIN_Init(argc, argv))
    return Usage();

  // Register Instruction to be called to instrument instructions
  INS_AddInstrumentFunction(Instruction, 0);

  // Register Fini to be called when the application exits
  PIN_AddFiniFunction(Fini, 0);

  // Start the program, never returns
  PIN_StartProgram();

  return 0;
}
