#include "cppunitlite/TestHarness.h"
#include "cppunitlite/TestResultStdErr.h"

int main() {
  TestResultStdErr result;
  TestRegistry::runAllTests(result);
  return (result.getFailureCount());
}
