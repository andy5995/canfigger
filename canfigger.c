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

#include <ctype.h>              // isspace()
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>             // free(), malloc()
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
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


size_t
canfigger_get_int_attrs(const struct Canfigger *node, int *out, size_t max)
{
  if (!node || !node->attributes || !node->attributes->str || !out || !max)
    return 0;

  size_t count = 0;
  char *p = node->attributes->str;

  while (count < max)
  {
    while (*p == ' ' || *p == '\t')
      p++;

    if (*p == '\0' || *p == '\n')
      break;

    char *endptr;
    errno = 0;
    long val = strtol(p, &endptr, 10);
    if (endptr == p || errno == ERANGE)
      break;
    out[count++] = (int) val;

    p = endptr;
    while (*p == ' ' || *p == '\t')
      p++;

    if (*p == '\0')
      break;
    if (*p != '\n')
      break;
    p++;
  }

  return count;
}


size_t
canfigger_get_double_attrs(const struct Canfigger *node, double *out,
                           size_t max)
{
  if (!node || !node->attributes || !node->attributes->str || !out || !max)
    return 0;

  size_t count = 0;
  char *p = node->attributes->str;

  while (count < max)
  {
    while (*p == ' ' || *p == '\t')
      p++;

    if (*p == '\0' || *p == '\n')
      break;

    char *endptr;
    errno = 0;
    double val = strtod(p, &endptr);
    if (endptr == p || errno == ERANGE)
      break;
    out[count++] = val;

    p = endptr;
    while (*p == ' ' || *p == '\t')
      p++;

    if (*p == '\0')
      break;
    if (*p != '\n')
      break;
    p++;
  }

  return count;
}


int
canfigger_parse_color_hex(const char *str, uint8_t *r, uint8_t *g, uint8_t *b,
                          uint8_t *a)
{
  if (!str || *str != '#' || !r || !g || !b || !a)
    return 0;
  str++;
  size_t len = strlen(str);
  if (len != 6 && len != 8)
    return 0;
  unsigned int rv, gv, bv, av = 255;
  if (sscanf(str, "%2x%2x%2x", &rv, &gv, &bv) != 3)
    return 0;
  if (len == 8 && sscanf(str + 6, "%2x", &av) != 1)
    return 0;
  *r = (uint8_t) rv;
  *g = (uint8_t) gv;
  *b = (uint8_t) bv;
  *a = (uint8_t) av;
  return len == 6 ? 3 : 4;
}


int
canfigger_parse_color(const struct Canfigger *node, uint8_t *r, uint8_t *g,
                      uint8_t *b, uint8_t *a)
{
  if (!node || !r || !g || !b || !a)
    return 0;
  if (node->value && node->value[0] == '#')
    return canfigger_parse_color_hex(node->value, r, g, b, a);
  int v[4];
  size_t count = canfigger_get_int_attrs(node, v, 4);
  if (count < 3)
    return 0;
  *r = (uint8_t) v[0];
  *g = (uint8_t) v[1];
  *b = (uint8_t) v[2];
  *a = count == 4 ? (uint8_t) v[3] : 255;
  return (int) count;
}


#ifndef _WIN32
static char *
xdg_base_dir(const char *xdg_env, const char *fallback)
{
  const char *base = getenv(xdg_env);
  if (base && *base)
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


#ifdef _WIN32
static char *
dir_for_appname(const char *appname, int csidl)
{
  if (!appname || *appname == '\0')
    return NULL;

  char base[MAX_PATH];
  if (FAILED(SHGetFolderPathA(NULL, csidl, NULL, 0, base)))
    return NULL;

  size_t len = strlen(base) + 1 + strlen(appname) + 1;
  char *result = malloc_wrap(len);
  if (!result)
    return NULL;
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
  return dir_for_appname(appname, CSIDL_APPDATA);
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
  return dir_for_appname(appname, CSIDL_LOCAL_APPDATA);
#else
  return dir_for_appname(appname, "XDG_DATA_HOME", ".local/share");
#endif
}


char *
canfigger_cache_dir(const char *appname)
{
#ifdef _WIN32
  return dir_for_appname(appname, CSIDL_LOCAL_APPDATA);
#else
  return dir_for_appname(appname, "XDG_CACHE_HOME", ".cache");
#endif
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
