#include "pin.H"
#include <iostream>
#include <map>

enum TransfType { IMUL = 0, ADD, SUB, TERM };

struct node;
struct transformation {
  TransfType transf_t;
  node *args;
  uint arg_count;
};

struct node {
  uint32_t identifier;
  transformation transf;
};

struct variable_context {
  REG reg;
  int displacement;
};

struct operand_context {
  // Operand ordinals within an instruction
  uint pin_idx;
  uint assembly_idx;
  // Register
  REG reg;
  bool is_mem;
};

std::map<std::string, int> ins_ctx_table;
std::vector<node *> nodes;

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
  return RTN_Valid(rtn) &&
         (RTN_Name(rtn) == "main" ||
          RTN_Name(rtn).find("multiply") != std::string::npos);
}

operand_context *analyze_transformation_operands(INS ins) {
  uint operand_count = ins_ctx_table[INS_Mnemonic(ins)];
  operand_context *op_ctx_arr = new operand_context[operand_count];

  for (UINT32 operand_idx = 0; operand_idx < operand_count; operand_idx++) {
    operand_context *op_ctx = op_ctx_arr + operand_idx;
    op_ctx->pin_idx = operand_count - operand_idx - 1;
    op_ctx->assembly_idx = operand_idx;
    op_ctx->is_mem = INS_OperandIsMemory(ins, op_ctx->pin_idx);
    op_ctx->reg =
        !op_ctx->is_mem ? INS_OperandReg(ins, op_ctx->pin_idx) : REG_INVALID();
  }

  return op_ctx_arr;
}

BOOL trace_result_destination(INS ins, operand_context *acc_op_ctx,
                              variable_context *dest_var_ctx) {
  assert(!acc_op_ctx->is_mem);
  assert(REG_valid(acc_op_ctx->reg));

  int max_forward_count = 2;
  while ((max_forward_count--) > 0) {
    INS next_ins = INS_Next(ins);
    if (!(INS_Valid(next_ins) && INS_IsMov(next_ins))) {
      ins = next_ins;
      continue;
    }

    REG source_reg = INS_OperandReg(next_ins, 1);
    bool destination_found =
        INS_OperandIsMemory(next_ins, 0) && source_reg == acc_op_ctx->reg;
    if (!destination_found) {
      ins = next_ins;
      continue;
    }

    dest_var_ctx->reg = INS_OperandMemoryBaseReg(next_ins, 0);
    dest_var_ctx->displacement = INS_OperandMemoryDisplacement(next_ins, 0);
    return true;
  }

  return false;
}

BOOL traceback_operand_to_memory(INS ins, operand_context *op_ctx,
                                 variable_context *var_ctx) {
  if (op_ctx->is_mem) {
    var_ctx->reg = INS_OperandMemoryBaseReg(ins, op_ctx->pin_idx);
    var_ctx->displacement = INS_OperandMemoryDisplacement(ins, op_ctx->pin_idx);
    return true;
  }

  int max_traceback_count = 3;
  while ((max_traceback_count--) > 0) {
    INS prev_ins = INS_Prev(ins);
    if (!(INS_Valid(prev_ins) && INS_IsMov(prev_ins))) {
      ins = prev_ins;
      continue;
    }

    REG load_reg = INS_OperandReg(prev_ins, 0);
    bool var_recovered =
        INS_OperandIsMemory(prev_ins, 1) && load_reg == op_ctx->reg;

    if (!var_recovered) {
      ins = prev_ins;
      continue;
    }

    var_ctx->reg = INS_OperandMemoryBaseReg(prev_ins, 1);
    var_ctx->displacement = INS_OperandMemoryDisplacement(prev_ins, 1);
    return true;
  }

  std::cout << "Failed to recover operand " << op_ctx->assembly_idx
            << " as a variable." << std::endl;
  return false;
}

BOOL transformation_candidate(INS ins) {
  std::string mnemonic = INS_Mnemonic(ins);
  return mnemonic == "MUL" || mnemonic == "IMUL" || mnemonic == "ADD";
}

VOID insert_dag_node(const CONTEXT *ctxt, variable_context *var_ctx_arr,
                     UINT32 operand_count, UINT instruction_type) {
  node *dag_nodes = new node[operand_count + 1];
  for (uint i = 0; i < operand_count + 1; i++) {
    variable_context var_ctx = var_ctx_arr[i];
    ADDRINT effective_addr;

    PIN_GetContextRegval(ctxt, var_ctx.reg, (UINT8 *)&effective_addr);
    effective_addr += var_ctx.displacement;

    (dag_nodes + i)->identifier = effective_addr;
    (dag_nodes + i)->transf = {
        .transf_t = TransfType::TERM, .args = nullptr, .arg_count = 0};
  }

  (dag_nodes + operand_count)->transf = {
      .transf_t = (TransfType)instruction_type,
      .args = dag_nodes,
      .arg_count = operand_count,
  };

  nodes.push_back(dag_nodes + operand_count);
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
  variable_context *var_ctx_arr =
      new variable_context[ins_ctx_table[INS_Mnemonic(ins)] + 1];

  bool all_vars_recovered = true;
  for (int i = 0; i < ins_ctx_table[INS_Mnemonic(ins)]; i++) {
    operand_context *op_ctx = op_ctx_arr + i;
    variable_context *var_ctx = var_ctx_arr + i;
    all_vars_recovered =
        all_vars_recovered && traceback_operand_to_memory(ins, op_ctx, var_ctx);
  }

  if (!all_vars_recovered) {
    std::cout << "Failde to recover operands for:" << std::endl;
    std::cout << INS_Disassemble(ins) << std::endl;
    goto clean_all;
  }

  // Here we start searching for the destination memory location
  if (!trace_result_destination(ins, op_ctx_arr + 1,
                                var_ctx_arr +
                                    ins_ctx_table[INS_Mnemonic(ins)])) {
    std::cout << "Failde to recover destination location for:" << std::endl;
    std::cout << INS_Disassemble(ins) << std::endl;
    goto clean_all;
  }

  INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)insert_dag_node,
                 IARG_CONST_CONTEXT, IARG_PTR, var_ctx_arr, IARG_UINT32,
                 ins_ctx_table[INS_Mnemonic(ins)], IARG_UINT32,
                 TransfType::IMUL, IARG_END);
  goto clean_operand_arr;

clean_all : {
  delete[](variable_context *) var_ctx_arr;
clean_operand_arr:
  delete[](operand_context *) op_ctx_arr;
}
  return;
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

void print_node(node *n, std::string prefix) {
  std::cout << prefix << "0x" << std::hex << n->identifier;
  if (n->transf.arg_count == 0 || n->transf.args == nullptr) {
    std::cout << std::endl;
    return;
  }

  std::cout << " " << n->transf.transf_t << std::endl;
  for (uint i = 0; i < n->transf.arg_count; i++) {
    print_node(n->transf.args + i, prefix + " ");
  }

  return;
}

void final_processing(INT32 code, VOID *v) {
  for (node *n : nodes) {
    print_node(n, "");
  }
};

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

  // Final graph processing
  PIN_AddFiniFunction(final_processing, 0);

  // Start the program, never returns
  PIN_StartProgram();

  return 0;
}
