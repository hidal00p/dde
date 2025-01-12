#include <cassert>
#include <string>

double f(double x) { return 2 * x * x * x + 5 * x * x - 4 * x - 3; }

int main(int argc, char *argv[]) {
  assert(argc == 2);

  double x = std::atof(argv[1]);
  double y = f(x);

  return 0;
}
