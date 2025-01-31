#include "../lib/dde.h"
#include "graph.h"
#include <cassert>
#include <chrono>
#include <iostream>
#include <string>

constexpr double TOL = 1e-6;
constexpr uint8_t MAX_ITER = 20;
constexpr char const *DELIM = "========";
const static std::string
    GRAPH_PATH("/home/hidaloop/.folder/random/pinenv/dde/scripts/prog.gr");

void newton(double);

int main() {
  std::vector<double> guesses = {-100.0, -10, 10, 100.0};
  std::cout << DELIM << std::endl;
  for (double &x0 : guesses) {
    auto tic = std::chrono::high_resolution_clock::now();
    newton(x0);
    auto toc = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> dt = toc - tic;
    std::cout << "Exec time: " << dt.count() << "ms" << std::endl;
    std::cout << DELIM << std::endl;
  }

  return 0;
}

namespace lib {
double g(double x) { return (x - 1.345); }

double p(double x) {
  double y = (x + 6.09);
  double z = (x * x - 1764); // +-42
  return y * z;
}

double f(double x) { return g(x) * p(x); }
} // namespace lib

void newton(double x0) {
  uint8_t i = 0;
  double eps = 10;
  do {
    // Forward step
    // TODO:
    // 1. Function calls look less magical than macros
    // 2. Var marking after declaration.
    DDE_START
    DDE_VAR("x", double x = x0)
    DDE_OUTPUT("f_x", double f_x = 0)
    f_x = lib::f(x);
    DDE_STOP

    // Newton update
    DDE_DUMP_GRAPH
    Graph gr(GRAPH_PATH);
    gr.backprop();
    double df_dx = gr.parsed["x"]->der;

    x0 = x - f_x / df_dx;
    eps = std::abs(f_x);

  } while (++i < MAX_ITER && eps > TOL);
  std::cout << "Solution: " << x0 << std::endl;
}
