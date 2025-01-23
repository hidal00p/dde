#!/home/hidaloop/.folder/random/pinenv/dde/scripts/venv/bin/python
from micrograd.engine import Value

x = Value(3.0)
y = 3.0 * x * x - 4.0 * x + 5

y.backward()

print(f"{x.grad=}")
