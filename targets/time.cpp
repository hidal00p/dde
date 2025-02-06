#include "time.h"
#include <iostream>

namespace ts {
void log_time(TimeDiff dt) {
  std::cout << dt.count() << " ms" << std::endl;
  std::cout << DELIM << std::endl;
}

void CheckPoint::show() {
  TimeDiff dt = toc - tic;

  std::cout << tag + " ";
  std::cout << dt.count() << " ms" << std::endl;
  std::cout << DELIM << std::endl;
}
} // namespace ts
