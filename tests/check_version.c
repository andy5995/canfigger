#include "test.h"

int
main(void)
{
  // Current version is 0.3.x — should pass.
  assert(CANFIGGER_CHECK_VERSION(0, 3));

  // Not yet at 0.4 — macro should return 0 (expected failure).
  assert(!CANFIGGER_CHECK_VERSION(0, 4));

  return 0;
}
