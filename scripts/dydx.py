#!/home/hidaloop/.folder/random/pinenv/dde/scripts/venv/bin/python
import pathlib
from typing import NamedTuple
from dataclasses import dataclass


class NodeGene(NamedTuple):
    uuid: str
    value: float
    is_active: bool
    op: str | None


@dataclass
class Node:
    gene: NodeGene
    parents: list["Node"]
    der: float = 0.0


class Graph:

    def __init__(self, graph_path: pathlib.Path):
        self.graph_path = graph_path
        self.node_map = {}
        self.results = []
        self.visited_nodes = set()
        self.ordered_graph = []

    def parse_graph(self) -> "Graph":
        with open(self.graph_path) as self.graph_file:

            while res_raw_gene := self.graph_file.readline():
                self.results.append(self.parse_node(res_raw_gene))

        return self

    def parse_node(self, raw_gene: str) -> Node:
        gene_atoms = raw_gene.split()
        gene = NodeGene(
            gene_atoms[0],
            float(gene_atoms[1]),
            gene_atoms[2] == "1",
            None if len(gene_atoms) == 3 else gene_atoms[3],
        )

        # If node was parsed before we just return it
        if node := self.node_map.get(gene.uuid):
            self.consume_node(node)
            return node

        parents = []
        if gene.op:
            parents = [
                self.parse_node(self.graph_file.readline()),
                self.parse_node(self.graph_file.readline()),
            ]

        node = Node(gene, parents)
        self.node_map[node.gene.uuid] = node

        return node

    def consume_node(self, node: Node):
        for p_node in node.parents:
            self.graph_file.readline()
            self.consume_node(p_node)

    def eval_jacobian(self):
        assert self.results

        for result in self.results:
            if result.der > 0.0:
                continue

            result.der = 1.0
            self.clear_derivtives(result)
            self.visited_nodes.clear()
            self.ordered_graph.clear()

            self.order_graph(result)
            for node in reversed(self.ordered_graph):
                self.propagate_derivative(node)

            self.show_derivatives(result)

    def show_derivatives(self, node: Node, prefix=""):
        print(f"{prefix} {node.gene.uuid} {node.der}")
        for p_node in node.parents:
            self.show_derivatives(p_node, prefix + " ")

    def clear_derivtives(self, node: Node):
        for p_node in node.parents:
            p_node.der = 0.0
            self.clear_derivtives(p_node)

    def order_graph(self, node: Node):
        if node.gene.uuid in self.visited_nodes:
            return

        self.visited_nodes.add(node.gene.uuid)
        for p_node in node.parents:
            self.order_graph(p_node)

        self.ordered_graph.append(node)

    def propagate_derivative(self, lhs_node: Node):
        if not (op := lhs_node.gene.op):
            return

        rhs_op0 = lhs_node.parents[0]
        rhs_op1 = lhs_node.parents[1]

        if op == "*":
            rhs_op0.der += (
                rhs_op1.gene.value * lhs_node.der if rhs_op0.gene.is_active else 0.0
            )
            rhs_op1.der += (
                rhs_op0.gene.value * lhs_node.der if rhs_op1.gene.is_active else 0.0
            )
        elif op == "+":
            rhs_op0.der += lhs_node.der if rhs_op0.gene.is_active else 0.0
            rhs_op1.der += lhs_node.der if rhs_op1.gene.is_active else 0.0


def main():
    graph_path = pathlib.Path("./prog.gr")
    assert graph_path.exists()

    Graph(graph_path).parse_graph().eval_jacobian()


if __name__ == "__main__":
    main()
