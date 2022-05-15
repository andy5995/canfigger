#include "test.h"

int
main(void)
{
  struct expected
  {
    const char *key;
    const char *value;
    const char *attribute;
  };
  const struct expected data[] = {
    {"🐢👀🍻", "some chars", "unicode"},
    {"👀🍻💯", "", ""},
    {"🍻💯💣", "looks like a 💣",
     "Some cartoon characters will be very unhappy."},
  };

  char test_config_file[PATH_MAX];
  assert((size_t)
         snprintf(test_config_file, sizeof test_config_file,
                  "%s/test_unicode.conf",
                  SOURCE_DIR) < sizeof test_config_file);

  // call the primary library function to read your config file
  st_canfigger_list *list = canfigger_parse_file(test_config_file, ';');

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

    // free the attribute node
    canfigger_free_attr(list->attr_node);

    list = list->next;
  }

  // free the list
  canfigger_free(root);

  return 0;
}
