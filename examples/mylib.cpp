#include "mylib.h"
#include <array>
#include <cmath>

namespace mylib {
double mysin(double x) { return std::sin(x); }
double mycos(double x) { return std::cos(x); }
double g(double x) { return (x - 1.345); }

double p(double x) { return (x + 4); }

double f1(double x) { return mycos(x) * g(x) * p(x); }

double f2(double x) {
  double y = 1.0;
  for (int i = 0; i < 4; i++) {
    if (i % 2 == 0)
      y = mysin(x + y);
    else
      y = mycos(x + y);
  }
  return y;
}

double rosenbrock(std::array<double, 2> &x, double a, double b) {
  double v0 = a - x[0];
  double v1 = v0 * v0;

  double v2 = x[1] - x[0] * x[0];
  double v3 = v2 * v2;
  return v1 + b * v3;
}
} // namespace mylib
