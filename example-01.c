#include <stdio.h>

#include "canfigger.h"

int
main(void)
{
  st_canfigger_list *config = canfigger_parse_file("../example-01.conf", ',');
  while (config != NULL)
  {
    printf("Key: %s, Value: %s\n", config->key,
           config->value != NULL ? config->value : "NULL");

    // Process attributes if necessary
    canfigger_init_attrs(config->attributes);
    while (canfigger_attr)
    {
      printf("Attribute: %s\n", canfigger_attr);
      canfigger_get_next_attr(config->attributes);
    }

    // Move to the next node and automatically free the current node
    canfigger_get_next_key(&config);
    putchar('\n');
  }

  return 0;
}
