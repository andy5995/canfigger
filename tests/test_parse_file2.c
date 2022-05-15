#include "test.h"

int
main(void)
{
  char test_config_file[PATH_MAX];
  assert((size_t)
         snprintf(test_config_file, sizeof test_config_file,
                  "%s/no_exist_test_canfigger.conf",
                  SOURCE_DIR) < sizeof test_config_file);
  st_canfigger_list *list = canfigger_parse_file(test_config_file, ',');

  assert(list == NULL);
  assert(errno);
  perror(__func__);

  return 0;
}
