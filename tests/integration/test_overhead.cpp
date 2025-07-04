#include "dde.h"
#include "mytime.h"
#include <cmath>

#define TIME_DDE(name, func, iter)                                             \
  {                                                                            \
    ts::RuntimeStats rs;                                                       \
    rs.tag = name + std::string(" dde");                                       \
    for (int i = 0; i < iter; i++) {                                           \
      ts::CheckPoint cp("");                                                   \
      cp.tic = ts::clock::now();                                               \
      dde::start();                                                            \
      func();                                                                  \
      dde::stop();                                                             \
      cp.toc = ts::clock::now();                                               \
      rs.cps.push_back(cp);                                                    \
    }                                                                          \
    rs.show();                                                                 \
  }

void product_dde() {
  double x = 3.141;
  dde::var(&x, "x");
  double y = 2.718;
  double res = x * y;
}

void addition_dde() {
  double x = 3.141;
  dde::var(&x, "x");
  double y = 2.718;
  double res = x + y;
}

void subtract_dde() {
  double x = 3.141;
  dde::var(&x, "x");
  double y = 2.718;
  double res = x - y;
}

void divide_dde() {
  double x = 3.141;
  dde::var(&x, "x");
  double y = 2.718;
  double res = x / y;
}

void intrinsic_call_dde() {
  double x = 32.19525;
  dde::var(&x, "x");
  double res = std::sin(x);
}

void compound_dde() {
  double x = 3.141;
  dde::var(&x, "x");
  double y = 2.718;
  double res = std::cos(42.0 + std::cos(x) * std::sin(x) / (x + y) / (x * y));
}

void compound_sa_dde() {
  double x = 3.141;
  dde::var(&x, "x");
  double y = 2.718;
  double v0 = std::cos(x);
  double v1 = std::sin(y);
  double v2 = v0 * v1;
  double v3 = x + y;
  double v4 = x * y;
  double v5 = v3 / v4;
  double v6 = v2 / v5;
  double v7 = 42.0 + v6;
  double res = std::cos(v7);
}

int main() {
  int max_iter = 10000;
  int raw_factor = 1;

  TIME_DDE("mul", product_dde, max_iter)
  TIME_DDE("add", addition_dde, max_iter)
  TIME_DDE("sub", subtract_dde, max_iter)
  TIME_DDE("div", divide_dde, max_iter)
  TIME_DDE("sin", intrinsic_call_dde, max_iter)
  TIME_DDE("compound", compound_dde, max_iter)
  TIME_DDE("compound_sac", compound_sa_dde, max_iter)

  return 0;
}
