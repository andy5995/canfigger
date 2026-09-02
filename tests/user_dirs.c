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

static char root[] = "/tmp/canfigger-userdirsXXXXXX";
static char cfg_home[512];

static void
write_user_dirs(const char *contents)
{
  char path[600];
  snprintf(path, sizeof path, "%s/user-dirs.dirs", cfg_home);
  FILE *fp = fopen(path, "w");
  assert(fp != NULL);
  fputs(contents, fp);
  fclose(fp);
}

static void
check(enum canfigger_user_dir which, const char *expected)
{
  char *dir = canfigger_user_dir(which);
  assert(dir != NULL);
  assert(strcmp(dir, expected) == 0);
  free(dir);
}

int
main(void)
{
  assert(mkdtemp(root) != NULL);
  snprintf(cfg_home, sizeof cfg_home, "%s/config", root);
  assert(mkdir(cfg_home, 0755) == 0);

  setenv("XDG_CONFIG_HOME", cfg_home, 1);
  setenv("HOME", "/tmp/testhome", 1);

  // No user-dirs.dirs at all: $HOME for everything, except the desktop's
  // historical $HOME/Desktop default.
  check(CANFIGGER_USER_DIR_DESKTOP, "/tmp/testhome/Desktop");
  check(CANFIGGER_USER_DIR_DOCUMENTS, "/tmp/testhome");
  check(CANFIGGER_USER_DIR_DOWNLOAD, "/tmp/testhome");

  write_user_dirs("# This file is written by xdg-user-dirs-update\n"
                  "XDG_DESKTOP_DIR=\"$HOME/Skrivebord\"\n"
                  "XDG_DOWNLOAD_DIR=\"$HOME/Downloads\"\n"
                  "XDG_DOCUMENTS_DIR=\"/srv/shared/docs\"\n"
                  "XDG_MUSIC_DIR=\"$HOME/Musik\"\n");

  // A localised name, an absolute path, and an entry the file does not mention.
  check(CANFIGGER_USER_DIR_DESKTOP, "/tmp/testhome/Skrivebord");
  check(CANFIGGER_USER_DIR_DOWNLOAD, "/tmp/testhome/Downloads");
  check(CANFIGGER_USER_DIR_DOCUMENTS, "/srv/shared/docs");
  check(CANFIGGER_USER_DIR_MUSIC, "/tmp/testhome/Musik");
  check(CANFIGGER_USER_DIR_PICTURES, "/tmp/testhome");

  // Backslash escapes, and whitespace around the assignment.
  write_user_dirs("  XDG_VIDEOS_DIR = \"$HOME/My \\\"Videos\\\"\"\n");
  check(CANFIGGER_USER_DIR_VIDEOS, "/tmp/testhome/My \"Videos\"");

  // An empty value disables the directory; a relative one is unusable. Both
  // fall back rather than returning "" or a path built from the cwd.
  write_user_dirs("XDG_TEMPLATES_DIR=\"\"\n"
                  "XDG_PUBLICSHARE_DIR=\"Public\"\n");
  check(CANFIGGER_USER_DIR_TEMPLATES, "/tmp/testhome");
  check(CANFIGGER_USER_DIR_PUBLICSHARE, "/tmp/testhome");

  // An unquoted value is not what this file may contain, and is ignored.
  write_user_dirs("XDG_MUSIC_DIR=$HOME/Music\n");
  check(CANFIGGER_USER_DIR_MUSIC, "/tmp/testhome");

  // Out of range.
  assert(canfigger_user_dir((enum canfigger_user_dir) 99) == NULL);

  char path[600];
  snprintf(path, sizeof path, "%s/user-dirs.dirs", cfg_home);
  remove(path);
  rmdir(cfg_home);
  rmdir(root);
  unsetenv("XDG_CONFIG_HOME");
  return 0;
}
#else
int
main(void)
{
  char *dir = canfigger_user_dir(CANFIGGER_USER_DIR_DOCUMENTS);
  assert(dir != NULL);
  free(dir);

  dir = canfigger_user_dir(CANFIGGER_USER_DIR_DOWNLOAD);
  assert(dir != NULL);
  assert(strstr(dir, "Downloads") != NULL);
  free(dir);

  assert(canfigger_user_dir((enum canfigger_user_dir) 99) == NULL);
  return 0;
}
#endif
