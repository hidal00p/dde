#include "stdio.h"
#include "stdlib.h"

/*
 * Persistance on stack can be thought of as
 * node finalization.
 *
 * A node, however, may exist in a temporary state,
 * where it is not yet associated with a particular memory
 * location like a stack.
 *
 * The thing, actually, is that we probaly do not care
 * if a node was a part of a certain stack frame or not,
 * all we want the addresses for is to be able to do some
 * data dependance analysis for the computations in the
 * nearest vicinity.
 *
 * Hence, we can store a DAG as one data structure,
 * and accordingly we can store memory and register
 * maps that point to the latest node that was persisted
 * into them.
 *
 * (Maybe registers should resolve to nodes via the memory map somehow?)
 *
 */

/*
 * Dealing with fps has a slight difference, in particular,
 * because variable initialization begins by reading form memory.
 *
 */

int main() {
  double x = 1.5;
  double y = 1.25;
  double z = x * y * y;

  double w = z * x + 0.95;
  double v = z * y;

  double c = w * v;
  return 0;
}
