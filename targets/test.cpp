
// int main() {
//   double x = 42.0;
//   double y = 12.0;
//
//   double z = x + y;
//   z = x * y / y;
//
//   z = f(z);
// }

#include "dde.h"

double f(double x) { return x * x; }

int main() {
  dde::start();

  dde::var("x");
  double x = 42.42;
  dde::endvar();

  double y = x;

  double z = 32.32;

  dde::stop();
}
