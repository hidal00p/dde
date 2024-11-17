#include "graph.h"
#include <iostream>

std::map<uint32_t, node*> known_node_buffer;

std::string transf_type_to_string(TransfType tt) {
  switch(tt) {
    case IMUL:
      return "*";
    case ADD:
      return "+";
    case SUB:
      return "-";
    default:
      break;
  };
  return "";
}

void traverse_node(const node* node, std::string prefix) {
  std::cout << prefix << "0x" << std::hex << node->identifier;

  if(node->transf.args == nullptr) {
    std::cout << std::endl;
    return;
  }

  std::cout << " " << transf_type_to_string(node->transf.type) << std::endl;

  for(uint8_t i = 0; i < node->transf.argc; i++) {
    traverse_node(node->transf.args + i, prefix + " ");
  }
}

void show_graph() {
  for (const auto & [key, value] : known_node_buffer) {
    if (!value->top)
      continue;

    std::string prefix = "";
    traverse_node(value, prefix);
  }

  known_node_buffer.clear();
}

void test_graph_construction() {
  node b = {
    .top = false,
    .identifier = 0,
    .transf = {
      .type = TransfType::NONE,
      .args = nullptr,
      .argc = 0
    }
  };

  node c = {
    .top = false,
    .identifier = 1,
    .transf = {
      .type = TransfType::NONE,
      .args = nullptr,
      .argc = 0
    }
  };

  node* a_args = new node[2];
  a_args[0] = b;
  a_args[1] = c;

  node a = {
    .top = false,
    .identifier = 2,
    .transf = {
      .type = TransfType::IMUL,
      .args = a_args,
      .argc = 2
    }
  }; 

  node* d_args = new node[2];
  d_args[0] = a;
  d_args[1] = c;

  node d = {
    .top = true,
    .identifier = 3,
    .transf = {
      .type = TransfType::IMUL,
      .args = d_args,
      .argc = 2
    }
  }; 

  node* e_args = new node[2];
  e_args[0] = a;
  e_args[1] = b;

  node e = {
    .top = true,
    .identifier = 4,
    .transf = {
      .type = TransfType::IMUL,
      .args = e_args,
      .argc = 2
    }
  }; 

  known_node_buffer[e.identifier] = &e;
  known_node_buffer[d.identifier] = &d;
  known_node_buffer[a.identifier] = &a;
  known_node_buffer[b.identifier] = &b;
  known_node_buffer[c.identifier] = &c;

  while(!known_node_buffer.empty()) {
    show_graph();
  }
}
