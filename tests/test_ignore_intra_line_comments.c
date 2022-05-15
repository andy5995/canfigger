#include "test.h"

int
main(void)
{
  const struct expected
  {
    const char *key;
    const char *value;
    const char *attribute;
  } data[] = {
    {"foo", "bar", ""},
    {"leadingSpace", "nullified", ""},
  };

  char test_config_file[PATH_MAX];
  assert((size_t)
         snprintf(test_config_file, sizeof test_config_file,
                  "%s/test_ignore_intra_line_comments.conf",
                  SOURCE_DIR) < sizeof test_config_file);

  // call the primary library function to read your config file
  st_canfigger_list *list = canfigger_parse_file(test_config_file, ',');

  // create a pointer to the head of the list before examining the list.
  st_canfigger_list *root = list;
  if (list == NULL)
  {
    fprintf(stderr, "Error");
    return -1;
  }

  int i = 0;
  while (list != NULL)
  {
    printf("\n\
Key: %s\n\
Value: %s\n\
Attribute: %s\n", list->key, list->value, list->attr_node->str);

    assert(strcmp(data[i].key, list->key) == 0);
    assert(strcmp(data[i].value, list->value) == 0);
    fprintf(stderr, "attr: %s\n", list->attr_node->str);
    assert(strcmp(data[i].attribute, list->attr_node->str) == 0);
    i++;

    // Free the attribute node (the root isn't needed when you know
    // there is only one node).
    canfigger_free_attr(list->attr_node);

    list = list->next;
  }

  assert(i == sizeof data / sizeof data[0]);

  // free the list
  canfigger_free(root);

  return 0;
}
