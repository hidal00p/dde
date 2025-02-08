#ifndef GRAPH_H
#define GRAPH_H

#include <fstream>
#include <map>
#include <string>
#include <vector>

#define NOP "\0"

struct Node {
  std::string uuid;
  std::string op;
  bool bop;
  double val;
  double der = 0.0;
  std::vector<Node *> parents;

  Node(std::string raw_repr);

  std::string str_code(std::string prefix = "");
  void differentiate();
};

class Graph {
private:
  std::ifstream graph_file;
  std::vector<Node *> topo;
  std::map<std::string, Node *> topo_visited;

public:
  Node *root;
  std::map<std::string, Node *> parsed;

  Graph(std::string file_name);

  Node *parse_node();

  void order_graph(Node *start_node);

  void backprop();
};

#endif
