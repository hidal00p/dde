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

#define TIME(name, func, iter)                                                 \
  {                                                                            \
    ts::RuntimeStats rs;                                                       \
    rs.tag = name;                                                             \
    for (int i = 0; i < iter; i++) {                                           \
      ts::CheckPoint cp("");                                                   \
      cp.tic = ts::clock::now();                                               \
      func();                                                                  \
      cp.toc = ts::clock::now();                                               \
      rs.cps.push_back(cp);                                                    \
    }                                                                          \
    rs.show();                                                                 \
  }

void product() {
  double x = 3.141;
  double y = 2.718;
  double res = x * y;
}

void product_dde() {
  double x = 3.141;
  dde::var(&x, "x");
  double y = 2.718;
  double res = x * y;
}

void addition() {
  double x = 3.141;
  double y = 2.718;
  double res = x + y;
}

void addition_dde() {
  double x = 3.141;
  dde::var(&x, "x");
  double y = 2.718;
  double res = x + y;
}

void subtract() {
  double x = 3.141;
  double y = 2.718;
  double res = x - y;
}

void subtract_dde() {
  double x = 3.141;
  dde::var(&x, "x");
  double y = 2.718;
  double res = x - y;
}

void divide() {
  double x = 3.141;
  double y = 2.718;
  double res = x / y;
}

void divide_dde() {
  double x = 3.141;
  dde::var(&x, "x");
  double y = 2.718;
  double res = x / y;
}

void intrinsic_call() {
  double x = 32.19525;
  double res = std::sin(x);
}

void intrinsic_call_dde() {
  double x = 32.19525;
  dde::var(&x, "x");
  double res = std::sin(x);
}

void compound() {
  double x = 3.141;
  double y = 2.718;
  double res = std::cos(42.0 + std::cos(x) * std::sin(x) / (x + y) / (x * y));
}

void compound_dde() {
  double x = 3.141;
  dde::var(&x, "x");
  double y = 2.718;
  double res = std::cos(42.0 + std::cos(x) * std::sin(x) / (x + y) / (x * y));
}

void compound_sa() {
  double x = 3.141;
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

  TIME("mul", product, max_iter * raw_factor)
  TIME_DDE("mul", product_dde, max_iter)

  TIME("add", addition, max_iter * raw_factor)
  TIME_DDE("add", addition_dde, max_iter)

  TIME("sub", subtract, max_iter * raw_factor)
  TIME_DDE("sub", subtract_dde, max_iter)

  TIME("div", divide, max_iter * raw_factor)
  TIME_DDE("div", divide_dde, max_iter)

  TIME("sin", intrinsic_call, max_iter * raw_factor)
  TIME_DDE("sin", intrinsic_call_dde, max_iter)

  TIME("compound", compound, max_iter * raw_factor)
  TIME_DDE("compound", compound_dde, max_iter)

  TIME("compound_sac", compound_sa, max_iter * raw_factor)
  TIME_DDE("compound_sac", compound_sa_dde, max_iter)

  return 0;
}
