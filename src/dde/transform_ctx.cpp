#include "transform_ctx.h"

#include <cassert>

namespace binary_op {
ctx *get_bop_operands(INS ins) {
  ctx *bop_ctx = new ctx();
  bop_ctx->ins = ins;

  if (INS_OperandIsMemory(ins, DEST_IDX)) {
    bop_ctx->dest = {
        .type = OprType::MEM,
    };
  } else {
    bop_ctx->dest = {.origin = {.reg = INS_OperandReg(ins, DEST_IDX)},
                     .type = OprType::REGSTR};
  }

  if (INS_OperandIsImmediate(ins, SRC_IDX)) {
    bop_ctx->src = {.origin = {.imm = INS_OperandImmediate(ins, SRC_IDX)},
                    .type = OprType::IMM};
  } else if (INS_OperandIsReg(ins, SRC_IDX)) {
    bop_ctx->src = {.origin = {.reg = INS_OperandReg(ins, SRC_IDX)},
                    .type = OprType::REGSTR};
  } else {
    bop_ctx->src = {.type = OprType::MEM};
  }

  return bop_ctx;
}
} // namespace binary_op
