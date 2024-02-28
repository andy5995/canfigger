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
    {"foo", "bar", "high"},
    {"blue", "color", "shiny"},
    {"statement", "hello world", "obvious"},
  };

  struct Canfigger *list = canfigger_parse_file(SOURCE_DIR "/test_canfigger_colons.conf", ':');
  assert (list);

  int i = 0;
  while (list != NULL)
  {
    char *attr = NULL;
    canfigger_free_current_attr_str_advance(list->attributes, &attr);
    printf("\n\
Key: %s\n\
Value: %s\n\
Attribute: %s\n", list->key, list->value, attr);

    assert(strcmp(data[i].key, list->key) == 0);
    // printf ("value = '%s' '%s'\n", data[i].value, list->value);
    assert(strcmp(data[i].value, list->value) == 0);
    fprintf(stderr, "attr: %s\n", attr);
    assert(strcmp(data[i].attribute, attr) == 0);
    i++;

    canfigger_free_current_key_node_advance(&list);
  }

  assert(i == sizeof data / sizeof data[0]);

  return 0;
}
