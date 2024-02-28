#include <stdio.h>

#include "test.h" // Used for canfigger test, not normally needed
#include "canfigger.h"

int
main(int argc, char *argv[])
{
  char *default_filename = SOURCE_DIR "/examplerc";
  char *filename_ptr = default_filename;

  if (argc == 2)
    filename_ptr = argv[1];

  if (argc > 2)
  {
    fputs("This example program only accepts a single argument:\n\n", stderr);
    fprintf(stderr, "%s <config-file>\n\n", argv[0]);
    return -1;
  }

  //
  // Get a linked list containing the parsed config file. Each node contains
  // a key (or a "setting", or an "option"), a value and attributes (if they
  // are provided in your program's configuration file.
  //
  // The second argument is based on what the config file uses to separate
  // the attributes.
  //
  struct Canfigger *config = canfigger_parse_file(filename_ptr, ',');

  if (!config)
    return -1;

  // i is only used for testing
  int i = 0;

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
    char *attr = NULL;
    //
    // Pass '&addr' to this function and it will get assigned an
    // attribute, or NULL if there are none.
    canfigger_free_current_attr_str_advance(config->attributes, &attr);
    while (attr)
    {
      printf("Attribute: %s\n", attr);

      //
      // Get the next attribute in the list (if there is one).
      //
      canfigger_free_current_attr_str_advance(config->attributes, &attr);
    }

    // Move to the next node and automatically free the current node
    canfigger_free_current_key_node_advance(&config);
    putchar('\n');

    i++;
  }

  // This should be the number of keys in the example config
  assert(i == 6);

  return 0;
}
