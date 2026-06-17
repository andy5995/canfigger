/*
 * Compilable example for canfigger_config_dir().
 */
#include <stdio.h>
#include <stdlib.h>
#include "canfigger.h"

static void
example(void)
{
//! [canfigger_config_dir]
  char *dir = canfigger_config_dir("myapp");
  if (dir)
  {
    char *path = canfigger_path_join(dir, "settings.conf");
    free(dir);
    if (path)
    {
      struct Canfigger *list = canfigger_parse_file(path, ',');
      free(path);
      canfigger_free_list(&list);
    }
  }
//! [canfigger_config_dir]
}

int
main(void)
{
  example();
  return 0;
}
