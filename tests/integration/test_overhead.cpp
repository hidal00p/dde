#include <dde.h>
#include <mytime.h>
#define TIME_DDE(name, func, iter)                                             \
  {                                                                            \
    ts::RuntimeStats rs;                                                       \
    rs.tag = name;                                                             \
    rs.tag += "_dde";                                                          \
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

void addition() {
  double x = 3.141;
  double y = 2.718;
  double res = x * y;
}

void subtract() {
  double x = 3.141;
  double y = 2.718;
  double res = x * y;
}

void divide() {
  double x = 3.141;
  double y = 2.718;
  double res = x * y;
}

int main() {
  TIME("mul", product, 500)
  TIME_DDE("mul", product, 500)

  TIME("add", addition, 500)
  TIME_DDE("add", addition, 500)

  TIME("sub", subtract, 500)
  TIME_DDE("sub", subtract, 500)

  TIME("div", divide, 500)
  TIME_DDE("div", divide, 500)

  return 0;
}
