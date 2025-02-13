#include "cppunitlite/TestHarness.h"
#include "graph.h"

#include "vector"

TEST(uuid_generation, GraphTest1) {
  std::vector<std::string> uuids;

  for (int i = 0; i < 200; i++)
    uuids.push_back(get_uuid());

  for (int i = 0; i < uuids.size(); i++) {
    for (int j = i + 1; j < uuids.size(); j++)
      CHECK(uuids[i] != uuids[j])
  }
}

class StackSetup : public TestSetup {
public:
  void setup() { fpu_stack.clear(); }

  void teardown() { fpu_stack.clear(); }
};

TESTWITHSETUP(Stack, StackTest1) {
  node *n1 = new node(2.0);
  stack::push(n1);
  CHECK(n1->uuid == stack::top()->uuid);

  node *n2 = new node(2.0);
  stack::push(n2);
  CHECK(n2->uuid == stack::top()->uuid);
}

TESTWITHSETUP(Stack, StackTest2) {
  node *n = new node(42.0);

  stack::push(n);
  stack::push(n);

  CHECK(stack::size() == 2);

  stack::pop();
  stack::pop();

  CHECK(stack::size() == 0);
}
