#include "../lib/dde.h"
#include <cassert>
#include <string>

// This would be effectively abstracted away from us
double g(double x) { return 3.0 * x - 4.0; }
double f(double x) { return g(x) * x + 5; }

int main() {
  DDE_START
  DDE_VAR("x", double x = 3.0)
  DDE_OUTPUT("y", double y = 0.0)
  y = f(x);
  DDE_STOP

  return 0;
}
