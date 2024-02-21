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
    {"blue", "color", "shiny"},
    {"statement", "hello world", "obvious"},
    {"FeatureFooEnabled", "", ""},
  };

  char test_config_file[PATH_MAX];
  sprintf(test_config_file, "%s/test_canfigger_colons.conf", SOURCE_DIR);
  st_canfigger_list *list = canfigger_parse_file(test_config_file, ':');
  if (list == NULL)
  {
    fprintf(stderr, "Error");
    return 1;
  }

  int i = 0;
  do
  {
    printf("\n\
Key: %s\n\
Value: %s\n\
Attribute: %s\n", list->key, list->value, list->attr_node->str);

    assert(strcmp(data[i].key, list->key) == 0);
    // printf ("value = '%s' '%s'\n", data[i].value, list->value);
    assert(strcmp(data[i].value, list->value) == 0);
    fprintf(stderr, "attr: %s\n", list->attr_node->str);
    assert(strcmp(data[i].attribute, list->attr_node->str) == 0);
    i++;

    list = canfigger_get_next_node(list);
  } while (list != NULL);

  assert(i == sizeof data / sizeof data[0]);

  return 0;
}
