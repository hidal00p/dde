#include "cppunitlite/TestHarness.h"

#include "pin.H"
#include "transform_ctx.h"

TEST(test_bop_ctx_mem_to_reg) {
  OPR dest = {.type = _REGSTR, .val = {.reg = REG::REG_EAX}};
  OPR src = {.type = _MEM};
  INS ins(dest, src);

  binary_op::ctx *bop = binary_op::get_bop_operands(ins);
  CHECK(bop->dest.type == OprType::REGSTR &&
        bop->dest.origin.reg == REG::REG_EAX);
  CHECK(bop->src.type == OprType::MEM);
}

TEST(test_bop_ctx_reg_to_reg) {
  OPR dest = {.type = _REGSTR, .val = {.reg = REG::REG_EDX}};
  OPR src = {.type = _REGSTR, .val = {.reg = REG::REG_EAX}};
  INS ins(dest, src);

  binary_op::ctx *bop = binary_op::get_bop_operands(ins);
  CHECK(bop->dest.type == OprType::REGSTR &&
        bop->dest.origin.reg == REG::REG_EDX);
  CHECK(bop->src.type == OprType::REGSTR &&
        bop->src.origin.reg == REG::REG_EAX);
}

TEST(test_bop_ctx_imm_to_reg) {
  OPR dest = {.type = _REGSTR, .val = {.reg = REG::REG_EDX}};
  OPR src = {.type = _IMM, .val = {.imm = 42}};
  INS ins(dest, src);

  binary_op::ctx *bop = binary_op::get_bop_operands(ins);
  CHECK(bop->dest.type == OprType::REGSTR &&
        bop->dest.origin.reg == REG::REG_EDX);
  CHECK(bop->src.type == OprType::IMM && bop->src.origin.imm == 42);
}
