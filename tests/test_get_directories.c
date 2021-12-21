#include "test.h"

int
main (void)
{
  const st_canfigger_directory *st_directory = canfigger_get_directories ();
  assert (st_directory != NULL);
  assert (st_directory->home != NULL);

  FILE *fp = fopen ("/home/andy/src", "r");
  if (fp != NULL)
  {
    assert (fclose (fp) == 0);
    assert (strcmp (st_directory->home, "/home/andy") == 0);
    assert (strcmp (st_directory->configroot, "/home/andy/.config") == 0);
    assert (strcmp (st_directory->dataroot, "/home/andy/.local/share") == 0);
  }

  return 0;
}