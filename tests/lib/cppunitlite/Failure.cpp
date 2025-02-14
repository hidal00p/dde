#include "Failure.h"

#include <cstdio>

std::string Failure::serialize() const {
  char std_buf[50] = {0};
  std::sprintf(std_buf, "%ld", lineNumber);
  return fileName + "(" + std_buf + "):" + "Failure: \"" + condition + "\" ";
}
