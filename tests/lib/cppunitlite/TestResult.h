#ifndef TESTRESULT_H
#define TESTRESULT_H

#include "Success.h"

#include <time.h>

class Failure;

class TestResult {
public:
  TestResult();
  virtual ~TestResult(){};

  virtual void testWasRun();
  virtual void startTests();
  virtual void endTests();
  virtual void addFailure(const Failure &failure);
  virtual void addSuccess(const Success &suc);

  int getFailureCount() const { return failureCount; }

protected:
  int failureCount;
  int testCount;
  time_t startTime;
  int secondsElapsed;
};

#endif
