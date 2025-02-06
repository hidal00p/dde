#include "../lib/dde.h"
#include <chrono>
#include <cmath>
#include <iostream>

#define DELIM "============"
typedef std::chrono::duration<double, std::milli> TimeDiff;

double g(double x) { return x * x * std::cos(x); }

void log_duration(TimeDiff dt) {
  std::cout << "Exec time: " << dt.count() << "ms" << std::endl;
  std::cout << DELIM << std::endl;
}

int main() {
  for (int i = 0; i < 5; i++) {
    auto tic = std::chrono::high_resolution_clock::now();

    {
      dde::start();

      dde::var("x");
      double x = 2.0;
      dde::endvar();

      dde::var("y", true);
      double y = 0;
      dde::endvar();

      y = g(x);

      dde::stop();
      dde::dump_graph();
    }

    auto toc = std::chrono::high_resolution_clock::now();
    log_duration(toc - tic);
  }

  return 0;
}
