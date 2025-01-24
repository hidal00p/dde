#ifndef PIN_UTILS_H
#define PIN_UTILS_H

#include "pin.H"

bool is_fpu_stack_reg(REG st);
uint8_t get_fpu_stack_idx_from_st(REG st);
bool is_img_main(RTN rtn);
bool is_img_main(INS ins);
bool is_main_rtn(INS ins);
void show_ins(INS ins);

#endif
