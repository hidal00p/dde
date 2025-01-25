#ifndef GRAPH_UTILS_H
#define GRAPH_UTILS_H

#include <map>
#include <optional>
#include <string>
#include <vector>

/*
 * Using the following two maps we attempt to resolve
 * every memory initialization with a value.
 *
 * We also keep track of final memory values.
 *
 * NOTE: might want to wrap things into a shared_ptr
 */
enum transformation { NONE, MUL, ADD, DIV, SUB, CHS };

std::string get_uuid();

struct node;

struct node {
  std::string uuid;
  bool is_active = true;
  double value;
  node **operands = nullptr;
  uint8_t n_operands = 0;
  transformation tr = transformation::NONE;
  bool output = false;

  node(double val) {
    uuid = get_uuid();
    value = val;
  }

  node(double val, uint8_t n_oprs, node **oprs, transformation trn) {
    uuid = get_uuid();
    value = val;
    operands = oprs;
    n_operands = n_oprs;
    tr = trn;
  }
};

extern std::map<uint64_t, node *> mem_map;
extern std::map<uint8_t, node *> reg_map;

#define FPU_STACK_MAX_SIZE 8
extern std::vector<node *> fpu_stack;

namespace stack {
void push(node *n);
node *pop();
node *at(uint8_t idx);
void at(uint8_t idx, node *n);
node *top();
uint8_t size();
} // namespace stack

void show_node(node *n);
void show_mem_map();

namespace mem {
void insert_node(uint64_t ef_addr, node *n);
bool is_node_recorded(uint64_t ef_addr);
std::optional<node *> get_node(uint64_t ef_addr);
node *expect_node(uint64_t ef_addr);
void write_to_reg(uint64_t from_mem, uint64_t to_reg);
void write_to_mem(uint64_t from_mem, uint64_t to_mem);
} // namespace mem

namespace reg {
void insert_node(uint64_t reg, node *n);
bool is_node_recorded(uint64_t reg);
std::optional<node *> get_node(uint64_t reg);
node *expect_node(uint64_t reg);
void write_to_mem(uint64_t from_reg, uint64_t to_mem);
void write_to_other_reg(uint64_t from_reg, uint64_t to_reg);
} // namespace reg

#endif
