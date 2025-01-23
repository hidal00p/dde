#ifndef DDE_PARAMS_H
#define DDE_PARAMS_H

#include "pin.H"

struct DataRegion {
  uint64_t start;
  uint64_t end;

  bool within_range(ADDRINT ea);
};

struct Sections {
  DataRegion data;
  DataRegion rodata;
};

extern Sections sec_info;

struct DdeState {
  bool to_instrument = false;
};

extern DdeState dde_state;

struct var_mark_ctx {
  bool is_var_marked = false;
  char var_mark_buffer[1] = {0};
};

extern var_mark_ctx vm_ctx;

#endif
