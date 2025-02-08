#include "mytime.h"
#include <iomanip>
#include <iostream>

namespace ts {
void log_time(TimeDiff dt) {
  std::cout << dt.count() << " ms" << std::endl;
  std::cout << DELIM << std::endl;
}

void CheckPoint::show() {
  TimeDiff dt = toc - tic;
  std::cout << std::fixed << dt.count() << std::endl;
}

void RuntimeStats::show() {
  std::cout << tag << std::endl;
  for (auto &cp : cps)
    cp.show();
}
} // namespace ts
