#!/home/hidaloop/.folder/random/pinenv/dde/scripts/venv/bin/python
from micrograd.engine import Value


def g(x):
    y = 3.0 * x
    return y * y - 4.0 + y


def f(x):
    return g(x) * x + 5


x = Value(2.0)
y = f(x)

y.backward()

print(f"{x.grad=}")
