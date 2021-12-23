#include "test.h"

int
main (void)
{
  struct expected
  {
    const char *key;
    const char *value;
    const char *attribute;
  };
  const struct expected data[] = {
    {"foo", "bar", ""},
    {"blue", "color", "shiny"},
    {"statement", "hello world", "obvious"},
    {"FeatureFooEnabled", "", ""},
  };

  int req_len =
    strlen ("test_canfigger.conf") + strlen (SOURCE_DIR) + strlen ("tests") +
    1 + 1 + 1;
  char test_config_file[req_len];
  sprintf (test_config_file, "%s/test_canfigger.conf", SOURCE_DIR);

  // call the primary library function to read your config file
  st_canfigger_list *list = canfigger_parse_file (test_config_file, ',');

  // create a pointer to the head of the list before examining the list.
  st_canfigger_list *root = list;
  if (list == NULL)
  {
    fprintf (stderr, "Error");
    return -1;
  }

  int i = 0;
  while (list != NULL)
  {
    printf ("\n\
Key: %s\n\
Value: %s\n\
Attribute: %s\n", list->key, list->value, list->attribute);

    assert (strcmp (data[i].key, list->key) == 0);
    assert (strcmp (data[i].value, list->value) == 0);
    assert (strcmp (data[i].attribute, list->attribute) == 0);
    i++;

    list = list->next;
  }

  // free the list
  canfigger_free (root);

  return 0;
}
