#include "dde_params.h"

bool DataRegion::within_range(ADDRINT ea) {
  return ea >= this->start && ea <= this->end;
}

Sections sec_info = {.data = {}, .rodata = {}};
DdeState dde_state = {.to_instrument = false};
var_mark_ctx vm_ctx;
