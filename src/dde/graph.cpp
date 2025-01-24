#include "graph.h"
#include <cassert>
#include <iostream>
#include <random>

std::map<uint64_t, node *> mem_map;
std::map<uint8_t, node *> reg_map;
std::vector<node *> fpu_stack;

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

namespace stack {
void push(node *n) {
  assert(fpu_stack.size() + 1 <= FPU_STACK_MAX_SIZE);
  fpu_stack.push_back(n);
}

node *pop() {
  assert(fpu_stack.size() > 0);

  node *n = fpu_stack.back();
  fpu_stack.pop_back();

  return n;
}

node *at(uint8_t idx) {
  assert(idx < FPU_STACK_MAX_SIZE);
  return fpu_stack[idx];
}

void at(uint8_t idx, node *n) {
  assert(idx < FPU_STACK_MAX_SIZE && idx < fpu_stack.size());
  fpu_stack[idx] = n;
}

node *top() {
  assert(fpu_stack.size() > 0);
  return fpu_stack.back();
}

uint8_t size() { return fpu_stack.size(); }
} // namespace stack

void show_node(node *n, std::string prefix) {
  std::cout << prefix << n->uuid << " " << n->value << " " << n->is_active;

  std::string tr_str = n->tr == transformation::NONE  ? ""
                       : n->tr == transformation::ADD ? "+"
                       : n->tr == transformation::MUL ? "*"
                       : n->tr == transformation::DIV ? "/"
                       : n->tr == transformation::SUB ? "-"
                                                      : "~";
  std::cout << " " << tr_str << std::endl;

  for (uint i = 0; i < n->n_operands; i++) {
    show_node(n->operands[i], prefix + " ");
  }
}

void show_mem_map() {
  for (const auto &[addr, n] : mem_map) {
    if (n->operands == nullptr)
      continue;
    show_node(n, "");
  }
}

namespace mem {
void insert_node(uint64_t ef_addr, node *n) { mem_map[ef_addr] = n; };

bool is_node_recorded(uint64_t ef_addr) { return mem_map.count(ef_addr) > 0; }

std::optional<node *> get_node(uint64_t ef_addr) {
  return is_node_recorded(ef_addr) ? std::optional<node *>{mem_map[ef_addr]}
                                   : std::nullopt;
}

node *expect_node(uint64_t ef_addr) {
  std::optional<node *> n = get_node(ef_addr);

  if (!n)
    assert(false);

  return n.value();
}

void write_to_reg(uint64_t from_mem, uint64_t to_reg) {
  std::optional<node *> n = get_node(from_mem);

  if (!n)
    assert(false);

  reg::insert_node(to_reg, n.value());
}

void write_to_mem(uint64_t from_mem, uint64_t to_mem) {
  std::optional<node *> n = get_node(from_mem);

  if (!n)
    assert(false);

  mem::insert_node(to_mem, n.value());
}
} // namespace mem

namespace reg {
void insert_node(uint64_t reg, node *n) { reg_map[reg] = n; };

bool is_node_recorded(uint64_t reg) { return reg_map.count(reg) > 0; }

std::optional<node *> get_node(uint64_t reg) {
  return is_node_recorded(reg) ? std::optional<node *>{reg_map[reg]}
                               : std::nullopt;
}

node *expect_node(uint64_t reg) {
  std::optional<node *> n = get_node(reg);

  if (!n)
    assert(false);

  return n.value();
}

void write_to_mem(uint64_t from_reg, uint64_t to_mem) {
  std::optional<node *> n = get_node(from_reg);

  if (!n)
    assert(false);

  mem::insert_node(to_mem, n.value());
}

void write_to_other_reg(uint64_t from_reg, uint64_t to_reg) {
  std::optional<node *> n = get_node(from_reg);

  if (!n)
    assert(false);

  reg::insert_node(to_reg, n.value());
}
} // namespace reg
