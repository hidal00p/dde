#ifndef GRAPH_UTILS_H
#define GRAPH_UTILS_H

#include <map>
#include <memory>
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
enum Transformation { ASSIGN, MUL, ADD, DIV, SUB, CHS, SIN, COS };

std::string get_uuid();

struct Node;
using NodePtr = std::shared_ptr<Node>;
using NodePtrVec = std::vector<NodePtr>;

struct Node {
  std::string uuid;

  bool is_active = true;
  bool output = false;
  double value;

  NodePtrVec operands;
  Transformation transf = Transformation::ASSIGN;

  Node(double val) {
    uuid = get_uuid();
    value = val;
  }

  Node(double val, NodePtrVec oprs, Transformation trn) {
    uuid = get_uuid();
    value = val;
    operands = oprs;
    transf = trn;
  }

  bool is_leaf();
};

extern std::map<uint64_t, NodePtr> mem_map;
extern std::map<uint8_t, NodePtr> reg_map;

extern std::string graph_path;
typedef std::vector<std::string> uuid_list;
void show_node(NodePtr n);
void show_mem_map();
void clean_mem_map();

namespace mem {
void insert_node(uint64_t ef_addr, NodePtr n);
bool is_node_recorded(uint64_t ef_addr);
std::optional<NodePtr> get_node(uint64_t ef_addr);
NodePtr expect_node(uint64_t ef_addr);
void write_to_reg(uint64_t from_mem, uint64_t to_reg);
void write_to_mem(uint64_t from_mem, uint64_t to_mem);
} // namespace mem

namespace reg {
void insert_node(uint64_t reg, NodePtr n);
void clean_reg(uint64_t reg);
bool is_node_recorded(uint64_t reg);
std::optional<NodePtr> get_node(uint64_t reg);
NodePtr expect_node(uint64_t reg);
void write_to_mem(uint64_t from_reg, uint64_t to_mem);
void write_to_other_reg(uint64_t from_reg, uint64_t to_reg);
} // namespace reg

#endif
