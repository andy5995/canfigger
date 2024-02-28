#include "test.h"

int
main(void)
{
  struct Canfigger *list = canfigger_parse_file("foo", ',');
  assert(list == NULL);
  assert(errno);
  perror(__func__);

  return 0;
}
