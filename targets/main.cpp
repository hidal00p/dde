#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "dde.h"
#include "graph.h"
#include "mylib.h"

std::string graph_path("/tmp/prog.gr");
bool parse_args(int argc, char *argv[]);
void newton(double);

int main() {
  std::vector<double> guesses = {-4.0, -2.0, 0.25, 4.12, 6.0, 10.0};

  for (double &x0 : guesses)
    newton(x0);

  return 0;
}

void newton(double x0) {

  constexpr double TOL = 1e-6;
  constexpr uint8_t MAX_ITER = 20;
  constexpr float pi = 3.14159265359;

  uint8_t i = 0;
  double eps = 10;

  do {
    // Forward step
    dde::start();

    dde::var("x");
    double x = x0;
    dde::endvar();

    dde::var("f_x", true);
    double f_x = 0;
    dde::endvar();

    f_x = mylib::f(x);
    dde::stop();

    // Newton update
    dde::dump_graph();

    Graph gr(graph_path);
    gr.backprop();
    double df_dx = gr.parsed["x"]->der;

    x0 = x - f_x / df_dx;
    eps = std::abs(f_x);

  } while (++i < MAX_ITER && eps > TOL);

  std::cout << "Solution: " << x0 << " = " << x0 / pi << " Pi" << std::endl;
}

bool parse_args(int argc, char *argv[]) {
  constexpr uint8_t def_arg_count = 1;

  if (argc == def_arg_count)
    return false;

  if (argc > def_arg_count + 1)
    return true;

  std::string arg_path(argv[def_arg_count]);
  graph_path = arg_path;

  return false;
}
