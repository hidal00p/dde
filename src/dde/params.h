#pragma once

#include "graph.h"
#include <map>
#include <optional>

struct DdeState {
  bool instr_active = false;
  bool within_instrinsic = false;
};

extern DdeState dde_state;

struct CallPair {
  std::string to;
  std::string from;

  bool reversed(std::string new_to, std::string new_from);
  bool empty();
};

extern CallPair call_pair;

typedef double (*IntrinsicCall)(double x);
struct Intrinsic {
  IntrinsicCall intrinsic_call;
  Transformation transf;
};

bool rtn_is_valid_transform(std::string rtn);
std::optional<Intrinsic> get_intrinsic_from_rtn_name(std::string rtn);
extern const std::map<std::string, Intrinsic> intrinsic_routines;
