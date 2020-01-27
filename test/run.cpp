#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

extern "C" {
#include "run.h"
};

bool run_tests() {
  const char *argv[] = { "arith" };
  return (Catch::Session().run(1, argv) == 0);
}
