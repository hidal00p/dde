#include "dde.h"

int main() {
  dde::start();

  dde::var("x", true);
  double x = 42.42;
  dde::endvar();

  double y = x;

  double z = 32.32;

  dde::stop();

  dde::dump_graph();
}
