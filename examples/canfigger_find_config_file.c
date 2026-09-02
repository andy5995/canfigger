/*
 * Compilable example for canfigger_find_config_file().
 */
#include <stdlib.h>
#include "canfigger.h"

static void
example(void)
{
//! [canfigger_find_config_file]
  /* The user's own myapp/myapp.conf wins; failing that, the first one found
     under $XDG_CONFIG_DIRS. NULL means neither exists. */
  char *path = canfigger_find_config_file("myapp", "myapp.conf");
  if (path)
  {
    struct Canfigger *list = canfigger_parse_file(path, ',');
    free(path);
    canfigger_free_list(&list);
  }
//! [canfigger_find_config_file]
}

int
main(void)
{
  example();
  return 0;
}
