#include <stdio.h>

#include "canfigger.h"

int
main(void)
{
  struct Canfigger *config = canfigger_parse_file("../example-01.conf", ',');
  while (config != NULL)
  {
    printf("Key: %s, Value: %s\n", config->key,
           config->value != NULL ? config->value : "NULL");

    // Process attributes if necessary
    while (canfigger_attr)
    {
      printf("Attribute: %s\n", canfigger_attr);
      canfigger_free_current_attr_str_advance(config->attributes);
    }

    // Move to the next node and automatically free the current node
    canfigger_free_current_key_node_advance(&config);
    putchar('\n');
  }

  return 0;
}
