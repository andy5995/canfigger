#include "test.h"

int
main(void)
{
  // Current version is 0.3.x — should pass.
  assert(CANFIGGER_CHECK_VERSION(0, 3));

  // Not yet at 0.4 — macro should return 0 (expected failure).
  assert(!CANFIGGER_CHECK_VERSION(0, 4));

  // The patch-aware form must order versions numerically at every level.
  assert(CANFIGGER_CHECK_VERSION_PATCH(0, 3, 0));
  assert(CANFIGGER_CHECK_VERSION_PATCH(0, 2, 9));
  assert(CANFIGGER_CHECK_VERSION_PATCH(CANFIGGER_VERSION_MAJOR,
                                       CANFIGGER_VERSION_MINOR,
                                       CANFIGGER_VERSION_PATCH));
  assert(!CANFIGGER_CHECK_VERSION_PATCH(CANFIGGER_VERSION_MAJOR,
                                        CANFIGGER_VERSION_MINOR,
                                        CANFIGGER_VERSION_PATCH + 1));
  assert(!CANFIGGER_CHECK_VERSION_PATCH(0, 4, 0));
  assert(!CANFIGGER_CHECK_VERSION_PATCH(1, 0, 0));

  return 0;
}
