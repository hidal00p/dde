#ifndef MYLIB_H
#define MYLIB_H

#include <array>

namespace mylib {
double f1(double x);
double f2(double x);
double rosenbrock(std::array<double, 2> &x, double a = 1.0, double b = 100.0);
} // namespace mylib

#endif
