#include <stdio.h>

#include "canfigger.h"

int
main(void)
{
  //
  // Get a linked list containing the parsed config file. Each node contains
  // a key (or a "setting", or an "option"), a value and attributes (if they
  // are provided in your program's configuration file.
  //
  struct Canfigger *config = canfigger_parse_file("../example-01.conf", ',');
  while (config != NULL)
  {
    //
    // The value member of the node must be checked for NULL
    // before using it.
    //
    printf("Key: %s, Value: %s\n", config->key,
           config->value != NULL ? config->value : "NULL");

    //
    // Process attributes if necessary
    //
    // You must check canfigger_attr for NULL before doing an operation on it,
    // such as strcmp or printf. If it is not NULL, it points to the the first
    // attribute (or the *only* attribute if only one was used
    //
    while (canfigger_attr)
    {
      printf("Attribute: %s\n", canfigger_attr);

      //
      // Get the next attribute in the list (if there is one).
      //
      canfigger_free_current_attr_str_advance(config->attributes);
    }

    // Move to the next node and automatically free the current node
    canfigger_free_current_key_node_advance(&config);
    putchar('\n');
  }

  return 0;
}
