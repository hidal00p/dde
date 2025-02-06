#include "../lib/dde.h"
#include "graph.h"
#include "mylib.h"
#include "time.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

std::vector<ts::CheckPoint> cps;

void newton(double);

int main() {
  std::vector<double> guesses = {-4.0, -2.0, 4.12};

  for (double &x0 : guesses) {
    newton(x0);

    for (int i = 0; i < cps.size(); i += 5) {
      cps[i].show();
      cps[i + 1].show();
      cps[i + 2].show();
      cps[i + 3].show();
      cps[i + 4].show();
      std::cout << std::endl;
    }

    cps.clear();
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
    ts::CheckPoint iter("iter_" + str_i);
    ts::CheckPoint inst("inst_" + str_i);
    ts::CheckPoint gdump("gdump_" + str_i);
    ts::CheckPoint graph("graph_" + str_i);
    ts::CheckPoint bp("bp_" + str_i);

    iter.tic = ts::clock::now();
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
    gdump.tic = ts::clock::now();
    dde::dump_graph();
    gdump.toc = ts::clock::now();

    graph.tic = ts::clock::now();
    Graph gr(GRAPH_PATH);
    graph.toc = ts::clock::now();

    bp.tic = ts::clock::now();
    gr.backprop();
    bp.toc = ts::clock::now();

    double df_dx = gr.parsed["x"]->der;

    x0 = x - f_x / df_dx;
    eps = std::abs(f_x);
    iter.toc = ts::clock::now();

    cps.push_back(inst);
    cps.push_back(gdump);
    cps.push_back(graph);
    cps.push_back(bp);
    cps.push_back(iter);

  } while (++i < MAX_ITER && eps > TOL);

  std::cout << "Solution: " << x0 << " = " << x0 / 3.14159265359 << " Pi"
            << std::endl;
}
