#include "pin_utils.h"

bool is_fpu_stack_reg(REG reg) {
  return reg >= REG_ST_BASE && reg <= REG_ST_LAST;
}

uint8_t get_fpu_stack_idx_from_st(REG st) {
  assert(is_fpu_stack_reg(st));
  return st - REG_ST_BASE;
}

BOOL is_img_main(RTN rtn) {
  if (!RTN_Valid(rtn))
    return false;

  SEC sec = RTN_Sec(rtn);
  if (!SEC_Valid(sec))
    return false;

  IMG img = SEC_Img(sec);
  return IMG_IsMainExecutable(img);
}

BOOL is_img_main(INS ins) { return is_img_main(INS_Rtn(ins)); }

BOOL is_main_rtn(INS ins) {
  RTN rtn = INS_Rtn(ins);
  return RTN_Valid(rtn) && (RTN_Name(rtn) == "main" ||
                            RTN_Name(rtn).find("my_func") != std::string::npos);
}
