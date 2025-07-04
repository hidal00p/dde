#include "mylib.h"
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
} // namespace mylib
