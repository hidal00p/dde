#include "cppunitlite/TestHarness.h"

#include "pin.H"
#include "transform_ctx.h"

TEST(test_bop_ctx) {
  OPR dest = {.type = _REGSTR, .val = {.reg = REG::REG_EAX}};
  OPR src = {.type = _MEM};
  INS ins(dest, src);

  binary_op::ctx *bop = binary_op::get_bop_operands(ins);
  CHECK(bop->dest.type == OprType::REGSTR &&
        bop->dest.origin.reg == REG::REG_EAX);
  CHECK(bop->src.type == OprType::MEM);
}
