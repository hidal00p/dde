#include "../lib/dde.h"
#include <cassert>
#include <string>

#define NINLINE

// This would be effectively abstracted away from us
#ifdef INLINE
__attribute__((always_inline)) inline
#endif
    double
    f(double &x) {
  return 3.0 * x * x - 4.0 * x + 5;
}

int main() {
  DDE_START
  DDE_MARK("x", double x = 3.0)
  double y = f(x);
  DDE_STOP

  return 0;
}
