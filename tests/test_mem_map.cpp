#include <random>
#include <string>
#include <iostream>

std::string get_uuid() {
  static const uint id_length = 9;
  static const std::string alphabet = "aAbBcCdDeEfFgGhHiIjJ0123456789-+=";

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distr(0, alphabet.size() - 1);

  std::string uuid = "";
  for (uint i = 0; i < id_length; i++) {
    uuid += alphabet[distr(gen)];
  }

  return uuid;
}

int main() {
  std::cout << get_uuid() << std::endl;
  std::cout << get_uuid() << std::endl;
  std::cout << get_uuid() << std::endl;
  std::cout << get_uuid() << std::endl;
  std::cout << get_uuid() << std::endl;
}
