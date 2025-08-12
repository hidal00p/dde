#include <cmath>
#include <iostream>

#include "dde.h"
#include "graph.h"

#define dde_instrument(call)                                                   \
  dde::start();                                                                \
  call;                                                                        \
  dde::stop()

double mycosinus(double x) { return std::cos(x); }

double f(double x) { return x * x * x; }

int main() {

  double x = 3.0;
  dde::var(&x, "x");
  dde_instrument(double f_x = f(x));
  dde::output(&f_x, "f_x");
  dde::dump_graph();

  Graph dag("/tmp/prog.gr");
  dag.order_graph(dag.root);
  dag.root->der = 1.0;

  for (auto node = dag.topo.rbegin(); node != dag.topo.rend(); node++) {
    dde::var(&((*node)->val), (*node)->uuid.c_str());
  }
  dde::var(&(dag.nodes["x"]->val), "x");
  dde_instrument(dag.eval_adjoints());
  dde::output(&(dag.nodes["x"]->der), "xd");
  dde::dump_graph();

  Graph dagg("/tmp/prog.gr");
  // restore initial computations
  for (auto node = dag.topo.rbegin(); node != dag.topo.rend(); node++) {
    dagg.swap_node_for(*node);
  }

  dagg.order_graph(dagg.root);
  dagg.root->der = 1.0;
  dagg.eval_adjoints();
  std::cout << dagg.nodes["x"]->der << std::endl;

  return 0;
}
