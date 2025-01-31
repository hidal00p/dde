#include "params.h"

bool DataRegion::within_range(ADDRINT ea) {
  return ea >= this->start && ea <= this->end;
}

bool CallPair::reversed(std::string new_to, std::string new_from) {
  return new_to == from && new_from == to;
}

bool CallPair::empty() { return to.empty() && from.empty(); }

Sections sec_info = {.data = {}, .rodata = {}};
DdeState dde_state;
var_mark_ctx vm_ctx;
CallPair call_pair;

const std::vector<std::string> intrinsic_routines = {"cos"};
bool rtn_is_valid_transform(std::string rtn) {
  if (rtn.empty())
    return false;

  for (auto &intr : intrinsic_routines) {
    if (rtn.find(intr) != std::string::npos)
      return true;
  }

  return false;
}
