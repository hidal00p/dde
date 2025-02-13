#include "cppunitlite/TestHarness.h"
#include "cppunitlite/TestResultStdErr.h"


TEST(test_addition, GraphTest) {
  double a = 3.0;
  double b = 2.0;

  CHECK_DOUBLES_EQUAL(a + b, 5.0f);
}

int main() {
    TestResultStdErr result;
    TestRegistry::runAllTests(result);
    return (result.getFailureCount());
}
