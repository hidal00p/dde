from micrograd.engine import Value

x = Value(1.5)
y = Value(1.25)
z = x * y * y

w = z * x + 0.95
v = z * y

c = v * w
c.backward()

print(x.grad, y.grad)
