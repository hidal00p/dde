#include "graph.h"

std::map<uint32_t, node *> known_node_buffer;

void register_operand(uint32_t op_id) {
  bool node_already_buffered = known_node_buffer.count(op_id) > 0;

  if (node_already_buffered) {
    known_node_buffer[op_id]->top = false;
  } else {
    known_node_buffer[op_id] = new node(op_id);
  }
}

void register_new_transformation(uint32_t op1_id, uint32_t op2_id,
                                 uint32_t res_id, TransfType transform) {
  register_operand(op1_id);
  register_operand(op2_id);
  register_operand(res_id);

  node *result_node = known_node_buffer[res_id];
  result_node->top = true;
  result_node->transf.type = transform;
  result_node->transf.args =
      new node *[2] { known_node_buffer[op1_id], known_node_buffer[op2_id] };
  result_node->transf.argc = 2;
}
