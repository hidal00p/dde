#include "params.h"
#include <cmath>

bool CallPair::reversed(std::string new_to, std::string new_from) {
  return new_to == from && new_from == to;
}

bool CallPair::empty() { return to.empty() && from.empty(); }

DdeState dde_state;
CallPair call_pair;

const std::map<std::string, Intrinsic> intrinsic_routines = {
    {"cos", {.intrinsic_call = std::cos, .transf = Transformation::COS}},
    {"sin", {.intrinsic_call = std::sin, .transf = Transformation::SIN}}};

bool rtn_is_valid_transform(std::string rtn) {
  if (rtn.empty())
    return false;

  for (const auto &[name, intr] : intrinsic_routines) {
    if (rtn.find(name) != std::string::npos)
      return true;
  }

  return false;
}

std::optional<Intrinsic> get_intrinsic_from_rtn_name(std::string rtn) {
  if (rtn.empty())
    return std::nullopt;

  for (const auto &[name, intr] : intrinsic_routines) {
    if (rtn.find(name) != std::string::npos)
      return std::optional(intr);
  }

  return std::nullopt;
}
