#include <cassert>
#include <cmath>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "dde.h"
#include "graph.h"
#include "mylib.h"

std::string graph_path("/tmp/prog.gr");
bool parse_args(int argc, char *argv[]);
void newton(double, std::function<double(double)> f);

int main() {
  std::vector<double> guesses = {-3.48, -2.0, 0.25, 3.0, 6.0, 10.0};

  for (double &x0 : guesses)
    newton(x0, mylib::f1);

  for (double &x0 : guesses)
    newton(x0, mylib::f2);

  return 0;
}

void newton(double x0, std::function<double(double)> f) {

  constexpr double TOL = 1e-6;
  constexpr uint8_t MAX_ITER = 20;

  uint8_t i = 0;
  double eps = 10;

  std::cout << "Guess: " << x0 << " = " << x0 / M_PI << " Pi" << std::endl;
  do {
    // Forward step
    double x = x0;

    dde::start();
    dde::var(&x, "x");

    double f_x = f(x);
    dde::output(&f_x, "f_x");
    dde::stop();

    // Newton update
    dde::dump_graph();

    Graph dag(graph_path);
    dag.eval_adjoints();
    double df_dx = dag.nodes["x"]->der;

    x0 = x - f_x / df_dx;
    eps = std::abs(f_x);

  } while (++i < MAX_ITER && eps > TOL);

  std::cout << "Solution: " << x0 << " = " << x0 / M_PI << " Pi" << std::endl
            << std::endl;
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
