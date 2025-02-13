#include "cppunitlite/TestHarness.h"
#include "cppunitlite/TestResultStdErr.h"

TEST(Whaever, Blah) {
  CHECK(false);
}

int main() {
  TestResultStdErr result;
  TestRegistry::runAllTests(result);
  return (result.getFailureCount());
}
