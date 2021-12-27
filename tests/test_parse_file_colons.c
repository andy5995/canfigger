#include "test.h"

int
main (void)
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
  sprintf (test_config_file, "%s/test_canfigger_colons.conf", SOURCE_DIR);
  st_canfigger_list *list = canfigger_parse_file (test_config_file, ':');

  // create a pointer to the head of the list before examining the list.
  st_canfigger_list *head = list;

  if (list == NULL)
  {
    fprintf (stderr, "Error");
    return 1;
  }

  int i = 0;
  while (list != NULL)
  {
    printf ("\n\
Key: %s\n\
Value: %s\n\
Attribute: %s\n", list->key, list->value, list->attribute);

    assert (strcmp (data[i].key, list->key) == 0);
    // printf ("value = '%s' '%s'\n", data[i].value, list->value);
    assert (strcmp (data[i].value, list->value) == 0);
    assert (strcmp (data[i].attribute, list->attribute) == 0);
    i++;

    list = list->next;
  }

  assert (i == sizeof data / sizeof data[0]);

  canfigger_free (head);

  return 0;
}
