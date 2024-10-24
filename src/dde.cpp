
#include "pin.H"
#include <fstream>

// The running count of instructions is kept here
// make it static to help the compiler optimize docount
static UINT64 icount = 0;

// This function is called before every instruction is executed
VOID docount() { icount++; }

// Pin calls this function every time a new instruction is encountered
VOID instruction(INS ins, VOID *v) {

  RTN rtn = INS_Rtn(ins);
  if (!RTN_Valid(rtn) || RTN_Name(rtn) != "main")
    return;

  INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)docount, IARG_END);
}

VOID fini(INT32 code, VOID *v) {
  std::ofstream res_file;
  res_file.open("res.txt");
  res_file << icount << std::endl;
  res_file.close();
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 usage() {
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
