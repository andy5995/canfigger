/*
This file is part of canfigger<https://github.com/andy5995/canfigger>

MIT License

Copyright (c) 2024 Andy Alt(arch_stanton5995@proton.me)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef _WIN32
/* stat() and getuid() are hidden by -std=c99 without this; canfigger_runtime_dir
   needs them to check that the runtime directory is really ours. */
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#endif

#include <ctype.h>              // isspace()
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>             // free(), malloc()
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

// This is only required for version info and can be removed
// if you're copying the canfigger source files to use as
// an embedded library with your own project (i.e., not building
// canfigger with the build system it's shipped with).
#include "canfigger_version.h"

#include "canfigger.h"

static char *grab_str_segment(char *a, char **dest, const int c);

/** \cond */
struct line
{
  size_t len;
  char *start;
  char *end;
};
/** \endcond */


static char *
strclone(const char *src, size_t n)
{
  char *dest = NULL;
  if (n == 0)
  {
    dest = malloc(strlen(src) + 1);
    if (dest)
      strcpy(dest, src);
  }
  else
  {
    dest = malloc(n + 1);
    if (dest)
    {
      memcpy(dest, src, n);
      dest[n] = '\0';
    }
  }

  if (!dest)
    perror("canfigger: malloc");
  return dest;
}


void
canfigger_free_current_attr_str_advance(struct attributes *attributes,
                                        char **attr)
{
  if (!attributes)
  {
    *attr = NULL;
    return;
  }

  if (attributes->current && attributes->iter_ptr)
    free(attributes->current);

  if (!attributes->iter_ptr)
  {
    free(attributes->current);
    attributes->current = NULL;
    *attr = NULL;
    return;
  }

  attributes->iter_ptr = grab_str_segment(attributes->iter_ptr,
                                          &attributes->current, '\n');

  if (*attributes->current)
  {
    *attr = attributes->current;
    return;
  }

  // If we're here, that means strdup() failed to allocate memory in grab_str_segment()
  // If an expected attribute isn't returned, the caller may want to terminate
  // the remainder of the loop that's iterating through the entire linked list
  // and exit the program.
  *attr = NULL;
  return;
}


void
canfigger_free_current_key_node_advance(struct Canfigger **node)
{
  if (*node)
  {
    if ((*node)->attributes)
    {
      if ((*node)->attributes->current)
      {
        free((*node)->attributes->current);
        (*node)->attributes->current = NULL;
      }

      if ((*node)->attributes->str)
      {
        free((*node)->attributes->str);
        (*node)->attributes->str = NULL;
      }

      free((*node)->attributes);
      (*node)->attributes = NULL;
    }

    if ((*node)->value)
    {
      free((*node)->value);
      (*node)->value = NULL;
    }

    free((*node)->key);
    (*node)->key = NULL;

    struct Canfigger *temp_node = (*node)->next;
    free(*node);
    *node = temp_node;
  }

  return;
}


void
canfigger_free_list(struct Canfigger **node)
{
  if (*node)
  {
    while (*node)
      canfigger_free_current_key_node_advance(node);
  }

  return;
}


/*
 * returns a pointer to the first character after lc
 * If lc appears more than once, the pointer
 * will move past that as well.
 *
 * Ex1: "__Hello World": the pointer will be set to the 'H'.
 * Ex2: "_H_ello World": Again, the pointer will be set to the 'H'.
 */
static char *
erase_lead_char(const int lc, char *haystack)
{
  char *ptr = haystack;
  if (*ptr != lc)
    return ptr;

  while (*ptr == lc)
    ptr++;

  return ptr;
}


static void
truncate_whitespace(char *str)
{
  if (!str)
    return;

  char *pos_0 = str;
  /* Advance pointer until NULL terminator is found
   * Don't try to use strchr() because you'll get a different
   * result if the pointer is already at '\0'. */
  while (*str != '\0')
    str++;

  /* set pointer to segment preceding NULL terminator */
  if (str != pos_0)
    str--;
  else
    return;

  while (isspace((unsigned char) *str))
  {
    *str = '\0';
    if (str != pos_0)
      str--;
    else
      break;
  }

  return;
}


static char *
grab_str_segment(char *a, char **dest, const int c)
{
  a = erase_lead_char(' ', a);

  char *b = strchr(a, c);
  if (!b)
  {
    *dest = strclone(a, 0);
    return b;
  }

  size_t len = b - a;
  *dest = strclone(a, len);
  if (!*dest)
    return NULL;

  truncate_whitespace(*dest);
  return b + 1;
}

static void *
malloc_wrap(size_t size)
{
  void *retval = malloc(size);
  if (retval)
    return retval;

  perror("canfigger: malloc");

  return NULL;
}

static void
add_key_node(struct Canfigger **root, struct Canfigger **cur_node)
{
  struct Canfigger *tmp_node = malloc_wrap(sizeof(struct Canfigger));
  if (!tmp_node)
    return;

  if (*root)
    (*cur_node)->next = tmp_node;
  else
    *root = tmp_node;

  *cur_node = tmp_node;

  return;
}


static char *
read_entire_file(const char *filename)
{
  char *buffer = NULL;
  long file_size;
  size_t n_bytes;

  FILE *fp = fopen(filename, "rb");
  if (!fp)
  {
    fprintf(stderr, "canfigger: Failed to open %s: %s\n", filename,
            strerror(errno));
    return NULL;
  }

  if (fseek(fp, 0, SEEK_END) != 0 || (file_size = ftell(fp)) < 0
      || fseek(fp, 0, SEEK_SET) != 0)
  {
    fprintf(stderr, "canfigger: Failed to determine size of %s: %s\n",
            filename, strerror(errno));
    goto done;
  }

  buffer = malloc_wrap(file_size + 1);
  if (!buffer)
    goto done;

  n_bytes = fread(buffer, 1, file_size, fp);
  if (n_bytes != (size_t) file_size)
  {
    if (ferror(fp))
      fprintf(stderr, "canfigger: Error reading %s: %s\n", filename,
              strerror(errno));
    else
      fprintf(stderr,
              "canfigger: Partial read of %s: expected %ld bytes, got %zu bytes\n",
              filename, file_size, n_bytes);
    free(buffer);
    buffer = NULL;
  }
  else
    buffer[file_size] = '\0';

done:
  fclose(fp);
  return buffer;
}


static void
free_incomplete_node(struct Canfigger **node)
{
  if (*node)
  {
    if ((*node)->key)
      free((*node)->key);

    if ((*node)->value)
      free((*node)->value);

    if ((*node)->attributes)
    {
      free((*node)->attributes->str);
      free((*node)->attributes);
    }
  }
  free(*node);

  return;
}


struct Canfigger *
canfigger_parse_file(const char *file, const int delimiter)
{
  struct Canfigger *root = NULL, *cur_node = NULL;

  char *file_contents = read_entire_file(file);
  if (file_contents == NULL)
    return NULL;

  /* Skip UTF-8 BOM (EF BB BF) if present. Some editors (especially on Windows)
   * prepend these three bytes silently; without this check they would be
   * prepended to the first key, corrupting any strcmp against it.
   * Array accesses are safe: read_entire_file allocates file_size+1 bytes with
   * a null terminator, and && short-circuits — [1] is only read when [0]
   * matched 0xEF (so at least 1 file byte exists), and [2] only when [1]
   * matched 0xBB (so at least 2 exist). */
  char *parse_start = file_contents;
  if ((unsigned char) parse_start[0] == 0xEF &&
      (unsigned char) parse_start[1] == 0xBB &&
      (unsigned char) parse_start[2] == 0xBF)
    parse_start += 3;

  struct line line;
  line.start = parse_start;

  bool node_complete = false;

  for (;;)
  {
    line.end = strchr(line.start, '\n');
    line.len =
      line.end ? (size_t) (line.end - line.start) : strlen(line.start);

    /* End of file with no remaining content */
    if (line.len == 0 && !line.end)
      break;

    char *tmp_line = malloc_wrap(line.len + 1);
    if (!tmp_line)
    {
      canfigger_free_list(&root);
      free(file_contents);
      return NULL;
    }

    memcpy(tmp_line, line.start, line.len);
    tmp_line[line.len] = '\0';
    line.start = line.end ? line.end + 1 : line.start + line.len;

    char *line_ptr = tmp_line;
    truncate_whitespace(line_ptr);

    while (isspace((unsigned char) *line_ptr))
      line_ptr = erase_lead_char(*line_ptr, line_ptr);

    if (*line_ptr == '\0' || *line_ptr == '#' || *line_ptr == '[')
    {
      free(tmp_line);
      if (!line.end)
        break;
      continue;
    }

    node_complete = false;
    struct Canfigger *prev_node = cur_node;
    add_key_node(&root, &cur_node);
    if (cur_node == prev_node)
    {
      free(tmp_line);
      break;
    }

    // Get key
    cur_node->key = NULL;
    line_ptr = grab_str_segment(line_ptr, &cur_node->key, '=');
    if (!cur_node->key)
    {
      free(tmp_line);
      free_incomplete_node(&cur_node);
      break;
    }

    // Get value
    cur_node->value = NULL;

    if (line_ptr)
    {
      line_ptr = grab_str_segment(line_ptr, &cur_node->value, delimiter);
      if (!cur_node->value)
      {
        free(tmp_line);
        free_incomplete_node(&cur_node);
        break;
      }
    }

    // Handle attributes
    if (line_ptr)
    {
      cur_node->attributes = malloc_wrap(sizeof(struct attributes));
      if (!cur_node->attributes)
      {
        free(tmp_line);
        free_incomplete_node(&cur_node);
        break;
      }

      struct attributes *attr_ptr = cur_node->attributes;
      attr_ptr->current = NULL;

      attr_ptr->str = strclone(line_ptr, 0);
      if (!attr_ptr->str)
      {
        free(tmp_line);
        free_incomplete_node(&cur_node);
        break;
      }

      attr_ptr->iter_ptr = attr_ptr->str;

      // Change the delimiter, which will be used later
      // in canfigger_free_current_attr_str_advance()
      char *delimiter_ptr = strchr(attr_ptr->iter_ptr, delimiter);
      while (delimiter_ptr)
      {
        *delimiter_ptr = '\n';
        delimiter_ptr = strchr(delimiter_ptr, delimiter);
      }
    }
    else
      cur_node->attributes = NULL;

    cur_node->next = NULL;
    node_complete = true;
    free(tmp_line);
    if (!line.end)
      break;
  }

  if (!root)
  {
    free(file_contents);
    return NULL;
  }

  if (!node_complete)
  {
    free(file_contents);
    canfigger_free_list(&root);
    return NULL;
  }

  free(file_contents);
  return root;
}


#ifndef _WIN32
static char *
xdg_base_dir(const char *xdg_env, const char *fallback)
{
  const char *base = getenv(xdg_env);
  /* The basedir spec requires these to be absolute and says a relative value is
     invalid and must be ignored -- resolving it against the current directory
     would put the file somewhere that depends on where the program was
     started. Fall through to the $HOME default instead. */
  if (base && *base == '/')
    return strclone(base, 0);

  const char *home = getenv("HOME");
  if (!home || !*home)
    return NULL;

  size_t len = strlen(home) + 1 + strlen(fallback) + 1;
  char *result = malloc_wrap(len);
  if (!result)
    return NULL;
  snprintf(result, len, "%s/%s", home, fallback);
  return result;
}
#endif


/* All the Windows helpers below go through SHGetFolderPathA, which is both
   deprecated and ANSI-only. This is accepted rather than overlooked: when the
   profile path cannot be represented in the active code page (a non-ASCII
   account name on a non-UTF-8 system) the call fails and the helper returns
   NULL, which is a clean failure rather than a corrupt path. Its replacement,
   SHGetKnownFolderPath, hands back UTF-16, and a UTF-8 char * built from that
   is not something MSVC's fopen will open -- so moving to it means a parallel
   wide-char API, not a drop-in swap. An application shipping a UTF-8
   active-code-page manifest (Windows 10 1903+) gets UTF-8 paths from this code
   as it stands. */
#ifdef _WIN32
/* @p subdir may be NULL. It exists because Windows has no separate cache or
   state root: everything below LOCAL_APPDATA would otherwise land on the same
   path, so canfigger_cache_dir() and canfigger_state_dir() would return exactly
   what canfigger_data_dir() does and a file written through one would be the
   file written through another. The subdirectory keeps them distinct, matching
   what Qt's QStandardPaths does for the same reason. */
static char *
dir_for_appname(const char *appname, int csidl, const char *subdir)
{
  if (!appname || *appname == '\0')
    return NULL;

  char base[MAX_PATH];
  if (FAILED(SHGetFolderPathA(NULL, csidl, NULL, 0, base)))
    return NULL;

  size_t len = strlen(base) + 1 + strlen(appname) + 1;
  if (subdir)
    len += strlen(subdir) + 1;
  char *result = malloc_wrap(len);
  if (!result)
    return NULL;
  if (subdir)
    snprintf(result, len, "%s\\%s\\%s", base, appname, subdir);
  else
    snprintf(result, len, "%s\\%s", base, appname);
  return result;
}
#else
static char *
dir_for_appname(const char *appname, const char *xdg_env,
                const char *xdg_fallback)
{
  if (!appname || *appname == '\0')
    return NULL;

  char *base = xdg_base_dir(xdg_env, xdg_fallback);
  if (!base)
    return NULL;
  char *result = canfigger_path_join(base, appname);
  free(base);
  return result;
}
#endif


char *
canfigger_config_dir(const char *appname)
{
#ifdef _WIN32
  return dir_for_appname(appname, CSIDL_APPDATA, NULL);
#else
  return dir_for_appname(appname, "XDG_CONFIG_HOME", ".config");
#endif
}


char *
canfigger_config_file(const char *filename)
{
  if (!filename || !*filename)
    return NULL;

#ifdef _WIN32
  char base[MAX_PATH];
  if (FAILED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, base)))
    return NULL;

  size_t len = strlen(base) + 1 + strlen(filename) + 1;
  char *result = malloc_wrap(len);
  if (!result)
    return NULL;
  snprintf(result, len, "%s\\%s", base, filename);
  return result;
#else
  char *base = xdg_base_dir("XDG_CONFIG_HOME", ".config");
  if (!base)
    return NULL;
  char *result = canfigger_path_join(base, filename);
  free(base);
  return result;
#endif
}


char *
canfigger_data_dir(const char *appname)
{
#ifdef _WIN32
  return dir_for_appname(appname, CSIDL_LOCAL_APPDATA, NULL);
#else
  return dir_for_appname(appname, "XDG_DATA_HOME", ".local/share");
#endif
}


char *
canfigger_cache_dir(const char *appname)
{
#ifdef _WIN32
  return dir_for_appname(appname, CSIDL_LOCAL_APPDATA, "Cache");
#else
  return dir_for_appname(appname, "XDG_CACHE_HOME", ".cache");
#endif
}


char *
canfigger_state_dir(const char *appname)
{
#ifdef _WIN32
  return dir_for_appname(appname, CSIDL_LOCAL_APPDATA, "State");
#else
  return dir_for_appname(appname, "XDG_STATE_HOME", ".local/state");
#endif
}


char *
canfigger_runtime_dir(const char *appname)
{
  if (!appname || *appname == '\0')
    return NULL;

#ifdef _WIN32
  /* Windows has no runtime-directory concept. Returning NULL rather than a
     LOCAL_APPDATA path keeps the contract honest: callers asking for a runtime
     directory want one that goes away with the session, and a path under
     LOCAL_APPDATA does not. Let them choose their own fallback. */
  return NULL;
#else
  const char *base = getenv("XDG_RUNTIME_DIR");
  if (!base || *base != '/')
    return NULL;

  /* The spec does not merely name this directory, it states what it must be:
     owned by the user, accessible to nobody else, mode 0700. A directory that
     fails those checks is not a safe place for the sockets, lock files and
     short-lived secrets this is used for, so treat it as absent and let the
     caller fall back rather than handing back a path that looks usable. */
  struct stat st;
  if (stat(base, &st) != 0)
    return NULL;
  if (!S_ISDIR(st.st_mode))
    return NULL;
  if (st.st_uid != getuid())
    return NULL;
  if ((st.st_mode & 07777) != 0700)
    return NULL;

  return canfigger_path_join(base, appname);
#endif
}


#ifdef _WIN32
/* Windows has no search *path*: ProgramData is the one system-wide location for
   settings and data shared by every user, so the list always holds exactly one
   entry. It is still handed back as a list so a caller can walk
   canfigger_config_dirs() / canfigger_data_dirs() the same way on both
   platforms instead of bracketing the loop in #ifdefs. */
static char **
common_appdata_list(void)
{
  char base[MAX_PATH];
  if (FAILED(SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA, NULL, 0, base)))
    return NULL;

  char **list = malloc_wrap(2 * sizeof(*list));
  if (!list)
    return NULL;

  list[0] = strclone(base, 0);
  if (!list[0])
  {
    free(list);
    return NULL;
  }
  list[1] = NULL;
  return list;
}
#endif


#ifndef _WIN32
/* Split a colon-separated XDG search path into a NULL-terminated array. */
static char **
xdg_dir_list(const char *xdg_env, const char *fallback)
{
  const char *value = getenv(xdg_env);
  if (!value || *value == '\0')
    value = fallback;

  size_t max = 2;
  const char *p;
  for (p = value; *p != '\0'; p++)
    if (*p == ':')
      max++;

  char **list = malloc_wrap(max * sizeof(*list));
  if (!list)
    return NULL;

  size_t n = 0;
  const char *start = value;
  for (;;)
  {
    const char *end = strchr(start, ':');
    size_t len = end ? (size_t) (end - start) : strlen(start);

    /* Same rule as xdg_base_dir: a relative entry is invalid per the spec, and
       an empty one ("a::b") is not a path. Skip both rather than failing the
       whole list -- one bad entry in a search path should not cost the caller
       the good ones. */
    if (len > 0 && *start == '/')
    {
      list[n] = strclone(start, len);
      if (!list[n])
      {
        list[n] = NULL;
        canfigger_free_dirs(list);
        return NULL;
      }
      n++;
    }

    if (!end)
      break;
    start = end + 1;
  }

  list[n] = NULL;
  return list;
}
#endif


char **
canfigger_config_dirs(void)
{
#ifdef _WIN32
  return common_appdata_list();
#else
  return xdg_dir_list("XDG_CONFIG_DIRS", "/etc/xdg");
#endif
}


char **
canfigger_data_dirs(void)
{
#ifdef _WIN32
  return common_appdata_list();
#else
  return xdg_dir_list("XDG_DATA_DIRS", "/usr/local/share:/usr/share");
#endif
}


void
canfigger_free_dirs(char **dirs)
{
  if (!dirs)
    return;

  size_t i;
  for (i = 0; dirs[i] != NULL; i++)
    free(dirs[i]);
  free(dirs);
}


char *
canfigger_path_join(const char *dir, const char *file)
{
  if (!dir || !*dir || !file || !*file)
    return NULL;

  size_t dirlen = strlen(dir);
  size_t filelen = strlen(file);
  bool needs_sep = dirlen > 0 && dir[dirlen - 1] != '/'
    && dir[dirlen - 1] != '\\';
  size_t total = dirlen + (needs_sep ? 1 : 0) + filelen + 1;

  char *result = malloc_wrap(total);
  if (!result)
    return NULL;

#ifdef _WIN32
  const char sep = '\\';
#else
  const char sep = '/';
#endif

  if (needs_sep)
    snprintf(result, total, "%s%c%s", dir, sep, file);
  else
    snprintf(result, total, "%s%s", dir, file);

  return result;
}


/* Does the path name an existing regular file? A directory of the right name is
   not a config file, so it must not stop the search short of one further down
   the path. */
static bool
path_is_file(const char *path)
{
#ifdef _WIN32
  DWORD attr = GetFileAttributesA(path);
  return attr != INVALID_FILE_ATTRIBUTES
    && !(attr & FILE_ATTRIBUTE_DIRECTORY);
#else
  struct stat st;
  return stat(path, &st) == 0 && S_ISREG(st.st_mode);
#endif
}


char *
canfigger_find_config_file(const char *appname, const char *filename)
{
  if (!filename || !*filename)
    return NULL;

  char *candidate = NULL;

  /* The user's own config always wins, whatever the system copies hold. */
  if (appname && *appname)
  {
    char *dir = canfigger_config_dir(appname);
    if (dir)
    {
      candidate = canfigger_path_join(dir, filename);
      free(dir);
    }
  }
  else
    candidate = canfigger_config_file(filename);

  if (candidate && path_is_file(candidate))
    return candidate;
  free(candidate);

  char **dirs = canfigger_config_dirs();
  if (!dirs)
    return NULL;

  char *found = NULL;
  size_t i;
  for (i = 0; dirs[i] != NULL && !found; i++)
  {
    char *base = NULL;
    if (appname && *appname)
    {
      base = canfigger_path_join(dirs[i], appname);
      if (!base)
        continue;
    }

    candidate = canfigger_path_join(base ? base : dirs[i], filename);
    free(base);

    if (candidate && path_is_file(candidate))
      found = candidate;
    else
      free(candidate);
  }

  canfigger_free_dirs(dirs);
  return found;
}


/* Keyed by enum canfigger_user_dir so the rows cannot silently shift if the
   enum is ever reordered. A new enumerator with no row here still compiles: the
   entry is a null pointer, which canfigger_user_dir() turns into its ordinary
   NULL return rather than a crash. */
#ifndef _WIN32
static const char *const user_dir_keys[] = {
  [CANFIGGER_USER_DIR_DESKTOP] = "XDG_DESKTOP_DIR",
  [CANFIGGER_USER_DIR_DOWNLOAD] = "XDG_DOWNLOAD_DIR",
  [CANFIGGER_USER_DIR_TEMPLATES] = "XDG_TEMPLATES_DIR",
  [CANFIGGER_USER_DIR_PUBLICSHARE] = "XDG_PUBLICSHARE_DIR",
  [CANFIGGER_USER_DIR_DOCUMENTS] = "XDG_DOCUMENTS_DIR",
  [CANFIGGER_USER_DIR_MUSIC] = "XDG_MUSIC_DIR",
  [CANFIGGER_USER_DIR_PICTURES] = "XDG_PICTURES_DIR",
  [CANFIGGER_USER_DIR_VIDEOS] = "XDG_VIDEOS_DIR"
};


/* user-dirs.dirs is a shell fragment sourced by the desktop session, not a
   canfigger file, so it cannot go through canfigger_parse_file(): its values
   are double-quoted, may contain backslash escapes, and the "$HOME/" prefix has
   to be expanded. This mirrors the reference xdg-user-dir-lookup implementation
   -- deliberately, since matching whatever the desktop itself resolves matters
   more here than being lenient. */
static char *
user_dir_from_file(const char *key)
{
  char *path = canfigger_config_file("user-dirs.dirs");
  if (!path)
    return NULL;

  FILE *fp = fopen(path, "r");
  free(path);
  if (!fp)
    return NULL;

  static const char home_prefix[] = "$HOME/";
  const char *home = getenv("HOME");
  size_t keylen = strlen(key);
  char line[1024];
  char *result = NULL;

  while (!result && fgets(line, sizeof line, fp) != NULL)
  {
    char *p = line;
    while (isspace((unsigned char) *p))
      p++;
    if (strncmp(p, key, keylen) != 0)
      continue;

    p += keylen;
    while (isspace((unsigned char) *p))
      p++;
    if (*p != '=')
      continue;
    p++;
    while (isspace((unsigned char) *p))
      p++;
    if (*p != '"')
      continue;
    p++;

    bool relative_to_home = false;
    if (strncmp(p, home_prefix, sizeof home_prefix - 1) == 0)
    {
      relative_to_home = true;
      p += sizeof home_prefix - 1;
    }
    /* Anything neither absolute nor under $HOME is unusable: an entry like
       "Desktop" would resolve against the current directory. */
    else if (*p != '/')
      break;

    size_t prefix = 0;
    if (relative_to_home)
    {
      if (!home || !*home)
        break;
      prefix = strlen(home) + 1;
    }

    char *out = malloc_wrap(prefix + strlen(p) + 1);
    if (!out)
      break;
    if (relative_to_home)
      snprintf(out, prefix + 1, "%s/", home);

    char *d = out + prefix;
    while (*p != '\0' && *p != '"')
    {
      if (*p == '\\' && *(p + 1) != '\0')
        p++;
      *d++ = *p++;
    }
    *d = '\0';

    /* An empty value (XDG_DESKTOP_DIR="") disables the directory; treat it as
       absent so the caller gets the $HOME fallback rather than "". */
    if (*out == '\0')
    {
      free(out);
      break;
    }
    result = out;
  }

  fclose(fp);
  return result;
}
#else
/* Keyed by enum canfigger_user_dir, as user_dir_keys[] is. CSIDL_PROFILE stands
   in for Downloads, which has no CSIDL at all (it arrived with the Vista-era
   KNOWNFOLDERID API); the name is appended below. Unlike the POSIX table a
   missing row is not detectable here -- 0 is CSIDL_DESKTOP -- so every
   enumerator needs one. */
static const int user_dir_csidl[] = {
  [CANFIGGER_USER_DIR_DESKTOP] = CSIDL_DESKTOPDIRECTORY,
  [CANFIGGER_USER_DIR_DOWNLOAD] = CSIDL_PROFILE,
  [CANFIGGER_USER_DIR_TEMPLATES] = CSIDL_TEMPLATES,
  [CANFIGGER_USER_DIR_PUBLICSHARE] = CSIDL_COMMON_DOCUMENTS,
  [CANFIGGER_USER_DIR_DOCUMENTS] = CSIDL_PERSONAL,
  [CANFIGGER_USER_DIR_MUSIC] = CSIDL_MYMUSIC,
  [CANFIGGER_USER_DIR_PICTURES] = CSIDL_MYPICTURES,
  [CANFIGGER_USER_DIR_VIDEOS] = CSIDL_MYVIDEO
};
#endif


char *
canfigger_user_dir(enum canfigger_user_dir which)
{
#ifdef _WIN32
  if ((unsigned) which >= sizeof user_dir_csidl / sizeof *user_dir_csidl)
    return NULL;

  char base[MAX_PATH];
  if (FAILED(SHGetFolderPathA(NULL, user_dir_csidl[which], NULL, 0, base)))
    return NULL;

  if (which == CANFIGGER_USER_DIR_DOWNLOAD)
    return canfigger_path_join(base, "Downloads");
  return strclone(base, 0);
#else
  if ((unsigned) which >= sizeof user_dir_keys / sizeof *user_dir_keys)
    return NULL;

  if (!user_dir_keys[which])
    return NULL;

  char *dir = user_dir_from_file(user_dir_keys[which]);
  if (dir)
    return dir;

  const char *home = getenv("HOME");
  if (!home || !*home)
    return NULL;

  /* The reference implementation falls back to $HOME for every type but the
     desktop, which keeps its historical $HOME/Desktop default. */
  if (which == CANFIGGER_USER_DIR_DESKTOP)
    return canfigger_path_join(home, "Desktop");
  return strclone(home, 0);
#endif
}
