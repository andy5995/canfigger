#include <stdio.h>

#include "tests/test.h"

int
main(int argc, char *argv[])
{
  char *default_filename = SOURCE_DIR "/example-01.conf";
  char *filename_ptr = default_filename;

  if (argc == 2)
    filename_ptr = argv[1];

  if (argc > 2)
  {
    fputs("This example program only accepts a single argument:\n\n", stderr);
    fprintf(stderr, "%s <config-file>\n\n", argv[0]);
    return -1;
  }

  // Parse the config file. The second argument is the delimiter used to
  // separate the value from any attributes on the same line.
  struct Canfigger *list = canfigger_parse_file(filename_ptr, ',');
  if (!list)
    return -1;

  int count = 0;

  while (list)
  {
    // value may be NULL for keys with no '=' on the line.
    printf("Key: %s, Value: %s\n", list->key,
           list->value ? list->value : "NULL");

    // Iterate attributes if present. Initialize attr to NULL before the
    // first call; each call frees the previous string and loads the next.
    // Skip this block entirely if the node has no attributes.
    char *attr = NULL;
    canfigger_free_current_attr_str_advance(list->attributes, &attr);
    while (attr)
    {
      printf("Attribute: %s\n", attr);
      canfigger_free_current_attr_str_advance(list->attributes, &attr);
    }

    // Free the current node and advance to the next.
    canfigger_free_current_key_node_advance(&list);
    putchar('\n');

    count++;
  }

  // Verify all entries in the example config were visited.
  assert(count == 6);

  return 0;
}
