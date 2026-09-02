#ifndef _WIN32
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L // mkdtemp(), setenv()
#endif
/* See the comment in tests/dirs.c: without this, defining _POSIX_C_SOURCE on
   its own hides mkdtemp() on macOS. */
#ifdef __APPLE__
#define _DARWIN_C_SOURCE
#endif
#endif

#include "tests/test.h"

#ifndef _WIN32
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static char root[] = "/tmp/canfigger-findXXXXXX";

static void
make_dir(const char *fmt, const char *a, const char *b)
{
  char path[512];
  if (b != NULL)
    snprintf(path, sizeof path, fmt, root, a, b);
  else
    snprintf(path, sizeof path, fmt, root, a);
  assert(mkdir(path, 0755) == 0 || errno == EEXIST);
}

static void
make_file(char *out, size_t outlen, const char *fmt, const char *a,
          const char *b)
{
  if (b != NULL)
    snprintf(out, outlen, fmt, root, a, b);
  else
    snprintf(out, outlen, fmt, root, a);

  FILE *fp = fopen(out, "w");
  assert(fp != NULL);
  fputs("key = value\n", fp);
  fclose(fp);
}

int
main(void)
{
  char *found;
  char user_file[512], sys1_file[512], sys2_file[512], flat_file[512];
  char cfg_home[512], sysdirs[1024];

  assert(mkdtemp(root) != NULL);

  make_dir("%s/%s", "user", NULL);
  make_dir("%s/%s", "sys1", NULL);
  make_dir("%s/%s", "sys2", NULL);
  make_dir("%s/%s/%s", "user", "myapp");
  make_dir("%s/%s/%s", "sys1", "myapp");
  make_dir("%s/%s/%s", "sys2", "myapp");

  snprintf(cfg_home, sizeof cfg_home, "%s/user", root);
  snprintf(sysdirs, sizeof sysdirs, "%s/sys1:%s/sys2", root, root);
  setenv("XDG_CONFIG_HOME", cfg_home, 1);
  setenv("XDG_CONFIG_DIRS", sysdirs, 1);

  // Nothing exists anywhere yet.
  assert(canfigger_find_config_file("myapp", "app.conf") == NULL);

  // Only the last system directory has it.
  make_file(sys2_file, sizeof sys2_file, "%s/%s/%s/app.conf", "sys2",
            "myapp");
  found = canfigger_find_config_file("myapp", "app.conf");
  assert(found != NULL);
  assert(strcmp(found, sys2_file) == 0);
  free(found);

  // An earlier system directory takes precedence over a later one.
  make_file(sys1_file, sizeof sys1_file, "%s/%s/%s/app.conf", "sys1",
            "myapp");
  found = canfigger_find_config_file("myapp", "app.conf");
  assert(found != NULL);
  assert(strcmp(found, sys1_file) == 0);
  free(found);

  // The user's own copy beats every system copy.
  make_file(user_file, sizeof user_file, "%s/%s/%s/app.conf", "user",
            "myapp");
  found = canfigger_find_config_file("myapp", "app.conf");
  assert(found != NULL);
  assert(strcmp(found, user_file) == 0);
  free(found);

  // A NULL appname looks directly under the config roots.
  assert(canfigger_find_config_file(NULL, "flat.conf") == NULL);
  make_file(flat_file, sizeof flat_file, "%s/%s/flat.conf", "sys1", NULL);
  found = canfigger_find_config_file(NULL, "flat.conf");
  assert(found != NULL);
  assert(strcmp(found, flat_file) == 0);
  free(found);
  // An empty appname behaves the same as NULL.
  found = canfigger_find_config_file("", "flat.conf");
  assert(found != NULL);
  assert(strcmp(found, flat_file) == 0);
  free(found);

  // A directory of the right name is not a config file, and must not stop the
  // search before the real file further down the path.
  make_dir("%s/%s/%s/dir.conf", "user", "myapp");
  make_file(sys1_file, sizeof sys1_file, "%s/%s/%s/dir.conf", "sys1",
            "myapp");
  found = canfigger_find_config_file("myapp", "dir.conf");
  assert(found != NULL);
  assert(strcmp(found, sys1_file) == 0);
  free(found);

  assert(canfigger_find_config_file("myapp", NULL) == NULL);
  assert(canfigger_find_config_file("myapp", "") == NULL);

  // Clean up: rmdir/remove in reverse order of creation.
  char path[512];
  const char *bases[] = { "user", "sys1", "sys2" };
  size_t i;
  for (i = 0; i < 3; i++)
  {
    snprintf(path, sizeof path, "%s/%s/myapp/app.conf", root, bases[i]);
    remove(path);
    snprintf(path, sizeof path, "%s/%s/myapp/dir.conf", root, bases[i]);
    remove(path);
    rmdir(path);
    snprintf(path, sizeof path, "%s/%s/myapp", root, bases[i]);
    rmdir(path);
    snprintf(path, sizeof path, "%s/%s/flat.conf", root, bases[i]);
    remove(path);
    snprintf(path, sizeof path, "%s/%s", root, bases[i]);
    rmdir(path);
  }
  rmdir(root);

  unsetenv("XDG_CONFIG_HOME");
  unsetenv("XDG_CONFIG_DIRS");
  return 0;
}
#else
int
main(void)
{
  // The search path exists on Windows too (%APPDATA% then %ProgramData%), but
  // there is nothing to find in a test environment.
  assert(canfigger_find_config_file("canfigger-no-such-app", "nope.conf") ==
         NULL);
  assert(canfigger_find_config_file("myapp", NULL) == NULL);
  assert(canfigger_find_config_file("myapp", "") == NULL);
  return 0;
}
#endif
