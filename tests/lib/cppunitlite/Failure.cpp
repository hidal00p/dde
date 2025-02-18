#include "Failure.h"

#include <cstdio>

std::string Failure::serialize() const {
  char std_buf[50] = {0};
  std::sprintf(std_buf, "%ld", lineNumber);
  return testName + "\n\t" + fileName + "(" + std_buf + "):" + "Failure: \"" +
         condition + "\" ";
}
