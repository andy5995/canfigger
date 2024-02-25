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
    {"foo", "bar", NULL},
    {"blue", "color", "shiny"},
    {"statement", "hello world", "obvious"},
    {"leadingSpace", "nullified", NULL},
    {"fookey", "bar-value", NULL},
    {"FeatureFooEnabled", NULL, NULL},
  };

  char test_config_file[PATH_MAX];
  assert((size_t)
         snprintf(test_config_file, sizeof test_config_file,
                  "%s/test_canfigger.conf",
                  SOURCE_DIR) < sizeof test_config_file);

  // call the primary library function to read your config file
  struct Canfigger *list = canfigger_parse_file(test_config_file, ',');

  if (list == NULL)
  {
    fprintf(stderr, "list == NULL");
    return -1;
  }

  int i = 0;
  while (list)
  {
    fprintf(stderr, "\n\
Key: %s | Expected: %s\n\
Value: %s | Expected: %s\n\
Attribute: %s | Expected: %s\n", list->key, data[i].key, list->value != NULL ? list->value : "NULL", data[i].value, list->attributes != NULL ? list->attributes->current : "NULL", data[i].attribute);

    assert(strcmp(data[i].key, list->key) == 0);
    assert(strcmp
           (data[i].value != NULL ? data[i].value : "NULL",
            list->value != NULL ? list->value : "NULL") == 0);
    fprintf(stderr, "attr: %s\n",
            list->attributes != NULL ? list->attributes->current : "NULL");
    assert(strcmp
           (data[i].attribute != NULL ? data[i].attribute : "NULL",
            list->attributes != NULL ? list->attributes->current : "NULL") == 0);
    i++;

    canfigger_free_current_key_node_advance(&list);
  }

  // 'list' should be NULL, not a dangling pointer
  assert(list == NULL);

  assert(i == ARRAY_SIZE(data));

  return 0;
}
