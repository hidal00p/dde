#pragma once
#include "pin.H"

enum OprType { IMM, REGSTR, MEM };

typedef union {
  REG reg;
  uint64_t imm;
} Origin;

struct Operand {
  Origin origin;
  OprType type;
};

namespace binary_op {
struct ctx {
  Operand src;
  Operand dest;
  INS ins;
};

ctx *get_bop_operands(INS ins);
} // namespace binary_op
