#include "cppunitlite/TestHarness.h"

#include "graph.h"
#include "handlers.h"
#include "params.h"
#include "pin.H"

#include <cmath>

#define Pi 4 * std::atan(1)

class DdeStateSetup : public TestSetup {
public:
  void setup() {
    dde_state.to_instrument = false;
    call_pair.to.clear();
    call_pair.from.clear();
  }

  void teardown() {
    dde_state.to_instrument = false;
    call_pair.to.clear();
    call_pair.from.clear();
  }
};

TESTWITHSETUP(test_call_to_intrnsic_disables_instrumentation, DdeStateSetup) {
  dde_state.to_instrument = true;
  reg::insert_node(REG_XMM0, std::make_shared<Node>(Pi / 2));

  analysis::call::track_call_to_intrinsic(SINUS, MAIN);
  CHECK(dde_state.to_instrument == false);
}

TESTWITHSETUP(test_call_to_intrnsic_as_a_transformation, DdeStateSetup) {
  dde_state.to_instrument = true;

  reg::insert_node(REG_XMM0, std::make_shared<Node>(Pi / 2));
  analysis::call::track_call_to_intrinsic(SINUS, MAIN);

  CHECK_DOUBLES_EQUAL(reg::expect_node(REG_XMM0)->value, std::sin(Pi / 2));
}

TESTWITHSETUP(test_return_from_intrnsic_enables_instrumentation,
              DdeStateSetup) {
  dde_state.to_instrument = true;

  reg::insert_node(REG_XMM0, std::make_shared<Node>(Pi / 2));
  analysis::call::track_call_to_intrinsic(SINUS, MAIN);

  analysis::call::track_ret_from_intrinsic(MAIN, SINUS);

  CHECK(dde_state.to_instrument == true);
}

TESTWITHSETUP(test_call_to_unregistered_func_keeps_instrumentation,
              DdeStateSetup) {
  dde_state.to_instrument = true;

  analysis::call::track_call_to_intrinsic(FOO, MAIN);

  CHECK(dde_state.to_instrument == true);
}
