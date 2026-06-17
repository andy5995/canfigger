/*
 * Compilable example for canfigger_config_file().
 */
#include <stdlib.h>
#include "canfigger.h"

static void
example(void)
{
//! [canfigger_config_file]
  char *path = canfigger_config_file("apprc");
  if (path)
  {
    struct Canfigger *list = canfigger_parse_file(path, ',');
    free(path);
    canfigger_free_list(&list);
  }
//! [canfigger_config_file]
}

int
main(void)
{
  example();
  return 0;
}
