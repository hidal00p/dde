#ifndef GRAPH_H
#define GRAPH_H

#include <cstdint>
#include <map>
#include <string>

enum TransfType { IMUL = 0, ADD, SUB, NONE };

struct node;

struct transformation {
  TransfType type;
  node **args;
  uint8_t argc;
  transformation() : type(TransfType::NONE), args(nullptr), argc(0) {}
};

struct node {
  bool top;
  uint32_t id;
  transformation transf;

  node() : top(false), id(0), transf(transformation()) {}
  node(uint32_t id) : top(false), id(id), transf(transformation()) {}
};

extern std::map<uint32_t, node *> known_node_buffer;

void register_new_transformation(uint32_t op1_id, uint32_t op2_id,
                                 uint32_t rest_id);
std::string transformation_to_str(TransfType tt);
void show_node(node *node, std::string prefix);
void show_graph();

#endif // GRAPH_H
