#ifndef TRANSFORM_CTX_H
#define TRANSFORM_CTX_H

#include "pin.H"

enum OprType { IMM, REGSTR, MEM };

typedef union {
  REG reg;
  uint64_t imm;
} Origin;

struct operand {
  Origin origin;
  OprType type;
};

namespace binary_op {
struct ctx {
  operand src;
  operand dest;
  INS ins;
};

ctx *get_bop_operands(INS ins);
} // namespace binary_op
#endif
