
#ifdef NDEBUG
#undef NDEBUG
#endif

#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "canfigger.h"

struct expected
{
  const char *key;
  const char *value;
  const char *attribute;
};
// #define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

int
main ()
{

  const struct expected data[] = {
    {"foo", "bar", ""},
    {"blue", "green", "color"},
    {"FeatureFoo-enabled", "", ""},
  };


  int req_len = strlen ("test_canfigger.conf") + strlen (SOURCE_DIR) + 1 + 1;
  char test_config_file[req_len];
  sprintf (test_config_file, "%s/test_canfigger.conf", SOURCE_DIR);
  st_canfigger_list *list =
    canfigger_parse_file (test_config_file, ',');

  // create a pointer to the head of the list before examining the list.
  st_canfigger_list *head = list;

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

  canfigger_free (head);

  return 0;
}
