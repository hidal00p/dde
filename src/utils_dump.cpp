#include "pin.H"
#include <iostream>

VOID echo_mem_op(VOID *addr, INT32 size, CHAR access_type, CHAR *mnemonic) {
  bool is_int = size == 4;
  if (!is_int)
    return;

  std::cout << access_type << ": " << addr << " for " << mnemonic << std::endl;
  free(mnemonic);
}

char *get_ins_mnemonic(INS ins) {
  char *mnemonic = (char *)malloc(sizeof(char) * 10);
  strcpy(mnemonic, INS_Mnemonic(ins).c_str());
  return mnemonic;
}

/*
 * Register usage is fixed per BBL. Since the assymbly is set in stone.
 * Hence, it is suffices to undestand the access pattern of the
 * instruction only once, and then at each next call the same
 * pattern will repeat itself.
 */
void show_regs(INS ins) {
  for (uint i = 0; i < INS_MaxNumRRegs(ins); i++) {
    REG reg = INS_RegR(ins, i);
    std::cout << "Reads from: " << REG_StringShort(reg) << std::endl;
  }

  // Print registers written by the instruction
  for (uint i = 0; i < INS_MaxNumWRegs(ins); i++) {
    REG reg = INS_RegW(ins, i);
    std::cout << "Writes to: " << REG_StringShort(reg) << std::endl;
  }
}

void find_calling_routine(CONTEXT *ctx, ADDRINT branch_addr) {
  ADDRINT ret_addr = PIN_GetContextReg(ctx, REG_INST_PTR);
  PIN_LockClient();
  RTN caller_rtn = RTN_FindByAddress(ret_addr);
  PIN_UnlockClient();

  PIN_LockClient();
  RTN target_rtn = RTN_FindByAddress(branch_addr);
  PIN_UnlockClient();

  if (RTN_Valid(caller_rtn) && RTN_Valid(target_rtn))
    std::cout << "Calling: " << RTN_Name(target_rtn) << " from "
              << RTN_Name(caller_rtn) << std::endl;
}

void generate_instruction_report(INS ins) {
  UINT32 operand_count = ins_ctx_table[INS_Mnemonic(ins)];
  UINT32 mem_op_idx = 0;

  std::cout << "----------------" << std::endl;
  std::cout << INS_Disassemble(ins) << std::endl;
  std::cout << "----------------" << std::endl;

  // For some reason Pin accesses variables in a reversed order.
  for (UINT32 operand_idx = 0; operand_idx < operand_count; operand_idx++) {
    UINT32 effective_idx =
        operand_count - operand_idx - 1; // account for reversed order
    std::cout << operand_idx + 1 << ": ";
    if (INS_OperandIsImmediate(ins, effective_idx)) {
      std::cout << "Immediate " << INS_OperandImmediate(ins, effective_idx)
                << std::endl;
    } else if (INS_OperandIsReg(ins, effective_idx)) {
      std::cout << "Register "
                << REG_StringShort(INS_OperandReg(ins, effective_idx))
                << std::endl;
    } else if (INS_OperandIsMemory(ins, effective_idx)) {
      const char *mem_access_type =
          INS_MemoryOperandIsRead(ins, mem_op_idx) ? "read" : "write";
      std::cout << "Memory " << mem_access_type << std::endl;
      mem_op_idx++;
    }
  }

  std::cout << "----------------" << std::endl << std::endl;
}
