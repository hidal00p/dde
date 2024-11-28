#include <map>
#include <optional>
#include <string>

/*
 * Using the following two maps we attempt to resolve
 * every memory initialization with a value.
 *
 * We also keep track of final memory values.
 *
 * NOTE: might want to wrap things into a shared_ptr
 */
enum transformation { NONE, MUL, ADD };

std::string get_uuid();

struct node;

struct node {
  std::string uuid;
  int value;
  node **operands;
  uint8_t n_operands;
  transformation tr;

  node(int val) {
    uuid = get_uuid();
    value = val;
    operands = nullptr;
    n_operands = 0;
    tr = transformation::NONE;
  }

  node(int val, uint8_t n_oprs, node **oprs, transformation trn) {
    uuid = get_uuid();
    value = val;
    operands = oprs;
    n_operands = n_oprs;
    tr = trn;
  }
};

extern std::map<uint64_t, node *> mem_map;
extern std::map<uint8_t, node *> reg_map;

void show_node(node *n);
void show_mem_map();

namespace mem {
void insert_node(uint64_t ef_addr, node *n);
std::optional<node *> get_node(uint64_t ef_addr);
node *expect_node(uint64_t ef_addr);
void write_to_reg(uint64_t from_mem, uint64_t to_reg);
} // namespace mem

namespace reg {
void insert_node(uint64_t reg, node *n);
std::optional<node *> get_node(uint64_t reg);
node *expect_node(uint64_t reg);
void write_to_mem(uint64_t from_reg, uint64_t to_mem);
void write_to_other_reg(uint64_t from_reg, uint64_t to_reg);
} // namespace reg
