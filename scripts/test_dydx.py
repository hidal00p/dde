#!/home/hidaloop/.folder/random/pinenv/dde/scripts/venv/bin/python
from micrograd.engine import Value

x = Value(1.5)
y = Value(2.0)

z = (x - y) / y

z.backward()

print(x.grad, y.grad)
