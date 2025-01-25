#include "../lib/dde.h"
#include <cassert>
#include <string>

// This would be effectively abstracted away from us
double g(double x) {
  double y = 3.0 * x;
  return y * y - 4.0 + y;
}
double f(double x) { return g(x) * x + 5; }

int main() {
  uint8_t i = 0;
  double tmp_x = 0;
  do {
    tmp_x = (i + 1) * 2.0;
    DDE_START
    DDE_VAR("x", double x = tmp_x)
    DDE_OUTPUT("y", double y = 0.0)
    y = f(x);
    DDE_STOP
    DDE_DUMP_GRAPH
  } while (++i < 1);
  return 0;
}
