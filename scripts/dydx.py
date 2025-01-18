#!/home/hidaloop/.folder/random/pinenv/dde/scripts/venv/bin/python
import pathlib
from graph import Graph


def main():
    graph_path = pathlib.Path("./prog.gr")
    assert graph_path.exists()

    Graph(graph_path, verbose=True).parse_graph().eval_jacobian()


if __name__ == "__main__":
    main()
