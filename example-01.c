#include <stdio.h>

#include "canfigger.h"

int
main(void)
{
  st_canfigger_list *config = canfigger_parse_file("../example-01.conf", ',');
  while (config != NULL)
  {
    printf("Key: %s, Value: %s\n", config->key, config->value);

    // Process attributes if necessary
    st_canfigger_attr_node *attr = config->attr_node;
    while (attr)
    {
      printf("Attribute: %s\n", attr->str);
      canfigger_get_next_attr(&attr);
    }

    // Move to the next node and automatically free the current node
    canfigger_get_next_key(&config);
  }

  return 0;
}
