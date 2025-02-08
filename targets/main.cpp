#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "dde.h"
#include "graph.h"
#include "mylib.h"
#include "mytime.h"

std::vector<ts::RuntimeStats> stats;

void newton(double);

int main() {
  std::vector<double> guesses = {-4.0, -2.0, 0.25, 1.25, 4.12, 6.0, 10.0};

  for (double &x0 : guesses) {
    newton(x0);
  }

  std::cout << "Stats:" << std::endl;
  std::cout << "Instrumentation Graph" << std::endl;
  for (auto &stat : stats) {
    std::string stat_str = "";
    for (auto &cp : stat.cps) {
      ts::TimeDiff dt = cp.toc - cp.tic;
      stat_str += std::to_string(dt.count()) + " ";
    }
    std::cout << stat_str << std::endl;
  }

  return 0;
}

void newton(double x0) {
  static const std::string GRAPH_PATH(
      "/home/hidaloop/.folder/random/pinenv/dde/scripts/prog.gr");
  static const double TOL = 1e-6;
  static const uint8_t MAX_ITER = 20;

  uint8_t i = 0;
  double eps = 10;

  do {
    // Forward step

    std::string str_i = std::to_string(i + 1);
    ts::CheckPoint inst("inst_" + str_i);
    ts::CheckPoint graph("graph_" + str_i);

    inst.tic = ts::clock::now();
    dde::start();

    dde::var("x");
    double x = x0;
    dde::endvar();

    dde::var("f_x", true);
    double f_x = 0;
    dde::endvar();

    f_x = mylib::f(x);
    dde::stop();
    inst.toc = ts::clock::now();

    // Newton update
    dde::dump_graph();

    graph.tic = ts::clock::now();
    Graph gr(GRAPH_PATH);

    gr.backprop();
    graph.toc = ts::clock::now();

    double df_dx = gr.parsed["x"]->der;

    x0 = x - f_x / df_dx;
    eps = std::abs(f_x);

    ts::RuntimeStats stat = {.tag = str_i, .cps = {inst, graph}};
    stats.push_back(stat);

  } while (++i < MAX_ITER && eps > TOL);

  std::cout << "Solution: " << x0 << " = " << x0 / 3.14159265359 << " Pi"
            << std::endl;
}
