#include "../lib/dde.h"
#include <cmath>

double g(double x) { return x * x * std::cos(x); }

int main() {
  dde::start();

  dde::var("x");
  double x = 2.0;
  dde::endvar();

  dde::var("y", true);
  double y = 0;
  dde::endvar();

  y = g(x);
  dde::stop();
  dde::dump_graph();

  return 0;
}
