#include <iostream>

void foo() {
  std::cout << "foo" << std::endl;
  return;
}

void baz() {
  std::cout << "baz" << std::endl;
  return;
}

void bar() {
  std::cout << "bar" << std::endl;
  return;
}

int main() {
  foo();
  baz();
  bar();
}
