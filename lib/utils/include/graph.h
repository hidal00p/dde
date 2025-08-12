#ifndef GRAPH_H
#define GRAPH_H

#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <memory>

#define NOP "\0"

struct Node;
using NodePtr = std::shared_ptr<Node>;
using NodePtrVec = std::vector<NodePtr>;

struct Node {
  std::string uuid;
  std::string op;
  bool bop;
  double val;
  double der = 0.0;
  NodePtrVec parents;

  Node(std::string raw_repr);

  std::string str_code(std::string prefix = "");
  void differentiate();
};

class Graph {
private:
  std::ifstream graph_file;
  std::map<std::string, NodePtr> topo_visited;

public:
  NodePtrVec topo;
  NodePtr root;
  std::map<std::string, NodePtr> nodes;

  Graph(std::string file_name);

  NodePtr parse_node();

  void order_graph(NodePtr start_node);

  void eval_adjoints();
};

#endif
