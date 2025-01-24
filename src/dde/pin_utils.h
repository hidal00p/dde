#ifndef PIN_UTILS_H
#define PIN_UTILS_H

#include "pin.H"

bool is_fpu_stack_reg(REG st);
uint8_t get_fpu_stack_idx_from_st(REG st);
BOOL is_img_main(RTN rtn);
BOOL is_img_main(INS ins);
BOOL is_main_rtn(INS ins);

#endif
