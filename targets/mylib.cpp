#include "mylib.h"
#include <cmath>

namespace mylib {
double g(double x) { return (x - 1.345); }

double p(double x) { return (x - 4); }

double f(double x) { return std::cos(x) * g(x) * p(x); }
} // namespace mylib
