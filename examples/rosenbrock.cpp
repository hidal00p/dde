#include "dde.h"
#include "graph.h"
#include "mylib.h"

#include <array>
#include <iostream>
#include <random>
#include <string>

std::string graph_path("/tmp/prog.gr");

typedef struct minimum_ctx_ {
  std::array<double, 2> x;
  double f;
  int trial;

  void echo() {
    std::cout << "Best minimal value:" << std::endl;
    std::cout << "\tf(x) = " << f << " occurred at:" << std::endl;
    std::cout << "\tx0 = " << x[0] << std::endl;
    std::cout << "\tx1 = " << x[1] << std::endl;
    std::cout << "\ttrial = " << trial << std::endl;
  }
} minimum_ctx;

int usage() {
  std::cout
      << "Provide 2 cli arguments to the program:" << std::endl
      << "\t1) Number of random restarts\n\t2) Number of epochs for the search"
      << std::endl
      << "./rosenbrock.exe <n-random-restarts> <n-epochs>" << std::endl;
  return 1;
}

void display_search_limits(double ll, double ul) {
  std::cout << "Searching in the square x0 * x1 := (" << ll << ", " << ul
            << ") * "
            << "(" << ll << ", " << ul << ")" << std::endl;
  std::cout << "(x1) |" << std::endl;
  std::cout << "     +------+" << std::endl;
  std::cout << "     |      |" << std::endl;
  std::cout << "     |      |" << std::endl;
  std::cout << "     +------+ __" << std::endl;
  std::cout << "              (x0)" << std::endl;
}

int main(int argc, char **argv) {
  if (argc != 3)
    return usage();

  int n_random_res = std::stoi(argv[1]);
  int n_epochs = std::stoi(argv[2]);

  minimum_ctx min;

  double lower_lim = -5.0, upper_lim = 5.0;
  std::random_device rand_dev;
  std::mt19937 gen(rand_dev());
  std::uniform_real_distribution<double> distr(lower_lim, upper_lim);
  display_search_limits(lower_lim, upper_lim);

  double alpha = 0.00005;
  for (int random_trial = 0; random_trial < n_random_res; random_trial++) {
    std::array<double, 2> x{distr(gen), distr(gen)};
    double f_x;

    std::cout << "Trial: " << random_trial + 1 << std::endl;
    for (int epoch = 0; epoch < n_epochs; epoch++) {
      for (int i = 0; i < x.size(); i++) {
        dde::var(&x[i], "x", i);
      }

      dde::start();
      f_x = mylib::rosenbrock(x);
      dde::stop();
      dde::output(&f_x, "f_x");

      dde::dump_graph();
      Graph dag(graph_path);
      dag.eval_adjoints();

      for (int i = 0; i < x.size(); i++) {
        x[i] -= alpha * dag.nodes["x" + std::to_string(i)]->der;
      }
    }

    std::cout << "\tdescended to f = " << f_x << "; at " << x[0] << ", " << x[1]
              << std::endl;

    if (random_trial == 0 || f_x < min.f) {
      min.x[0] = x[0];
      min.x[1] = x[1];
      min.f = f_x;
      min.trial = random_trial + 1;
    }
  }

  min.echo();
  return 0;
}
