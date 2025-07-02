#include "pin.H"

#include <cassert>
#include <map>

bool INS_OperandIsMemory(INS ins, uint8_t idx) {
  assert(idx < 2);
  return ins.ops[idx].type == OPR_TYPE::_MEM;
}

bool INS_OperandIsReg(INS ins, uint8_t idx) {
  assert(idx < 2);
  return ins.ops[idx].type == OPR_TYPE::_REGSTR;
}

bool INS_OperandIsImmediate(INS ins, uint8_t idx) {
  assert(idx < 2);
  return ins.ops[idx].type == OPR_TYPE::_IMM;
}

REG INS_OperandReg(INS ins, uint8_t idx) {
  assert(INS_OperandIsReg(ins, idx));
  return ins.ops[idx].val.reg;
}

uint64_t INS_OperandImmediate(INS ins, uint8_t idx) {
  assert(INS_OperandIsImmediate(ins, idx));
  return ins.ops[idx].val.imm;
}

void PIN_SafeCopy(void *a, void *mem_addr, uint8_t size) {
  double *f = (double *)a;
  *f = 42.0;
}

bool is_fpu_stack_reg(REG reg) {
  return reg >= REG_ST_BASE && reg <= REG_ST_LAST;
}

uint8_t get_fpu_stack_idx_from_st(REG st) {
  assert(is_fpu_stack_reg(st));
  return st - REG_ST_BASE;
}

std::map<ADDRINT, std::string> routines = {
    {MAIN, "main"},   {FOO, "foo"}, {SINUS, "sin"},
    {COSINUS, "cos"}, {BAR, "bar"}, {EMPTY, ""},
};

std::string RTN_FindNameByAddress(ADDRINT rtn_addr) {
  if (routines.count(rtn_addr) == 0)
    return routines[EMPTY];

  return routines[rtn_addr];
}
void PIN_GetContextRegval(CONTEXT *ctx, REG reg, uint8_t *addr) {}
