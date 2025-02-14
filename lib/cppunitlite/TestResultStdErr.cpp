#include "TestResultStdErr.h"
#include "Failure.h"
#include <iostream>

void TestResultStdErr::addSuccess(const Success &success) {
  std::cerr << "[V] " << success.serialize() << std::endl;
}

void TestResultStdErr::addFailure(const Failure &failure) {
  TestResult::addFailure(failure);
  std::cerr << "[X] " << failure.serialize() << std::endl;
}

void TestResultStdErr::endTests() {
  TestResult::endTests();
  std::cerr << testCount << " tests run" << std::endl;
  if (failureCount > 0)
    std::cerr << "****** There were " << failureCount << " failures.";
  else
    std::cerr << "There were no test failures.";
  std::cerr << "(time: " << secondsElapsed << " s)" << std::endl;
}
