#include "graph.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>

#ifdef TEST_MODE
#include "testutils/exceptions.h"
#endif

std::map<uint64_t, NodePtr> mem_map;
std::map<uint8_t, NodePtr> reg_map;

std::string get_transf_text(Transformation tr) {
  std::string tr_str;
  switch (tr) {
  case Transformation::ASSIGN:
    tr_str = "";
    break;
  case Transformation::MUL:
    tr_str = "*";
    break;
  case Transformation::ADD:
    tr_str = "+";
    break;
  case Transformation::DIV:
    tr_str = "/";
    break;
  case Transformation::SUB:
    tr_str = "-";
    break;
  case Transformation::CHS:
    tr_str = "~";
    break;
  case Transformation::SIN:
    tr_str = "sin";
    break;
  case Transformation::COS:
    tr_str = "cos";
    break;
  case Transformation::EXP:
    tr_str = "exp";
    break;
  }
  return tr_str;
}

std::string get_uuid() {
  static const uint id_length = 7;
  static const std::string alphabet = "aAbBcCdDeEfFgGhHiIjJ0123456789-+=";

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distr(0, alphabet.size() - 1);

  std::string uuid = "";
  for (uint i = 0; i < id_length; i++) {
    uuid += alphabet[distr(gen)];
  }

  return uuid;
}

bool is_visited(std::string uuid, uuid_list visited) {
  return std::find(visited.begin(), visited.end(), uuid) != visited.end();
}

bool Node::is_leaf() { return !operands.empty() && output; }

void show_node(NodePtr n, std::string prefix, uuid_list &visited,
               std::ostream &out) {
  out << prefix << n->uuid << " " << n->value << " " << n->is_active << " "
      << get_transf_text(n->transf) << std::endl;

  if (is_visited(n->uuid, visited)) {
    return;
  }

  for (auto &operand : n->operands) {
    show_node(operand, prefix + " ", visited, out);
    visited.push_back(n->uuid);
  }
}
void show_node(NodePtr n, std::string prefix) {

  std::cout << prefix << n->uuid << " " << n->value << " " << n->is_active
            << " " << get_transf_text(n->transf) << std::endl;

  for (auto &operand : n->operands) {
    show_node(operand, prefix + " ");
  }
}
void show_mem_map(std::ostream &out) {
  uuid_list visited;

  for (const auto &[addr, n] : mem_map) {
    if (!n->is_leaf() || is_visited(n->uuid, visited))
      continue;

    uuid_list visited_parents;
    show_node(n, "", visited_parents, out);
    visited.push_back(n->uuid);
  }
}

void clean_mem_map() { mem_map.clear(); }

namespace mem {
void insert_node(uint64_t ef_addr, NodePtr n) { mem_map[ef_addr] = n; };

void clean_mem(uint64_t ef_addr) { mem_map.erase(ef_addr); }
bool is_node_recorded(uint64_t ef_addr) { return mem_map.count(ef_addr) > 0; }

std::optional<NodePtr> get_node(uint64_t ef_addr) {
  return is_node_recorded(ef_addr) ? std::optional<NodePtr>{mem_map[ef_addr]}
                                   : std::nullopt;
}

NodePtr expect_node(uint64_t ef_addr) {
  std::optional<NodePtr> n = get_node(ef_addr);

#ifndef TEST_MODE
  if (!n) {
    assert(false);
  }
#else
  if (!n)
    throw NodeExpectedException("Expected node in memory at address " +
                                std::to_string(ef_addr));
#endif

  return n.value();
}

void write_to_reg(uint64_t from_mem, uint64_t to_reg) {
  reg::insert_node(to_reg, expect_node(from_mem));
}

void write_to_mem(uint64_t from_mem, uint64_t to_mem) {
  mem::insert_node(to_mem, expect_node(from_mem));
}
} // namespace mem

namespace reg {
void insert_node(uint64_t reg, NodePtr n) { reg_map[reg] = n; };

void clean_reg(uint64_t reg) { reg_map.erase(reg); }

bool is_node_recorded(uint64_t reg) { return reg_map.count(reg) > 0; }

std::optional<NodePtr> get_node(uint64_t reg) {
  return is_node_recorded(reg) ? std::optional<NodePtr>{reg_map[reg]}
                               : std::nullopt;
}

NodePtr expect_node(uint64_t reg) {
  std::optional<NodePtr> n = get_node(reg);

#ifndef TEST_MODE
  if (!n)
    assert(false);
#else
  if (!n)
    throw NodeExpectedException("Expected node in reg  " + std::to_string(reg));
#endif

  return n.value();
}

void write_to_mem(uint64_t from_reg, uint64_t to_mem) {
  mem::insert_node(to_mem, expect_node(from_reg));
}

void write_to_other_reg(uint64_t from_reg, uint64_t to_reg) {
  reg::insert_node(to_reg, expect_node(from_reg));
}
} // namespace reg
