#include "pin.H"
#include <iostream>
#include <map>

struct operand_context {
  bool is_mem;
  uint pin_idx;
  uint assembly_idx;

  REG reg;
  int displacement;
};

std::map<std::string, int> ins_ctx_table;

VOID init_ins_ctx_table() {
  ins_ctx_table["IMUL"] = 2;
  ins_ctx_table["MOV"] = 2;
}

BOOL is_img_main(RTN rtn) {
  if (!RTN_Valid(rtn))
    return false;

  SEC sec = RTN_Sec(rtn);
  if (!SEC_Valid(sec))
    return false;

  IMG img = SEC_Img(sec);
  return IMG_IsMainExecutable(img);
}

BOOL is_img_main(INS ins) { return is_img_main(INS_Rtn(ins)); }

BOOL is_img_main(TRACE trc) { return is_img_main(TRACE_Rtn(trc)); }

BOOL is_main_rtn(INS ins) {
  RTN rtn = INS_Rtn(ins);
  return RTN_Valid(rtn) && RTN_Name(rtn) == "main";
}

VOID echo_recovered_operand(operand_context *op_ctx) {
  std::cout << "Operand " << op_ctx->assembly_idx + 1 << " corresponds to ";
  std::cout << "[ " << REG_StringShort(op_ctx->reg);

  if (op_ctx->displacement < 0)
    std::cout << " - " << std::abs(op_ctx->displacement);
  else
    std::cout << " + " << op_ctx->displacement;

  std::cout << " ]" << std::endl;
}

operand_context *analyze_transformation_operands(INS ins) {
  uint operand_count = ins_ctx_table[INS_Mnemonic(ins)];
  operand_context *op_ctx_arr = new operand_context[operand_count];

  for (UINT32 operand_idx = 0; operand_idx < operand_count; operand_idx++) {
    operand_context *op_ctx = op_ctx_arr + operand_idx;

    // Account for Pin reversed operand order
    op_ctx->pin_idx = operand_count - operand_idx - 1;

    op_ctx->assembly_idx = operand_idx;
    op_ctx->is_mem = INS_OperandIsMemory(ins, op_ctx->pin_idx);

    if (op_ctx->is_mem) {
      op_ctx->reg = INS_OperandMemoryBaseReg(ins, op_ctx->pin_idx);
      op_ctx->displacement =
          INS_OperandMemoryDisplacement(ins, op_ctx->pin_idx);
    } else {
      op_ctx->reg = INS_OperandReg(ins, op_ctx->pin_idx);
    }
  }

  return op_ctx_arr;
}

VOID traceback_operand_to_memory(INS ins, operand_context *op_ctx) {

  int max_traceback_count = 3;
  while ((max_traceback_count--) > 0) {
    INS prev_ins = INS_Prev(ins);
    if (!INS_Valid(prev_ins))
      continue;

    if (!INS_IsMov(prev_ins))
      continue;

    REG load_reg = INS_OperandReg(prev_ins, 0);
    bool var_recovered =
        INS_OperandIsMemory(prev_ins, 1) && load_reg == op_ctx->reg;

    if (!var_recovered)
      continue;

    op_ctx->is_mem = true;
    op_ctx->reg = INS_OperandMemoryBaseReg(prev_ins, 1);
    op_ctx->displacement = INS_OperandMemoryDisplacement(prev_ins, 1);

    echo_recovered_operand(op_ctx);
    return;
  }

  std::cout << "Failed to recover operand " << op_ctx->assembly_idx
            << " as a variable." << std::endl;
}

BOOL transformation_candidate(INS ins) {
  std::string mnemonic = INS_Mnemonic(ins);
  return mnemonic == "MUL" || mnemonic == "IMUL" || mnemonic == "ADD";
}

VOID insert_dag_node(const CONTEXT *ctxt, VOID *op_ctx_arr, UINT32 arr_size) {

  for (uint i = 0; i < arr_size; i++) {
    operand_context op_ctx = ((operand_context *)op_ctx_arr)[i];
    ADDRINT effective_addr;
    PIN_GetContextRegval(ctxt, op_ctx.reg,
                         reinterpret_cast<UINT8 *>(&effective_addr));
    effective_addr += op_ctx.displacement;
    std::cout << REG_StringShort(op_ctx.reg) << ": 0x" << std::hex
              << effective_addr << std::endl;
  }

  delete[](operand_context *) op_ctx_arr;
}

VOID instruction(INS ins, VOID *v) {
  if (!is_main_rtn(ins))
    return;

  if (!transformation_candidate(ins))
    return;

  // All memory locations eventually end up as values in registers.
  // Hence, for indentification what we need is the register + displacement
  // value to compute an effective address.
  operand_context *op_ctx_arr = analyze_transformation_operands(ins);

  bool all_vars_recovered = true;
  for (int i = 0; i < ins_ctx_table[INS_Mnemonic(ins)]; i++) {
    operand_context *op_ctx = op_ctx_arr + i;
    if (op_ctx->is_mem) {
      echo_recovered_operand(op_ctx);
    } else {
      traceback_operand_to_memory(ins, op_ctx);
    }
    all_vars_recovered = all_vars_recovered && op_ctx->is_mem;
  }

  if (!all_vars_recovered)
    return;

  INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)insert_dag_node,
                 IARG_CONST_CONTEXT, IARG_PTR, op_ctx_arr, IARG_UINT32,
                 ins_ctx_table[INS_Mnemonic(ins)], IARG_END);
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

  // Initialize analysis runtime structures
  init_ins_ctx_table();

  // Register Instruction to be called to instrument instructions
  INS_AddInstrumentFunction(instruction, 0);

  // Start the program, never returns
  PIN_StartProgram();

  return 0;
}
