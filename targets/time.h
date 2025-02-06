#ifndef TIME_H
#define TIME_H

#include <chrono>
#include <string>
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
}

#endif
