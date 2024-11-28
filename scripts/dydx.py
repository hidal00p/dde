import pathlib
from typing import NamedTuple
from dataclasses import dataclass


class NodeGene(NamedTuple):
    uuid: str
    value: int
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
        self.result = None

    def parse_graph(self) -> "Graph":
        with open(self.graph_path) as self.graph_file:
            self.result = self.parse_node(self.graph_file.readline())
        return self

    def parse_node(self, raw_gene: str) -> Node:
        gene_atoms = raw_gene.split()
        gene = NodeGene(
            gene_atoms[0],
            int(gene_atoms[2]),
            None if len(gene_atoms) == 3 else gene_atoms[3],
        )

        # If node was parsed before we just return it
        if node := self.node_map.get(gene.uuid):
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

    def eval_jacobian(self):
        assert self.result
        self.result.der = 1.0
        self.propagate_derivative(self.result)
        return self

    def propagate_derivative(self, lhs_node: Node):
        if not (op := lhs_node.gene.op):
            return

        rhs_op0 = lhs_node.parents[0]
        rhs_op1 = lhs_node.parents[1]

        if op == "*":
            rhs_op0.der += rhs_op1.gene.value * lhs_node.der
            rhs_op1.der += rhs_op0.gene.value * lhs_node.der
        elif op == "+":
            rhs_op0.der += lhs_node.der
            rhs_op1.der += lhs_node.der

        self.propagate_derivative(rhs_op0)

        if rhs_op0 == rhs_op1:
            return

        self.propagate_derivative(rhs_op1)


def main():
    graph_path = pathlib.Path("./prog.gr")
    assert graph_path.exists()

    graph = Graph(graph_path).parse_graph().eval_jacobian()
    for k, v in graph.node_map.items():
        print(f"{k} {v.der}")


if __name__ == "__main__":
    main()
