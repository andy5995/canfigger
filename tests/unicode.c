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

  // call the primary library function to read your config file
  struct Canfigger *list = canfigger_parse_file(SOURCE_DIR "/test_unicode.conf", ';');
  assert (list);

  int i = 0;
  while (list)
  {
    char *attr = NULL;
    canfigger_free_current_attr_str_advance(list->attributes, &attr);
    printf("\n\
Key: %s\n\
Value: %s\n\
Attribute: %s\n", list->key, list->value != NULL ? list->value : "NULL", attr != NULL ? attr : "NULL");

    assert(strcmp(data[i].key, list->key) == 0);
    assert(strcmp
           (data[i].value != NULL ? data[i].value : "NULL",
            list->value != NULL ? list->value : "NULL") == 0);
    fprintf(stderr, "attr: %s\n",
            attr != NULL ? attr : "NULL");
    assert(strcmp
           (data[i].attribute != NULL ? data[i].attribute : "NULL",
            attr != NULL ? attr : "NULL") == 0);
    i++;

    canfigger_free_current_key_node_advance(&list);
  }

  return 0;
}
