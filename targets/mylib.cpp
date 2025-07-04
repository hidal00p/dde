#include "mylib.h"
#include <cmath>

namespace mylib {
double g(double x) { return (x - 1.345); }

double p(double x) { return (x + 4); }

double f1(double x) { return std::cos(x) * g(x) * p(x); }

double f2(double x) {

  double y = 1.0;
  for(int i = 0; i < 4; i++) {
    if (i % 2 == 0)
      y = std::sin(x + y);
    else
      y = std::cos(x + y);
  }

  return y;
}
} // namespace mylib
