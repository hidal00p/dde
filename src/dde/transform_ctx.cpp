#include "transform_ctx.h"
#include <iostream>

void show_operand(CONTEXT *ctx, operand opr) {
  Origin orig = opr.origin;
  switch (opr.type) {
  case OprType::IMM:
    std::cout << orig.imm;
    break;
  case OprType::REGSTR:
    std::cout << REG_StringShort(orig.reg);
    break;
  case OprType::MEM:
    uint64_t eff_addr;
    PIN_GetContextRegval(ctx, orig.mem.reg, (uint8_t *)&eff_addr);
    eff_addr += orig.mem.disp;
    std::cout << "0x" << std::hex << eff_addr;
    break;
  }
}
