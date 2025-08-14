#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "graph.h"

#define NOP "\0"

double _sin_(double x) {
  return std::sin(x);
}
double _cos_(double x) {
  return std::cos(x);
}

double _exp_(double x) {
  return std::exp(x);
}

Node::Node(std::string raw_repr) {
  std::vector<std::string> tokens;
  std::string token;

  for (char &c : raw_repr) {
    bool term_char = c == ' ';
    if (term_char && token.empty()) {
      continue;
    } else if (term_char) {
      tokens.push_back(token);
      token.clear();
    } else {
      token += c;
    }
  }

  if (!token.empty()) {
    tokens.push_back(token);
  }

  uuid = tokens[0];
  val = std::stod(tokens[1]);
  op = tokens.size() != 4 ? NOP : tokens[3];

  static std::string BINARY_OPS[] = {"+", "-", "/", "*"};
  static uint8_t bop_size = sizeof(BINARY_OPS) / sizeof(BINARY_OPS[0]);

  bop = false;
  for (uint8_t i = 0; i < bop_size; i++) {
    bop |= BINARY_OPS[i] == op;
    if (bop) {
      break;
    }
  }
}

std::string Node::str_code(std::string prefix) {
  std::string code;
  code += prefix;
  code += uuid;
  code += " ";
  code += std::to_string(val);
  if (op != "\0") {
    code += " ";
    code += op;
  }
  code += "\n";

  for (auto &p : parents) {
    code += p->str_code(prefix + " ");
  }

  return code;
}

void Node::differentiate() {
  if (op == NOP) {
    return;
  }

  if (bop) {
    NodePtr rhs1 = parents[0];
    NodePtr rhs2 = parents[1];
    if (op == "*") {
      rhs1->der += rhs2->val * der;
      rhs2->der += rhs1->val * der;
    } else if (op == "+") {
      rhs1->der += der;
      rhs2->der += der;
    } else if (op == "-") {
      rhs1->der += der;
      rhs2->der += -der;
    } else if (op == "/") {
      rhs1->der += der / rhs2->val;
      rhs2->der += -rhs1->val * der / (rhs2->val * rhs2->val);
    } else {
      std::cout << "Unsupported bop " << op << std::endl;
      assert(false);
    }

  } else {
    NodePtr p = parents[0];
    if (op == "~") {
      p->der += -der;
    } else if (op == "sin") {
      p->der += _cos_(p->val) * der;
    } else if (op == "cos") {
      p->der += _sin_(p->val) * (-der);
    } else if (op == "exp") {
      p->der += _exp_(p->val) * der;
    } else {
      std::cout << "Unsupported op " << op << std::endl;
      assert(false);
    }
  }
}

Graph::Graph(std::string file_name) {
  graph_file.open(file_name);
  if (!graph_file.is_open()) {
    std::exit(-1);
  }

  root = parse_node();
  graph_file.close();
}

NodePtr Graph::parse_node() {
  std::string raw_repr;
  if (!std::getline(graph_file, raw_repr)) {
    std::exit(-1);
  }

  NodePtr n = std::make_shared<Node>(raw_repr);

  if (nodes.count(n->uuid) > 0) {
    return nodes[n->uuid];
  }

  if (n->bop) {
    n->parents.push_back(parse_node());
    n->parents.push_back(parse_node());

    if (n->parents[0]->uuid == n->parents[1]->uuid) {
      n->parents[0]->children.push_back(n);
    } else {
      n->parents[0]->children.push_back(n);
      n->parents[1]->children.push_back(n);
    }
  } else if (n->op != NOP) {
    n->parents.push_back(parse_node());
    n->parents[0]->children.push_back(n);
  }

  nodes[n->uuid] = n;
  return n;
}

void Graph::order_graph(NodePtr start_node) {
  if (topo_visited.count(start_node->uuid) > 0) {
    return;
  }

  topo_visited[start_node->uuid] = start_node;
  for (auto &p : start_node->parents) {
    order_graph(p);
  }

  start_node->der = 0.0;
  topo.push_back(start_node);
}

void Graph::eval_adjoints() {
  for (auto node = topo.rbegin(); node != topo.rend(); node++) {
    (*node)->differentiate();
  }
}

void Graph::swap_node_for(NodePtr node_to_insert) {
  std::string uuid = node_to_insert->uuid;
  if(nodes.count(uuid) == 0)
    return;

  NodePtr node_to_remove = nodes[uuid];
  for(auto &c: node_to_remove->children) {
    for(size_t pc = 0; pc < c->parents.size(); pc++){
      if(c->parents[pc]->uuid != node_to_remove->uuid)
        continue;
      c->parents[pc] = node_to_insert;
    }
  }
  nodes.erase(node_to_remove->uuid);
  nodes[node_to_insert->uuid] = node_to_insert;
}
