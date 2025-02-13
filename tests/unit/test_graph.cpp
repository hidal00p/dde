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
  node *n = new node(42.0);
  stack::push(n);
  CHECK(n->uuid == stack::top()->uuid);
}

TESTWITHSETUP(Stack, StackTest2) {
  node *n = new node(42.0);

  stack::push(n);
  CHECK(stack::size() == 1);

  stack::pop();
  CHECK(stack::size() == 0);
}

class MemMapSetup : public TestSetup {
public:
  void setup() { mem_map.clear(); }

  void teardown() { mem_map.clear(); }
};

TESTWITHSETUP(MemMap, MapTest1) {
  node* n = new node(42.0);
  mem::insert_node(1, n);
  CHECK(mem::is_node_recorded(1));
}

TESTWITHSETUP(MemMap, MapTest2) {
  node* n = new node(42.0);
  mem::insert_node(1, n);
  mem::write_to_mem(1, 2);
  CHECK(mem::expect_node(2)->uuid == mem::expect_node(1)->uuid);
}

class RegMapSetup : public TestSetup {
public:
  void setup() { reg_map.clear(); }

  void teardown() { reg_map.clear(); }
};

TESTWITHSETUP(RegMap, RegTest1) {
  node* n = new node(42.0);
  reg::insert_node(1, n);
  CHECK(reg::is_node_recorded(1));
}

TESTWITHSETUP(RegMap, RegTest2) {
  node* n = new node(42.0);
  reg::insert_node(1, n);
  reg::write_to_other_reg(1, 2);
  CHECK(reg::expect_node(2)->uuid == reg::expect_node(1)->uuid);
}
