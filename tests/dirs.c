#ifndef _WIN32
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L // mkstemp()
#endif
/* Darwin sets __DARWIN_C_LEVEL from _POSIX_C_SOURCE when that is defined on its
   own, which HIDES mkdtemp(): Apple keeps it behind the full/BSD guard rather
   than the POSIX-2008 band (mkstemp is unaffected -- POSIX-2001 put that in
   stdlib.h). _DARWIN_C_SOURCE restores the full namespace and is ignored
   elsewhere. Without it the macOS CI job fails to compile. */
#ifdef __APPLE__
#define _DARWIN_C_SOURCE
#endif
#endif

#include "tests/test.h"

#ifndef _WIN32
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

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

  // Windows has no cache or state root separate from its data root, so these
  // carry a subdirectory; without it all three would be the same path and a
  // file written through one would be the file written through another.
  dir = canfigger_cache_dir("testapp");
  assert(dir != NULL);
  assert(strstr(dir, "testapp\\Cache") != NULL);
  free(dir);

  dir = canfigger_state_dir("testapp");
  assert(dir != NULL);
  assert(strstr(dir, "testapp\\State") != NULL);
  free(dir);

  // No runtime-directory concept on Windows.
  assert(canfigger_runtime_dir("testapp") == NULL);

  // The search-path lists hold the single ProgramData entry.
  char **dirs = canfigger_config_dirs();
  assert(dirs != NULL);
  assert(dirs[0] != NULL && dirs[1] == NULL);
  canfigger_free_dirs(dirs);

  dirs = canfigger_data_dirs();
  assert(dirs != NULL);
  assert(dirs[0] != NULL && dirs[1] == NULL);
  canfigger_free_dirs(dirs);

  canfigger_free_dirs(NULL);

  assert(canfigger_config_dir(NULL) == NULL);
  assert(canfigger_config_dir("") == NULL);
  assert(canfigger_data_dir(NULL) == NULL);
  assert(canfigger_data_dir("") == NULL);
  assert(canfigger_cache_dir(NULL) == NULL);
  assert(canfigger_cache_dir("") == NULL);
  assert(canfigger_state_dir(NULL) == NULL);
  assert(canfigger_state_dir("") == NULL);
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

  // XDG_STATE_HOME set
  setenv("XDG_STATE_HOME", "/tmp/teststate", 1);
  dir = canfigger_state_dir("myapp");
  assert(dir != NULL);
  assert(strcmp(dir, "/tmp/teststate/myapp") == 0);
  free(dir);
  unsetenv("XDG_STATE_HOME");

  // XDG_STATE_HOME unset: fall back to $HOME/.local/state
  dir = canfigger_state_dir("myapp");
  assert(dir != NULL);
  assert(strcmp(dir, "/tmp/testhome/.local/state/myapp") == 0);
  free(dir);

  // A relative value is invalid per the spec and must be ignored, not resolved
  // against the current directory; the $HOME default is used instead.
  setenv("XDG_CONFIG_HOME", "relative/path", 1);
  dir = canfigger_config_dir("myapp");
  assert(dir != NULL);
  assert(strcmp(dir, "/tmp/testhome/.config/myapp") == 0);
  free(dir);
  unsetenv("XDG_CONFIG_HOME");

  // NULL / empty guards
  assert(canfigger_config_dir(NULL) == NULL);
  assert(canfigger_config_dir("") == NULL);
  assert(canfigger_data_dir(NULL) == NULL);
  assert(canfigger_data_dir("") == NULL);
  assert(canfigger_cache_dir(NULL) == NULL);
  assert(canfigger_cache_dir("") == NULL);
  assert(canfigger_state_dir(NULL) == NULL);
  assert(canfigger_state_dir("") == NULL);
}

static void
test_runtime_dir(void)
{
  char *dir;

  // Unset is an ordinary outcome, not an error.
  unsetenv("XDG_RUNTIME_DIR");
  assert(canfigger_runtime_dir("myapp") == NULL);

  // Relative is invalid.
  setenv("XDG_RUNTIME_DIR", "relative/path", 1);
  assert(canfigger_runtime_dir("myapp") == NULL);

  // Absolute but nonexistent.
  setenv("XDG_RUNTIME_DIR", "/tmp/canfigger-does-not-exist-12345", 1);
  assert(canfigger_runtime_dir("myapp") == NULL);

  // A real directory we own with mode 0700 is accepted.
  char good[] = "/tmp/canfigger-rt-goodXXXXXX";
  assert(mkdtemp(good) != NULL);        // mkdtemp creates it 0700
  setenv("XDG_RUNTIME_DIR", good, 1);
  dir = canfigger_runtime_dir("myapp");
  assert(dir != NULL);
  {
    char expected[512];
    snprintf(expected, sizeof expected, "%s/myapp", good);
    assert(strcmp(dir, expected) == 0);
  }
  free(dir);

  // Loosening the mode makes it unusable: the spec requires 0700, and anything
  // wider means another user could read the sockets and secrets kept there.
  assert(chmod(good, 0755) == 0);
  assert(canfigger_runtime_dir("myapp") == NULL);

  // A file rather than a directory is rejected too.
  char file[] = "/tmp/canfigger-rt-fileXXXXXX";
  int fd = mkstemp(file);
  assert(fd >= 0);
  close(fd);
  setenv("XDG_RUNTIME_DIR", file, 1);
  assert(canfigger_runtime_dir("myapp") == NULL);

  assert(canfigger_runtime_dir(NULL) == NULL);
  assert(canfigger_runtime_dir("") == NULL);

  remove(file);
  rmdir(good);
  unsetenv("XDG_RUNTIME_DIR");
}

static void
test_dir_lists(void)
{
  char **dirs;

  // Unset: the spec's defaults.
  unsetenv("XDG_CONFIG_DIRS");
  dirs = canfigger_config_dirs();
  assert(dirs != NULL);
  assert(dirs[0] != NULL && strcmp(dirs[0], "/etc/xdg") == 0);
  assert(dirs[1] == NULL);
  canfigger_free_dirs(dirs);

  unsetenv("XDG_DATA_DIRS");
  dirs = canfigger_data_dirs();
  assert(dirs != NULL);
  assert(dirs[0] != NULL && strcmp(dirs[0], "/usr/local/share") == 0);
  assert(dirs[1] != NULL && strcmp(dirs[1], "/usr/share") == 0);
  assert(dirs[2] == NULL);
  canfigger_free_dirs(dirs);

  // Order is preserved, most important first.
  setenv("XDG_CONFIG_DIRS", "/one:/two:/three", 1);
  dirs = canfigger_config_dirs();
  assert(dirs != NULL);
  assert(strcmp(dirs[0], "/one") == 0);
  assert(strcmp(dirs[1], "/two") == 0);
  assert(strcmp(dirs[2], "/three") == 0);
  assert(dirs[3] == NULL);
  canfigger_free_dirs(dirs);

  // Relative and empty entries are dropped, the valid ones survive: one bad
  // entry in a search path should not cost the caller the good ones.
  setenv("XDG_CONFIG_DIRS", "/good:relative::/also-good:", 1);
  dirs = canfigger_config_dirs();
  assert(dirs != NULL);
  assert(strcmp(dirs[0], "/good") == 0);
  assert(strcmp(dirs[1], "/also-good") == 0);
  assert(dirs[2] == NULL);
  canfigger_free_dirs(dirs);

  // Every entry invalid: an empty list, not NULL.
  setenv("XDG_CONFIG_DIRS", "relative:another", 1);
  dirs = canfigger_config_dirs();
  assert(dirs != NULL);
  assert(dirs[0] == NULL);
  canfigger_free_dirs(dirs);
  unsetenv("XDG_CONFIG_DIRS");

  canfigger_free_dirs(NULL);    // no-op
}
#endif

int
main(void)
{
  test_path_join();
  test_dirs();
#ifndef _WIN32
  test_runtime_dir();
  test_dir_lists();
#endif
  return 0;
}
