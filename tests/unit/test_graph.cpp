#include "cppunitlite/TestHarness.h"
#include "graph.h"
#include "testutils/exceptions.h"

TEST(test_uuid_generation) {
  std::vector<std::string> uuids;

  for (int i = 0; i < 200; i++)
    uuids.push_back(get_uuid());

  for (int i = 0; i < uuids.size(); i++) {
    for (int j = i + 1; j < uuids.size(); j++)
      CHECK(uuids[i] != uuids[j])
  }
}

class MemMapSetup : public TestSetup {
public:
  void setup() { mem_map.clear(); }

  void teardown() { mem_map.clear(); }
};

TESTWITHSETUP(test_mem_node_insertion, MemMapSetup) {
  NodePtr n = std::make_shared<Node>(42.0);
  mem::insert_node(1, n);
  CHECK(mem::is_node_recorded(1));
}

TESTWITHSETUP(test_mem_overwrite, MemMapSetup) {
  NodePtr n = std::make_shared<Node>(42.0);
  mem::insert_node(1, n);

  NodePtr new_node = std::make_shared<Node>(42.0);
  mem::insert_node(1, new_node);
  CHECK(mem::expect_node(1)->uuid == new_node->uuid);
}

TESTWITHSETUP(test_mem_node_move_between_addresses, MemMapSetup) {
  NodePtr n = std::make_shared<Node>(42.0);
  mem::insert_node(1, n);
  mem::write_to_mem(1, 2);
  CHECK(mem::expect_node(2)->uuid == mem::expect_node(1)->uuid);
}

TESTWITHSETUP(test_mem_access_at_invalid_address, MemMapSetup) {
  NodePtr n = std::make_shared<Node>(42.0);
  mem::insert_node(1, n);
  CHECK_FAILURE(mem::expect_node(2), NodeExpectedException);
}

class RegMapSetup : public TestSetup {
public:
  void setup() { reg_map.clear(); }

  void teardown() { reg_map.clear(); }
};

TESTWITHSETUP(test_reg_node_insertion, RegMapSetup) {
  NodePtr n = std::make_shared<Node>(42.0);
  reg::insert_node(1, n);
  CHECK(reg::is_node_recorded(1));
}

TESTWITHSETUP(test_reg_overwrite, RegMapSetup) {
  NodePtr n = std::make_shared<Node>(42.0);
  reg::insert_node(1, n);

  NodePtr new_node = std::make_shared<Node>(42.0);
  reg::insert_node(1, new_node);
  CHECK(reg::expect_node(1)->uuid == new_node->uuid);
}

TESTWITHSETUP(test_reg_node_move_between_regs, RegMapSetup) {
  NodePtr n = std::make_shared<Node>(42.0);
  reg::insert_node(1, n);
  reg::write_to_other_reg(1, 2);
  CHECK(reg::expect_node(2)->uuid == reg::expect_node(1)->uuid);
}

TESTWITHSETUP(test_reg_access_at_invalid_register, RegMapSetup) {
  NodePtr n = std::make_shared<Node>(42.0);
  reg::insert_node(1, n);
  CHECK_FAILURE(reg::expect_node(2), NodeExpectedException);
}

class MapSetup : public TestSetup {
public:
  void setup() {
    reg_map.clear();
    mem_map.clear();
  }

  void teardown() {
    reg_map.clear();
    mem_map.clear();
  }
};

TESTWITHSETUP(test_reg_mem_interconnect, MapSetup) {
  NodePtr n = std::make_shared<Node>(42.0);
  reg::insert_node(1, n);
  reg::write_to_mem(1, 1);
  CHECK(reg::expect_node(1)->uuid == mem::expect_node(1)->uuid);
}

TESTWITHSETUP(test_mem_reg_interconnect, MapSetup) {
  NodePtr n = std::make_shared<Node>(42.0);
  mem::insert_node(1, n);
  mem::write_to_reg(1, 1);
  CHECK(reg::expect_node(1)->uuid == mem::expect_node(1)->uuid);
}
