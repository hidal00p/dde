#include "pin_utils.h"
#include <iostream>

bool is_img_main(RTN rtn) {
  if (!RTN_Valid(rtn))
    return false;

  SEC sec = RTN_Sec(rtn);
  if (!SEC_Valid(sec))
    return false;

  IMG img = SEC_Img(sec);
  return IMG_IsMainExecutable(img);
}

bool is_img_main(INS ins) { return is_img_main(INS_Rtn(ins)); }

bool is_main_rtn(INS ins) {
  RTN rtn = INS_Rtn(ins);
  return RTN_Valid(rtn) && (RTN_Name(rtn) == "main" ||
                            RTN_Name(rtn).find("my_func") != std::string::npos);
}

void show_ins(INS ins) { std::cout << INS_Disassemble(ins) << std::endl; }
