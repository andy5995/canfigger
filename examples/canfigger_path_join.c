/*
 * Compilable example for canfigger_path_join().
 */
#include <stdlib.h>
#include "canfigger.h"

static void
example(void)
{
//! [canfigger_path_join]
  char *path =
    canfigger_path_join("/home/user/.config/myapp", "settings.conf");
  if (path)
  {
    struct Canfigger *list = canfigger_parse_file(path, ',');
    free(path);
    canfigger_free_list(&list);
  }
//! [canfigger_path_join]
}

int
main(void)
{
  example();
  return 0;
}
