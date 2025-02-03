#include "../lib/dde.h"
#include "graph.h"
#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <string>

constexpr double TOL = 1e-6;
constexpr uint8_t MAX_ITER = 20;
constexpr char const *DELIM = "========";
const static std::string
    GRAPH_PATH("/home/hidaloop/.folder/random/pinenv/dde/scripts/prog.gr");

void newton(double);

int main() {
  std::vector<double> guesses = {-4.0, -2.0};
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

double p(double x) { return std::cos(x); }

double f(double x) { return std::cos(x) * (x - 4); }
} // namespace lib

void newton(double x0) {
  uint8_t i = 0;
  double eps = 10;
  do {
    // Forward step
    // TODO:
    // 2. Var marking after declaration.
    dde::start();

    dde::var("x");
    double x = x0;
    dde::endvar();

    dde::var("f_x", true);
    double f_x = 0;
    dde::endvar();

    f_x = lib::f(x);
    dde::stop();

    // Newton update
    dde::dump_graph();

    Graph gr(GRAPH_PATH);
    gr.backprop();
    double df_dx = gr.parsed["x"]->der;

    x0 = x - f_x / df_dx;
    eps = std::abs(f_x);

  } while (++i < MAX_ITER && eps > TOL);
  std::cout << "Solution: " << x0 << " = " << x0 / 3.14159265359 << "Pi"
            << std::endl;
}
