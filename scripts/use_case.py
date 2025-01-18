import os
import time
import pathlib
from graph import Graph

DDE_DIR = "/home/hidaloop/.folder/random/pinenv/dde"
PIN_TOOL = f"{DDE_DIR}/src/obj-intel64/dde.so"
TARGET_FILE = f"{DDE_DIR}/targets/polynom.exe"
GRAPH_FILE = f"{DDE_DIR}/scripts/prog.gr"


def f(x: float) -> tuple[float, float]:
    cmd = f"pin -t {PIN_TOOL} -- {TARGET_FILE} {x} > {GRAPH_FILE}"
    os.system(cmd)

    graph = Graph(pathlib.Path(GRAPH_FILE)).parse_graph()
    f_x = graph.results[0].gene.value

    graph.eval_jacobian()
    df_dx = graph.ordered_graph[0].der

    return f_x, df_dx


# x_n+1 = x_n - f(x_n) / f'(x_n)
def newton_method(x: float):
    tic = time.time()
    itter = 0
    f_x = None
    while itter < 100:
        f_x, df_dx = f(x)
        x -= f_x / df_dx

        if abs(f_x) < 1e-8:
            break
        itter += 1

    assert f_x is not None
    toc = time.time()
    print(f"{x=} {f_x=} with {itter=} in {toc - tic} s")


if __name__ == "__main__":
    x_guess = [-10.0, 0.0, 10.0]
    for x in x_guess:
        newton_method(x)
