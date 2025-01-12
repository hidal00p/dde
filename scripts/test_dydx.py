#!/home/hidaloop/.folder/random/pinenv/dde/scripts/venv/bin/python
from micrograd.engine import Value

x = Value(2.63)
y = 2 * x * x * x + 5 * x * x - 4 * x - 3

y.backward()

print(x.grad)
