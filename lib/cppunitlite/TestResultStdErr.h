#ifndef TESTRESULTSTDERR_H
#define TESTRESULTSTDERR_H

#include "Success.h"
#include "TestResult.h"

class TestResultStdErr : public TestResult {
public:
  void addFailure(const Failure &failure);
  void addSuccess(const Success &suc);
  void endTests();
};

#endif
