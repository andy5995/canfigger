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
#include <stdio.h>
#include <stdlib.h>             // free(), malloc()
#include <string.h>

// This is only required for version info and can be removed
// if you're copying the canfigger source files to use as
// an embedded library with your own project (i.e., not building
// canfigger with the build system it's shipped with).
#include "config.h"

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
    perror("malloc (canfigger)");
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
  FILE *fp = fopen(filename, "rb");
  if (!fp)
  {
    fprintf(stderr, "Failed to open %s: %s\n", filename, strerror(errno));
    return NULL;
  }

  fseek(fp, 0, SEEK_END);
  long file_size = ftell(fp);
  if (file_size < 0)
  {
    fprintf(stderr, "Error getting the size of %s: %s\n", filename,
            strerror(errno));
    fclose(fp);
    return NULL;
  }
  fseek(fp, 0, SEEK_SET);

  char *buffer = malloc_wrap(file_size + 1);
  if (!buffer)
  {
    fclose(fp);
    return NULL;
  }

  size_t n_bytes = fread(buffer, 1, file_size, fp);

  if (ferror(fp))
  {
    fprintf(stderr, "Error reading %s: %s\n", filename, strerror(errno));
    free(buffer);
    fclose(fp);
    return NULL;
  }

  // Note that if the return value of ftell() is -1 this cast would be bad.
  // However, above, the return value of ftell() is checked, and the function
  // returns if the value is < 0
  if (n_bytes == (size_t) file_size)
  {
    buffer[file_size] = '\0';
    fclose(fp);
    return buffer;
  }

  free(buffer);
  fprintf(stderr, "Partial read of %s: expected %ld bytes, got %zu bytes\n",
          filename, file_size, n_bytes);
  fclose(fp);
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

  size_t buffer_len = strlen(buffer) + 1;
  char *file_contents = malloc_wrap(buffer_len);
  if (!file_contents) {
    free(buffer);
    return NULL;
  }
  memcpy(file_contents, buffer, buffer_len);
  free(buffer);

  struct line line;
  line.start = file_contents;
  line.end = strchr(line.start, '\n');

  bool node_complete;

  while (line.end)
  {
    line.len = line.end - line.start;
    char *tmp_line = malloc_wrap(line.len + 1);
    if (!tmp_line) {
      free(file_contents);
      return NULL;
    }

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

    if (*line_ptr == '\0' || *line_ptr == '#' || *line_ptr == '[')
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

      attr_ptr->str = strclone(line_ptr, 0);
      if (!attr_ptr->str)
      {
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
  }

  if (!root) {
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
