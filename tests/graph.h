#ifndef GRAPH_H
#define GRAPH_H

#include<map>
#include <string>
#include <cstdint>

enum TransfType { IMUL = 0, ADD, SUB, NONE };

struct node;

struct transformation {
  TransfType type;
  node *args;
  uint8_t argc;
};

struct node {
  bool top;
  uint32_t identifier;
  transformation transf;
};

extern std::map<uint32_t, node*> known_node_buffer;

std::string transf_type_to_string(TransfType tt);
void traverse_node(const node* node, std::string prefix);
void show_graph();
void test_graph_construction();

#endif // GRAPH_H
