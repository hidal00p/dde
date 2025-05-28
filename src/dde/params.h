#ifndef DDE_PARAMS_H
#define DDE_PARAMS_H

#include "graph.h"
#include <map>
#include <optional>

struct DataRegion {
  uint64_t start;
  uint64_t end;

  bool within_range(uint64_t ea);
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

struct VarMarkCtx {
  bool is_var_marked = false;
  bool output = false;
  std::string mark;
};

extern VarMarkCtx var_marking_ctx;

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

#endif
