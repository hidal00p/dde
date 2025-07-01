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

  double y = 2.0;

  for (int i = 0; i < 3; i++)
    x *= y;

  double z = 4.0;
  double y1 = (x / z) - z;

  dde::stop();
}
