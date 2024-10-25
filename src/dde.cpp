#include "pin.H"
#include <iostream>

BOOL is_img_main(TRACE trc) {
  RTN rtn = TRACE_Rtn(trc);
  if (!RTN_Valid(rtn))
    return false;

  SEC sec = RTN_Sec(rtn);
  if (!SEC_Valid(sec))
    return false;

  IMG img = SEC_Img(sec);
  return IMG_IsMainExecutable(img);
}

void insert_dag_node() { return; }

VOID trace(TRACE trc, VOID *v) {
  if (!is_img_main(trc))
    return;

  // Visit every basic block in the trace
  for (BBL bbl = TRACE_BblHead(trc); BBL_Valid(bbl); bbl = BBL_Next(bbl)) {
    // Insert a call to docount before every bbl, passing the number of
    // instructions
    BBL_InsertCall(bbl, IPOINT_BEFORE, (AFUNPTR)insert_dag_node, IARG_END);
  }
}

VOID fini(INT32 code, VOID *v) { std::cout << "Done..." << std::endl; }

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
  // Initialize pin and symbols
  PIN_InitSymbols();
  if (PIN_Init(argc, argv))
    return usage();

  // Register Instruction to be called to instrument instructions
  TRACE_AddInstrumentFunction(trace, 0);

  // Register Fini to be called when the application exits
  PIN_AddFiniFunction(fini, 0);

  // Start the program, never returns
  PIN_StartProgram();

  return 0;
}
