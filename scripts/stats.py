import sys
import pathlib as pl
import numpy as np

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


def analyze_one(stats_path: pl.Path):
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

        stats = extract_stats(np.array(val)[0:])
        comparison_data[name].append([key] + stats)

    table = []
    for stats_pair in comparison_data.values():
        raw, dde = stats_pair[0], stats_pair[1]
        overhead = round(dde[3] / raw[3], 4)
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


def analyze_two(raw_path: pl.Path, dde_path: pl.Path):
    data = {}
    with open(raw_path) as s_file:
        func = None
        while line := s_file.readline():
            if not is_float(line):
                func = line.strip()
                data[func] = []
                continue
            data[func].append(float(line))

    with open(dde_path) as s_file:
        func = None
        while line := s_file.readline():
            if not is_float(line):
                func = line.strip() + " dde"
                data[func] = []
                continue
            data[func].append(float(line))

    comparison_data = {}
    for key, val in data.items():
        name_parts = key.split(" ")
        name = name_parts[0] if len(name_parts) == 1 else " ".join(name_parts[:-1])

        if name not in comparison_data:
            comparison_data[name] = []

        stats = extract_stats(np.array(val)[:])
        comparison_data[name].append([key] + stats)

    table = []
    for stats_pair in comparison_data.values():
        raw, dde = stats_pair[0], stats_pair[1]
        overhead = round(dde[3] / raw[3], 4)
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
    assert len(sys.argv) == 3 or len(sys.argv) == 2

    if len(sys.argv) == 3:
        raw_path = pl.Path(sys.argv[1])
        dde_path = pl.Path(sys.argv[2])
        assert raw_path.exists(), f"{raw_path} was not find."
        assert dde_path.exists(), f"{dde_path} was not find."

        analyze_two(raw_path, dde_path)
    else:
        raw_path = pl.Path(sys.argv[1])
        assert raw_path.exists(), f"{raw_path} was not find."

        analyze_one(raw_path)
