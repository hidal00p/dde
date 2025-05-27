#ifndef GRAPH_UTILS_H
#define GRAPH_UTILS_H

#include <map>
#include <optional>
#include <string>
#include <vector>
#include <memory>

/*
 * Using the following two maps we attempt to resolve
 * every memory initialization with a value.
 *
 * We also keep track of final memory values.
 *
 * NOTE: might want to wrap things into a shared_ptr
 */
enum transformation { NONE, MUL, ADD, DIV, SUB, CHS, SIN, COS };

std::string get_uuid();

struct Node;
using NodePtr = std::shared_ptr<Node>;

struct Node {
  std::string uuid;
  bool is_active = true;
  double value;
  Node **operands = nullptr;
  uint8_t n_operands = 0;
  transformation tr = transformation::NONE;
  bool output = false;

  Node(double val) {
    uuid = get_uuid();
    value = val;
  }

  Node(double val, uint8_t n_oprs, Node **oprs, transformation trn) {
    uuid = get_uuid();
    value = val;
    operands = oprs;
    n_operands = n_oprs;
    tr = trn;
  }

  bool is_leaf();
};

extern std::map<uint64_t, Node *> mem_map;
extern std::map<uint8_t, Node *> reg_map;

#define FPU_STACK_MAX_SIZE 8
extern std::vector<Node *> fpu_stack;

namespace stack {
void push(Node *n);
Node *pop();
Node *at(uint8_t idx);
void at(uint8_t idx, Node *n);
Node *top();
uint8_t size();
} // namespace stack

extern std::string graph_path;
typedef std::vector<std::string> uuid_list;
void show_node(Node *n);
void show_mem_map();
void clean_mem_map();

namespace mem {
void insert_node(uint64_t ef_addr, Node *n);
bool is_node_recorded(uint64_t ef_addr);
std::optional<Node *> get_node(uint64_t ef_addr);
Node *expect_node(uint64_t ef_addr);
void write_to_reg(uint64_t from_mem, uint64_t to_reg);
void write_to_mem(uint64_t from_mem, uint64_t to_mem);
} // namespace mem

namespace reg {
void insert_node(uint64_t reg, Node *n);
bool is_node_recorded(uint64_t reg);
std::optional<Node *> get_node(uint64_t reg);
Node *expect_node(uint64_t reg);
void write_to_mem(uint64_t from_reg, uint64_t to_mem);
void write_to_other_reg(uint64_t from_reg, uint64_t to_reg);
} // namespace reg

#endif
