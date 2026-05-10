#ifndef _WIN32
#define _POSIX_C_SOURCE 200112L
#endif

#include "tests/test.h"

static void
test_path_join(void)
{
  char *path;

#ifdef _WIN32
  path =
    canfigger_path_join("C:\\Users\\user\\AppData\\Roaming\\app",
                        "settings.conf");
  assert(path != NULL);
  assert(strcmp(path, "C:\\Users\\user\\AppData\\Roaming\\app\\settings.conf")
         == 0);
  free(path);

  // Trailing separator: no double backslash
  path =
    canfigger_path_join("C:\\Users\\user\\AppData\\Roaming\\app\\",
                        "settings.conf");
  assert(path != NULL);
  assert(strcmp(path, "C:\\Users\\user\\AppData\\Roaming\\app\\settings.conf")
         == 0);
  free(path);
#else
  path = canfigger_path_join("/home/user/.config/app", "settings.conf");
  assert(path != NULL);
  assert(strcmp(path, "/home/user/.config/app/settings.conf") == 0);
  free(path);

  // Trailing separator: no double slash
  path = canfigger_path_join("/home/user/.config/app/", "settings.conf");
  assert(path != NULL);
  assert(strcmp(path, "/home/user/.config/app/settings.conf") == 0);
  free(path);
#endif

  // NULL / empty guards
  assert(canfigger_path_join(NULL, "file.conf") == NULL);
  assert(canfigger_path_join("/dir", NULL) == NULL);
  assert(canfigger_path_join("", "file.conf") == NULL);
  assert(canfigger_path_join("/dir", "") == NULL);
}

#ifdef _WIN32
static void
test_dirs(void)
{
  char *dir;

  dir = canfigger_config_dir("testapp");
  assert(dir != NULL);
  assert(strstr(dir, "testapp") != NULL);
  free(dir);

  dir = canfigger_data_dir("testapp");
  assert(dir != NULL);
  assert(strstr(dir, "testapp") != NULL);
  free(dir);

  dir = canfigger_cache_dir("testapp");
  assert(dir != NULL);
  assert(strstr(dir, "testapp") != NULL);
  free(dir);

  assert(canfigger_config_dir(NULL) == NULL);
  assert(canfigger_config_dir("") == NULL);
  assert(canfigger_data_dir(NULL) == NULL);
  assert(canfigger_data_dir("") == NULL);
  assert(canfigger_cache_dir(NULL) == NULL);
  assert(canfigger_cache_dir("") == NULL);
}
#else
static void
test_dirs(void)
{
  char *dir;

  // XDG_CONFIG_HOME set
  setenv("XDG_CONFIG_HOME", "/tmp/testcfg", 1);
  dir = canfigger_config_dir("myapp");
  assert(dir != NULL);
  assert(strcmp(dir, "/tmp/testcfg/myapp") == 0);
  free(dir);
  unsetenv("XDG_CONFIG_HOME");

  // XDG_CONFIG_HOME unset: fall back to $HOME/.config
  setenv("HOME", "/tmp/testhome", 1);
  dir = canfigger_config_dir("myapp");
  assert(dir != NULL);
  assert(strcmp(dir, "/tmp/testhome/.config/myapp") == 0);
  free(dir);

  // XDG_DATA_HOME set
  setenv("XDG_DATA_HOME", "/tmp/testdata", 1);
  dir = canfigger_data_dir("myapp");
  assert(dir != NULL);
  assert(strcmp(dir, "/tmp/testdata/myapp") == 0);
  free(dir);
  unsetenv("XDG_DATA_HOME");

  // XDG_DATA_HOME unset: fall back to $HOME/.local/share
  dir = canfigger_data_dir("myapp");
  assert(dir != NULL);
  assert(strcmp(dir, "/tmp/testhome/.local/share/myapp") == 0);
  free(dir);

  // XDG_CACHE_HOME set
  setenv("XDG_CACHE_HOME", "/tmp/testcache", 1);
  dir = canfigger_cache_dir("myapp");
  assert(dir != NULL);
  assert(strcmp(dir, "/tmp/testcache/myapp") == 0);
  free(dir);
  unsetenv("XDG_CACHE_HOME");

  // XDG_CACHE_HOME unset: fall back to $HOME/.cache
  dir = canfigger_cache_dir("myapp");
  assert(dir != NULL);
  assert(strcmp(dir, "/tmp/testhome/.cache/myapp") == 0);
  free(dir);

  // NULL / empty guards
  assert(canfigger_config_dir(NULL) == NULL);
  assert(canfigger_config_dir("") == NULL);
  assert(canfigger_data_dir(NULL) == NULL);
  assert(canfigger_data_dir("") == NULL);
  assert(canfigger_cache_dir(NULL) == NULL);
  assert(canfigger_cache_dir("") == NULL);
}
#endif

int
main(void)
{
  test_path_join();
  test_dirs();
  return 0;
}
