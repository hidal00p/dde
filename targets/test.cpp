#include "dde.h"
double f(double x) { return x * x; }

// int main() {
//   double x = 42.0;
//   double y = 12.0;
//
//   double z = x + y;
//   z = x * y / y;
//
//   z = f(z);
// }

int main() {
  dde::start();

  dde::var("x");
  double x = 2.0;
  dde::endvar();

  double z = 3.0;

  double y1 = x * x;
  double y2 = x + z;
  double y3 = y1 * y2;

  dde::stop();
}
