#include "../lib/dde.h"
#include "graph.h"
#include <cassert>
#include <iostream>
#include <string>

// This would be effectively abstracted away from us
double g(double x) {
  double y = 3.0 * x;
  return y * y - 4.0 + y;
}
double f(double x) { return x * x - 4.0; }

int main() {
  uint8_t i = 0;
  double tmp_x = -10;
  double eps;
  std::string path("/home/hidaloop/.folder/random/pinenv/dde/scripts/prog.gr");

  do {
    // Forward step
    DDE_START
    DDE_VAR("x", double x = tmp_x)
    DDE_OUTPUT("f_x", double f_x = 0);
    f_x = f(x);
    DDE_STOP

    // Newton update
    DDE_DUMP_GRAPH
    Graph gr(path);
    gr.backprop();
    tmp_x = x - f_x / gr.parsed["x"]->der;
    eps = std::abs(f_x);

  } while (++i < 150 || eps > 1e-8);

  std::cout << tmp_x << std::endl;

  return 0;
}
