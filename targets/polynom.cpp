#include "../lib/dde.h"
#include <cassert>
#include <string>

// This would be effectively abstracted away from us
double f(double x) { return 3 * x * x - 4.0; }

int main() {

  DDE_START
  double x = 3.0;
  double y = f(x);
  DDE_STOP

  return 0;
}

/*
int main(int argc, char *argv[]) {
  assert(argc == 2);

  double x = std::atof(argv[1]);

#pragma dde_record_graph
  double y = f(x);
#pragma dde_record_graph_end

#pragma dde_get_graph
  graph g = {.i = 0};
#pragma dde_get_graph_end

  return 0;
}
*/
