/* Example: parse a config file containing both typed scalar fields and
 * variable-length numeric attribute lists, using an offset-based dispatch
 * table (ConfigType/ConfigEntry) for the scalar fields and
 * canfigger_free_current_attr_str_advance() for the lists.
 *
 * Numeric attribute lists use the form:
 *   key = list, v1, v2, ...
 *
 * attributes->current is NULL when a node is first returned; the caller must
 * invoke canfigger_free_current_attr_str_advance() with attr=NULL to load
 * the first attribute before entering the loop.
 */

#include "tests/test.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifdef _MSC_VER
  #define strcasecmp _stricmp
#else
  #include <strings.h>
#endif

typedef enum {
  CFG_TYPE_STRING,
  CFG_TYPE_INT,
  CFG_TYPE_UINT8,
  CFG_TYPE_UINT16,
  CFG_TYPE_UINT32,
  CFG_TYPE_BOOL
} ConfigType;

typedef struct {
  const char *key;
  ConfigType  type;
  const char *default_value;
  size_t      offset;
  size_t      size;
} ConfigEntry;

#define MAX_NUMERIC_LIST 8
#define MAX_STR_LEN      64

typedef struct {
  char     host[MAX_STR_LEN];
  char     log_level[MAX_STR_LEN];
  char     description[MAX_STR_LEN];
  int      timeout;
  uint8_t  max_retries;
  uint16_t port;
  uint32_t max_connections;
  bool     verbose;
  uint32_t bet_amounts[MAX_NUMERIC_LIST];
  uint8_t  bet_amount_count;
  uint32_t difficulty_levels[MAX_NUMERIC_LIST];
  uint8_t  difficulty_level_count;
  uint32_t port_list[MAX_NUMERIC_LIST];
  uint8_t  port_list_count;
} AppConfig_t;

static const ConfigEntry entries[] = {
  { "host",            CFG_TYPE_STRING, "localhost", offsetof(AppConfig_t, host),            MAX_STR_LEN },
  { "log_level",       CFG_TYPE_STRING, "info",      offsetof(AppConfig_t, log_level),       MAX_STR_LEN },
  { "description",     CFG_TYPE_STRING, "",          offsetof(AppConfig_t, description),     MAX_STR_LEN },
  { "port",            CFG_TYPE_UINT16, "8080",      offsetof(AppConfig_t, port),            sizeof(uint16_t) },
  { "timeout",         CFG_TYPE_INT,    "-5",        offsetof(AppConfig_t, timeout),         sizeof(int) },
  { "max_retries",     CFG_TYPE_UINT8,  "3",         offsetof(AppConfig_t, max_retries),     sizeof(uint8_t) },
  { "max_connections", CFG_TYPE_UINT32, "100",       offsetof(AppConfig_t, max_connections), sizeof(uint32_t) },
  { "verbose",         CFG_TYPE_BOOL,   "no",        offsetof(AppConfig_t, verbose),         sizeof(bool) },
};

static void
config_set_field(AppConfig_t *cfg, const ConfigEntry *entry, const char *val)
{
  void *field = (uint8_t *)cfg + entry->offset;
  switch (entry->type)
  {
  case CFG_TYPE_STRING:
    snprintf((char *)field, entry->size, "%s", val);
    break;
  case CFG_TYPE_INT:
    *(int *)field = (int)strtol(val, NULL, 10);
    break;
  case CFG_TYPE_UINT8:
    *(uint8_t *)field = (uint8_t)strtoul(val, NULL, 10);
    break;
  case CFG_TYPE_UINT16:
    *(uint16_t *)field = (uint16_t)strtoul(val, NULL, 10);
    break;
  case CFG_TYPE_UINT32:
    *(uint32_t *)field = (uint32_t)strtoul(val, NULL, 10);
    break;
  case CFG_TYPE_BOOL:
    *(bool *)field = (strcasecmp(val, "yes") == 0
                      || strcasecmp(val, "true") == 0
                      || strcmp(val, "1") == 0);
    break;
  }
}

static void
print_uint32_list(const char *label, const uint32_t *arr, uint8_t count)
{
  printf("  %-20s", label);
  for (uint8_t i = 0; i < count; i++)
    printf("%s%u", i ? ", " : "", arr[i]);
  putchar('\n');
}

static void
print_config(const AppConfig_t *cfg)
{
  printf("\n--- Parsed config ---\n");
  printf("  %-20s%s\n",  "host:",            cfg->host);
  printf("  %-20s%s\n",  "log_level:",       cfg->log_level);
  printf("  %-20s%s\n",  "description:",     cfg->description);
  printf("  %-20s%u\n",  "port:",            cfg->port);
  printf("  %-20s%d\n",  "timeout:",         cfg->timeout);
  printf("  %-20s%u\n",  "max_retries:",     cfg->max_retries);
  printf("  %-20s%u\n",  "max_connections:", cfg->max_connections);
  printf("  %-20s%s\n",  "verbose:",         cfg->verbose ? "true" : "false");
  print_uint32_list("bet_amounts:",      cfg->bet_amounts,      cfg->bet_amount_count);
  print_uint32_list("difficulty_levels:", cfg->difficulty_levels, cfg->difficulty_level_count);
  print_uint32_list("port_list:",        cfg->port_list,        cfg->port_list_count);
  printf("---------------------\n");
}

static void
parse_numeric_list(struct Canfigger *node, uint32_t *out, uint8_t *count)
{
  char *attr = NULL;
  canfigger_free_current_attr_str_advance(node->attributes, &attr);
  while (attr && *count < MAX_NUMERIC_LIST)
  {
    printf("    attribute: %s\n", attr);
    out[(*count)++] = (uint32_t)strtoul(attr, NULL, 10);
    canfigger_free_current_attr_str_advance(node->attributes, &attr);
  }
}

int
main(int argc, char *argv[])
{
  char *default_filename = SOURCE_DIR "/example-02.conf";
  char *filename_ptr = default_filename;

  if (argc == 2)
    filename_ptr = argv[1];

  if (argc > 2)
  {
    fputs("This example program only accepts a single argument:\n\n", stderr);
    fprintf(stderr, "%s <config-file>\n\n", argv[0]);
    return -1;
  }

  AppConfig_t config = { 0 };

  /* Apply defaults for all scalar entries up front. */
  for (size_t i = 0; i < ARRAY_SIZE(entries); i++)
    config_set_field(&config, &entries[i], entries[i].default_value);

  struct Canfigger *list = canfigger_parse_file(filename_ptr, ',');
  if (!list)
    return -1;

  bool found_keys[ARRAY_SIZE(entries)];
  memset(found_keys, 0, sizeof(found_keys));

  printf("--- Parsing %s ---\n", filename_ptr);

  struct Canfigger *node = list;
  while (node)
  {
    printf("key: %s, value: %s\n", node->key,
           node->value ? node->value : "NULL");

    if (strcmp(node->key, "bet_amounts") == 0)
      parse_numeric_list(node, config.bet_amounts, &config.bet_amount_count);
    else if (strcmp(node->key, "difficulty_levels") == 0)
      parse_numeric_list(node, config.difficulty_levels, &config.difficulty_level_count);
    else if (strcmp(node->key, "port_list") == 0)
      parse_numeric_list(node, config.port_list, &config.port_list_count);
    else
    {
      for (size_t i = 0; i < ARRAY_SIZE(entries); i++)
      {
        if (strcmp(node->key, entries[i].key) == 0)
        {
          config_set_field(&config, &entries[i], node->value);
          found_keys[i] = true;
          break;
        }
      }
    }
    canfigger_free_current_key_node_advance(&node);
  }

  /* Apply defaults for any scalar keys absent from the file. */
  for (size_t i = 0; i < ARRAY_SIZE(entries); i++)
    if (!found_keys[i])
      config_set_field(&config, &entries[i], entries[i].default_value);

  if (filename_ptr != default_filename)
    return 0;

  print_config(&config);

  assert(strcmp(config.host, "example.com") == 0);
  assert(strcmp(config.log_level, "debug") == 0);
  assert(strcmp(config.description, "A sample configuration file") == 0);
  assert(config.port == 9000);
  assert(config.timeout == -10);
  assert(config.max_retries == 5);
  assert(config.max_connections == 200);
  assert(config.verbose == true);

  const uint32_t expected_bet[]        = { 100, 250, 500, 1000 };
  const uint32_t expected_difficulty[] = { 1, 2, 3, 5, 8 };
  const uint32_t expected_ports[]      = { 8080, 9090 };

  assert(config.bet_amount_count == ARRAY_SIZE(expected_bet));
  for (size_t i = 0; i < config.bet_amount_count; i++)
    assert(config.bet_amounts[i] == expected_bet[i]);

  assert(config.difficulty_level_count == ARRAY_SIZE(expected_difficulty));
  for (size_t i = 0; i < config.difficulty_level_count; i++)
    assert(config.difficulty_levels[i] == expected_difficulty[i]);

  assert(config.port_list_count == ARRAY_SIZE(expected_ports));
  for (size_t i = 0; i < config.port_list_count; i++)
    assert(config.port_list[i] == expected_ports[i]);

  return 0;
}
