#include "pin.H"

#include <cassert>

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
