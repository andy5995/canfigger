#ifndef _WIN32
#define _POSIX_C_SOURCE 200112L
#endif

#include "tests/test.h"

#ifdef _WIN32
static void
test_config_file(void)
{
  char *path;

  path = canfigger_config_file("app.conf");
  assert(path != NULL);
  assert(strstr(path, "app.conf") != NULL);
  free(path);

  assert(canfigger_config_file(NULL) == NULL);
  assert(canfigger_config_file("") == NULL);
}
#else
static void
test_config_file(void)
{
  char *path;

  // XDG_CONFIG_HOME set
  setenv("XDG_CONFIG_HOME", "/tmp/testcfg", 1);
  path = canfigger_config_file("app.conf");
  assert(path != NULL);
  assert(strcmp(path, "/tmp/testcfg/app.conf") == 0);
  free(path);
  unsetenv("XDG_CONFIG_HOME");

  // XDG_CONFIG_HOME unset: fall back to $HOME/.config
  setenv("HOME", "/tmp/testhome", 1);
  path = canfigger_config_file("app.conf");
  assert(path != NULL);
  assert(strcmp(path, "/tmp/testhome/.config/app.conf") == 0);
  free(path);

  // NULL / empty guards
  assert(canfigger_config_file(NULL) == NULL);
  assert(canfigger_config_file("") == NULL);
}
#endif

int
main(void)
{
  test_config_file();
  return 0;
}
