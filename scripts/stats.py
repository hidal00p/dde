import numpy as np
import pathlib as pl


def print_stats(data: np.ndarray):
    print(
        "Max: {:.6} ms\n"
        "Min: {:.6} ms\n"
        "Mean: {:.6} ms\n"
        "Total: {:.6} ms\n"
        "Worst portion: {}%".format(
            np.max(data),
            np.min(data),
            np.mean(data),
            np.sum(data),
            np.round(np.max(data) / np.sum(data) * 100, 2),
        )
    )


def analyze(stats_path: pl.Path):
    with open(stats_path) as s_file:
        while s_file.readline() != "Stats:\n":
            pass

        s_file.readline()

        inst_data, graph_data = [], []
        while line := s_file.readline():
            inst, graph = map(lambda x: float(x), line.strip().split(" "))
            inst_data.append(inst)
            graph_data.append(graph)

        inst_data = np.array(inst_data)
        graph_data = np.array(graph_data)

        print("Instrumentation: ")
        print_stats(inst_data)
        print()

        print(f"Graph parsing:")
        print_stats(graph_data)


if __name__ == "__main__":
    stats_path = pl.Path("/home/hidaloop/.folder/random/pinenv/dde/stats.log")
    assert stats_path.exists(), f"{stats_path} was not find."

    analyze(stats_path)
