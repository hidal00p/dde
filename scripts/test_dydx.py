#!/home/hidaloop/.folder/random/pinenv/dde/scripts/venv/bin/python
from micrograd.engine import Value

x = Value(1.5)
y = Value(1.25)

c = x * y

for _ in range(4):
    c *= y

c.backward()

print(x.grad, y.grad)
