#include "../lib/dde.h"
#include <cmath>

double g(double x) { return x * x * std::cos(x); }

int main() {
  DDE_START
  DDE_VAR("x", double x = 2.0)
  DDE_OUTPUT("y", double y = 0.0)
  y = g(x) * std::sin(x);
  DDE_DUMP_GRAPH
  DDE_STOP

  return 0;
}
