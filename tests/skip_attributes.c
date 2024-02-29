#include "test.h"

// When built with sanitize enabled, this test will fail if all structs and
// and members aren't freed before exiting.

int
main(void)
{
  // call the primary library function to read your config file
  struct Canfigger *list = canfigger_parse_file(SOURCE_DIR "/multiple_attributes.conf", ',');
  assert (list);

  int i = 0;
  while (list)
  {
    canfigger_free_current_key_node_advance(&list);
    i++;
  }

  assert(i == 3);

  return 0;
}
