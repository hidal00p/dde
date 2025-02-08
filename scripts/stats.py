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


def is_float(line: str) -> bool:
    try:
        float(line)
        return True
    except:
        return False


def analyze(stats_path: pl.Path):
    with open(stats_path) as s_file:
        data = {}
        func = None
        while line := s_file.readline():
            if not is_float(line):
                func = line.strip()
                data[func] = []
                continue
            data[func].append(float(line))

        for key, val in data.items():
            val = np.array(val)
            print(key)
            print_stats(val)


if __name__ == "__main__":
    stats_path = pl.Path("/home/hidaloop/.folder/random/pinenv/dde/stats.log")
    assert stats_path.exists(), f"{stats_path} was not find."

    analyze(stats_path)
