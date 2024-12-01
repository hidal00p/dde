#include "graph_utils.h"
#include <cassert>

int main() {
  node* start = new node(2.0);
  node* mid = new node(3.0);
  node* end = new node(4.0);

  stack::push(start);
  stack::push(mid);
  stack::push(end);

  assert(fpu_stack.size() == 3);
  assert(stack::at(0)->uuid == start->uuid);
  assert(stack::at(1)->uuid == mid->uuid);
  assert(stack::at(2)->uuid == end->uuid && stack::back()->uuid == end->uuid);

  assert(stack::pop()->uuid == end->uuid);
  assert(fpu_stack.size() == 2);
  assert(stack::back()->uuid == mid->uuid);

  assert(stack::pop()->uuid == mid->uuid);
  assert(fpu_stack.size() == 1);
  assert(stack::back()->uuid == start->uuid);

  assert(stack::pop()->uuid == start->uuid);
  assert(fpu_stack.size() == 0);

  return 0;
}
