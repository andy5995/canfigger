/*
This file is part of canfigger<https://github.com/andy5995/canfigger>

Copyright (C) 2021-2024 Andy Alt (arch_stanton5995@proton.me)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <ctype.h> // isspace()
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> // free(), malloc()
#include <string.h>
#include <stdarg.h> // valist (variable argument function)

// This is only required for version info and can be removed
// if you're copying the canfigger source files to use as
// an embedded library with your own project (i.e., not building
// canfigger with the build system it's shipped with).
#include "config.h"

#include "canfigger.h"

#define strdup_wrap(...) strdup_wrap_real(__VA_ARGS__, (size_t)0)

static char *grab_str_segment(char *a, char **dest, const int c);
static void free_list(struct Canfigger **node);

/** \cond */
struct line
{
  size_t len;
  char *start;
  char *end;
};
/** \endcond */


static char *
strdup_wrap_real(const char *argv, ...)
{
  va_list args;
  char *src = (char *) argv;
  va_start(args, argv);
  size_t n = va_arg(args, size_t); // Try to get the second argument

  char *retval = NULL;
  if (n == 0) // If n is 0, it means there was no second argument
    retval = strdup(src);
  else
    retval = strndup(src, n);

  va_end(args);
  if (retval)
    return retval;

  perror("Failed to duplicate string:");
  return NULL;
}


void
canfigger_free_current_attr_str_advance(struct attributes *attributes, char **attr)
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
                                     &attributes->current,
                                     '\n');

  *attr = attributes->current;
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


static void
free_list(struct Canfigger **node)
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

  while (isspace(*str))
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
    *dest = strdup_wrap(a);
    return b;                   // NULL
  }

  size_t len = b - a;
  *dest = strdup_wrap(a, len);
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

  perror("Failed to allocate memory: ");

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
  FILE *fp = fopen(filename, "r");
  if (!fp)
  {
    fprintf(stderr, "Failed to open %s:%s\n", filename, strerror(errno));
    return NULL;
  }

  fseek(fp, 0, SEEK_END);
  long file_size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  char *buffer = malloc_wrap(file_size + 1);
  if (!buffer)
  {
    fclose(fp);
    return NULL;
  }

  fread(buffer, 1, file_size, fp);
  buffer[file_size] = '\0';

  int r = ferror(fp);
  clearerr(fp);
  fclose(fp);

  if (r == 0)
    return buffer;

  fprintf(stderr, "Error reading %s(%d)\n", filename, r);
  return NULL;
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
      if ((*node)->attributes->str)
        free((*node)->attributes->str);
  }
  free(*node);

  return;
}


struct Canfigger *
canfigger_parse_file(const char *file, const int delimiter)
{
  struct Canfigger *root = NULL, *cur_node = NULL;

  char *buffer = read_entire_file(file);
  if (buffer == NULL)
    return NULL;

  char file_contents[strlen(buffer) + 1];
  memcpy(file_contents, buffer, sizeof file_contents);
  free(buffer);

  struct line line;
  line.start = file_contents;
  line.end = strchr(line.start, '\n');

  bool node_complete;

  while (line.end)
  {
    line.len = line.end - line.start;
    char tmp_line[line.len + 1];
    memcpy(tmp_line, line.start, line.len);
    tmp_line[line.len] = '\0';

    // Used in the next loop
    if (line.end)
    {
      line.start = line.end + 1;
      line.end = strchr(line.start, '\n');
    }

    char *line_ptr = tmp_line;
    truncate_whitespace(line_ptr);

    while (isspace(*line_ptr))
      line_ptr = erase_lead_char(*line_ptr, line_ptr);

    if (*line_ptr == '\0' || *line_ptr == '#')
      continue;

    node_complete = false;
    add_key_node(&root, &cur_node);
    if (!cur_node)
      break;

    // Get key
    cur_node->key = NULL;
    line_ptr = grab_str_segment(line_ptr, &cur_node->key, '=');
    if (!cur_node->key)
    {
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
        free_incomplete_node(&cur_node);
        break;
      }

      struct attributes *attr_ptr = cur_node->attributes;
      attr_ptr->current = NULL;

      attr_ptr->str = strdup_wrap(line_ptr);
      if (!attr_ptr->str)
      {
        free_incomplete_node(&cur_node);
        break;
      }

      attr_ptr->iter_ptr = attr_ptr->str;

      // Change the delimiter, which will be used later
      // in canfigger_free_current_attr_str_advance()
      char *delimiter_ptr = strchr(attr_ptr->iter_ptr, delimiter);
      while(delimiter_ptr)
      {
        *delimiter_ptr = '\n';
        delimiter_ptr = strchr(delimiter_ptr, delimiter);
      }
    }
    else
      cur_node->attributes = NULL;

    cur_node->next = NULL;
    node_complete = true;
  }

  if (!root)
    return NULL;

  if (!node_complete)
  {
    free_list(&root);
    return NULL;
  }

  return root;
}
