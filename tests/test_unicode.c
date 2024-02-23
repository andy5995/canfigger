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
    {"👀🍻💯", NULL, NULL},
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
  if (list == NULL)
  {
    fprintf(stderr, "Error");
    return -1;
  }

  int i = 0;
  while (list)
  {
    printf("\n\
Key: %s\n\
Value: %s\n\
Attribute: %s\n", list->key, list->value != NULL ? list->value : "NULL", list->attr_node != NULL ? list->attr_node->str : "NULL");

    assert(strcmp(data[i].key, list->key) == 0);
    assert(strcmp
           (data[i].value != NULL ? data[i].value : "NULL",
            list->value != NULL ? list->value : "NULL") == 0);
    fprintf(stderr, "attr: %s\n",
            list->attr_node != NULL ? list->attr_node->str : NULL);
    assert(strcmp
           (data[i].attribute != NULL ? data[i].attribute : "NULL",
            list->attr_node != NULL ? list->attr_node->str : "NULL") == 0);
    i++;

    canfigger_get_next_key(&list);
  }

  return 0;
}
