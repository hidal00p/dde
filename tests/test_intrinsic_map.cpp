#include <iostream>
#include <map>
#include <string>
#include <cmath>
#include <vector>
#include <cassert>

typedef double (* intrinsic)(double);

const std::map<std::string, intrinsic> intr_dict = {
  {"cos", std::cos},
  {"sin", std::sin}
};

int main() {
  std::vector<double> xs = {1.0, 1.1, 1.15, 1.2};
  for(auto& x: xs) {
    double c = intr_dict.at("cos")(x);
    double s = intr_dict.at("sin")(x);
    assert(std::fabs(c*c + s*s - 1) < 1e-8);
  }
}
