#include "dde.h"
#include <cassert>
#include <vector>

void dot(std::vector<double> &a, std::vector<double> &b, double &y) {
  assert(a.size() == b.size());
  for (int i = 0; i < a.size(); i++) {
    y += a[i] * b[i];
  }
}

double f(double x) { return x * x + 1.345; }

int main() {

  double x = 3.12;
  dde::start();
  dde::var(&x, "x");

  double y = f(x);
  dde::output(&y, "f");

  dde::stop();

  return 0;
}
