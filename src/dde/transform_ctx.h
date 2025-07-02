#pragma once
#include "pin.H"

constexpr uint8_t SRC_IDX = 1, DEST_IDX = 0;
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
