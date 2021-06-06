#include "test.h"

int main (void)
{
  int req_len = strlen ("no_exist_test_canfigger.conf") + strlen (SOURCE_DIR) + strlen ("tests") + 1 + 1 + 1;
  char test_config_file[req_len];
  sprintf (test_config_file, "%s/tests/no_exist_test_canfigger.conf", SOURCE_DIR);
  st_canfigger_list *list =
    canfigger_parse_file (test_config_file, ',');

  assert (list == NULL);
  assert (errno);
  perror (__func__);

  return 0;
}
