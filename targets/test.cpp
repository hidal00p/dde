#include "dde.h"
#include "mylib.h"
#include <cassert>
#include <vector>

void dot(std::vector<double> &a, std::vector<double> &b, double &y) {
  assert(a.size() == b.size());
  for (int i = 0; i < a.size(); i++) {
    y += a[i] * b[i];
  }
}

int main(int argc, char **argv) {
  assert(argc == 2);
  int use_case = std::atoi(argv[1]);

  if (use_case == 1) {
    double x = 3.12;
    dde::start();
    dde::var(&x, "x");
    double y = mylib::f2(x);
    dde::output(&y, "f");
    dde::stop();
    dde::dump_graph();
  } else {
    std::vector<double> a = {1, 1};
    std::vector<double> b = {1, 2};

    dde::start();
    for (int i = 0; i < a.size(); i++) {
      dde::var(&a[i], "a", i);
      dde::var(&b[i], "b", i);
    }

    double yd = 0;
    dot(a, b, yd);
    dde::output(&yd, "dot");
    dde::stop();
    dde::dump_graph();
  }

  return 0;
}
