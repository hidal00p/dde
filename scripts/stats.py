import sys
import pathlib as pl
import numpy as np
import pprint

from tabulate import tabulate


def extract_stats(data: np.ndarray):
    max_val, min_val, mean_val, sum_val = (
        np.max(data),
        np.min(data),
        np.mean(data),
        np.sum(data),
    )
    return [max_val, min_val, mean_val, sum_val]


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

    comparison_data = {}
    for key, val in data.items():
        name_parts = key.split(" ")
        name = name_parts[0] if len(name_parts) == 1 else " ".join(name_parts[:-1])

        if name not in comparison_data:
            comparison_data[name] = []

        stats = extract_stats(np.array(val)[1:])
        comparison_data[name].append([key] + stats)

    table = []
    for stats_pair in comparison_data.values():
        raw, dde = stats_pair[0], stats_pair[1]
        overhead = int(dde[2] / raw[2])
        table.append([raw[0]] + raw[1:] + ["-"])
        table.append([dde[0]] + dde[1:] + [overhead])

    print(
        tabulate(
            table,
            headers=[
                "Test",
                "Max [ms]",
                "Min [ms]",
                "Mean [ms]",
                "Total [ms]",
                "Overhead [dde / raw]",
            ],
        )
    )


if __name__ == "__main__":
    assert len(sys.argv) == 2, "Provide full path to the stats.log"

    stats_path = pl.Path(sys.argv[1])
    assert stats_path.exists(), f"{stats_path} was not find."

    analyze(stats_path)
