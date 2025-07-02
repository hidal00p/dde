#include "dde.h"
double f(double x) { return x * x; }

int main() {
  dde::start();

  dde::var("x");
  double x = 2.0;
  dde::endvar();

  dde::var("y", true);
  double y = 0.0;
  dde::endvar();

  double z = 2.0;
  double y1 = (f(x) / z) - y;

  dde::stop();
}
