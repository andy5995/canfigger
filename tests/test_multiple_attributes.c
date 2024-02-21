#include "test.h"

int
main(void)
{
  const char *data1[] =
    { "one", "two", "three", "forty-four", "fifty five", "six", "7",
    "eight"
  };
  const char *data2[] =
    { "Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Neptune",
    "Uranus", "Pluto"
  };
  const char *data3[] = {
    "second",
    "after one",
  };

  char test_config_file[PATH_MAX];
  assert((size_t)
         snprintf(test_config_file, sizeof test_config_file,
                  "%s/multiple_attributes.conf",
                  SOURCE_DIR) < sizeof test_config_file);

  // call the primary library function to read your config file
  st_canfigger_list *list = canfigger_parse_file(test_config_file, ',');
  if (list == NULL)
  {
    fprintf(stderr, "Error");
    return -1;
  }

  int i = 0;
  do
  {
    printf("\n\
Key: %s\n\
Value: %s\n\
Attribute: %s\n", list->key, list->value, list->attr_node->str);

    // assert (strcmp (data[i].key, list->key) == 0);
    // assert (strcmp (data[i].value, list->value) == 0);
    int j = 0;

    do
    {
      fprintf(stderr, "attr: %s\n", list->attr_node->str);
      switch (i)
      {
      case 0:
        assert(strcmp(data1[j], list->attr_node->str) == 0);
        break;
      case 1:
        assert(strcmp(data2[j], list->attr_node->str) == 0);
        break;
      case 2:
        assert(strcmp(data3[j], list->attr_node->str) == 0);
        break;
      }
      j++;

      list->attr_node = canfigger_get_next_attr_list_node(list->attr_node);
    } while (list->attr_node != NULL);

    fprintf(stderr, "j: %d\n", j);
    if (j > 0)
    {
      switch (i)
      {
      case 0:
        assert(j == sizeof data1 / sizeof data1[0]);
        break;
      case 1:
        assert(j == sizeof data2 / sizeof data2[0]);
        break;
      case 2:
        assert(j == sizeof data3 / sizeof data3[0]);
        break;
      }
    }

    i++;

    list = canfigger_get_next_node(list);
  } while (list != NULL);

  assert(i == 3);

  return 0;
}
