#ifndef TRANSFORM_CTX_H
#define TRANSFORM_CTX_H

#include "pin.H"

struct Mem {
  REG reg;
  int64_t disp;
};

enum OprType { IMM, REGSTR, MEM };

typedef union {
  REG reg;
  uint64_t imm;
  Mem mem;
} Origin;

struct operand {
  Origin origin;
  OprType type;
};

namespace binary_op {
struct ctx {
  operand src;
  operand dest;
};
} // namespace binary_op

void show_operand(CONTEXT *ctx, OprType t, operand opr);
#endif
