#!/home/hidaloop/.folder/random/pinenv/dde/scripts/venv/bin/python
from micrograd.engine import Value

x = Value(3.0)
y = 3 * x**2 - 4.0

y.backward()

print(x.grad)
