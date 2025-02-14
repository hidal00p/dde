#include "Test.h"
#include "Failure.h"
#include "TestRegistry.h"
#include "TestResult.h"

Test::Test(const std::string &testName) : name(testName) {
  TestRegistry::addTest(this);
  passed = true;
}

void Test::run(TestResult &result) {
#ifndef DONT_CATCH_EXCEPTIONS
  try {
#endif
    setup();
    runTest(result);
    if (passed)
      result.addSuccess(Success(name));
#ifndef DONT_CATCH_EXCEPTIONS
  } catch (...) {
    result.addFailure(Failure("Unhandled exception", name, "", 0));
  }
#endif
  teardown();
  result.testWasRun();
}
