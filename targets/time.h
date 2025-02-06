#ifndef TIME_H
#define TIME_H

#include <chrono>
#include <string>
#include <vector>
#define DELIM "========"

namespace ts {
using clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::system_clock::time_point;
typedef std::chrono::duration<double, std::milli> TimeDiff;

void log_time(TimeDiff dt);

struct CheckPoint {
  std::string tag;
  TimePoint tic;
  TimePoint toc;

  CheckPoint(std::string tag) : tag(tag){};

  void show();
};

struct RuntimeStats {
  std::string tag;
  std::vector<CheckPoint> cps;

  void show();
};
} // namespace ts

#endif
