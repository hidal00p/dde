import numpy as np
import pathlib as pl

from tabulate import tabulate


def extract_stats(data: np.ndarray):
    max_val, min_val, mean_val, sum_val = (
        np.max(data),
        np.min(data),
        np.mean(data),
        np.sum(data),
    )
    return [max_val, min_val, mean_val, sum_val, np.round(max_val / sum_val * 100, 2)]


def is_float(line: str) -> bool:
    try:
        float(line)
        return True
    except:
        return False


def analyze(stats_path: pl.Path):
    data = {}
    with open(stats_path) as s_file:
        func = None
        while line := s_file.readline():
            if not is_float(line):
                func = line.strip()
                data[func] = []
                continue
            data[func].append(float(line))

    table = []
    for key, val in data.items():
        stats = extract_stats(np.array(val))
        table.append([key] + stats)

    print(
        tabulate(
            table,
            headers=[
                "Test",
                "Max [ms]",
                "Min [ms]",
                "Mean [ms]",
                "Total [ms]",
                "Latency of first run [%]",
            ],
        )
    )


if __name__ == "__main__":
    stats_path = pl.Path("/home/hidaloop/.folder/random/pinenv/dde/stats.log")
    assert stats_path.exists(), f"{stats_path} was not find."

    analyze(stats_path)
