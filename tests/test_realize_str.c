#include "test.h"

int
main (void)
{
  char str[PATH_MAX];
  strcpy (str, "$HOME/.local/share");
  canfigger_realize_str (str, "/home/foo");
  assert (strcmp (str, "/home/foo/.local/share") == 0);

  strcpy (str, "~/.config");
  canfigger_realize_str (str, "/home/foo");
  assert (strcmp (str, "/home/foo/.config") == 0);

  return 0;
}
