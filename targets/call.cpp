#include "../lib/dde.h"
#include <cmath>

int main() {
  DDE_START
  DDE_VAR("x", double x = 2.0)
  DDE_OUTPUT("y", double y = 0.0)
  y = std::cos(x * x - 1.0);
  DDE_DUMP_GRAPH
  DDE_STOP

  return 0;
}
