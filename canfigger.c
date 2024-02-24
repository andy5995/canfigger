/*
This file is part of canfigger<https://github.com/andy5995/canfigger>

Copyright (C) 2021-2024 Andy Alt (arch_stanton5995@proton.me)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>

#include "canfigger.h"

static int err_strdup = 0;
static int canfigger_delimiter = 0;
char *canfigger_attr = NULL;

static char *grab_str_segment(char *a, char **dest, const int c);

struct line
{
  char *line;
  char *start;
  char *end;
};

static void
cleanup_1(char **buf)
{
  if (*buf)
    free(*buf);

  return;
}


void
canfigger_get_next_attr(struct attributes *attributes)
{
  if (!attributes)
  {
    canfigger_attr = NULL;
    return;
  }

  if (attributes->current && attributes->ptr)
    free(attributes->current);

  if (!attributes->ptr)
  {
    free(attributes->current);
    attributes->current = NULL;
    canfigger_attr = NULL;
    return;
  }

  attributes->ptr = grab_str_segment(attributes->ptr,
                                     &attributes->current,
                                     canfigger_delimiter);

  canfigger_attr = attributes->current;

  return;
}

void
canfigger_init_attrs(struct attributes *attributes)
{
  canfigger_get_next_attr(attributes);
  return;
}

void
canfigger_get_next_key(st_canfigger_list **node)
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

    st_canfigger_node *temp_node = (*node)->next;
    free(*node);
    *node = temp_node;
  }

  return;
}


void
canfigger_free(st_canfigger_node **node)
{
  if (*node)
  {
    while (*node)
      canfigger_get_next_key(node);
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


/*!
 * Removes trailing white space from a string (including newlines, formfeeds,
 * tabs, etc
 * @param[out] str The string to be altered
 * @return void
 */
static void
trim_whitespace(char *str)
{
  if (!str)
    return;

  char *pos_0 = str;
  /* Advance pointer until NULL terminator is found */
  while (*str)
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
    *dest = strdup(a);
    if (!*dest)
      err_strdup = 1;
    return b;                   // NULL
  }

  *dest = strndup(a, b - a);
  if (!*dest)
  {
    if (!*dest)
      err_strdup = 1;
    return NULL;
  }
  trim_whitespace(*dest);
  return b + 1;
}


static int
add_key_node(st_canfigger_node **root, st_canfigger_node **cur_node)
{
  st_canfigger_node *tmp_node = malloc(sizeof(struct st_canfigger_node));
  if (!tmp_node)
  {
    perror("canfigger->malloc:");
    return -1;
  }

  if (*root)
    (*cur_node)->next = tmp_node;
  else
    *root = tmp_node;

  *cur_node = tmp_node;

  return 0;
}


static void
free_cur_line_and_advance(struct line *line)
{
  free(line->line);
  line->line = NULL;
  line->start = line->end;
  line->end = grab_str_segment(line->start, &line->line, '\n');

  return;
}


static char *
read_entire_file(const char *filename)
{
  FILE *file = fopen(filename, "rb");
  if (!file)
  {
    perror("canfigger->fopen");
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *buffer = malloc(file_size + 1);
  if (!buffer)
  {
    perror("canfigger->malloc");
    fclose(file);
    return NULL;
  }

  fread(buffer, 1, file_size, file);
  buffer[file_size] = '\0';

  fclose(file);
  return buffer;
}


st_canfigger_list *
canfigger_parse_file(const char *file, const int delimiter)
{
  err_strdup = 0;
  canfigger_delimiter = delimiter;
  st_canfigger_node *root = NULL, *cur_node = NULL;
  struct line line;

  char *file_contents = read_entire_file(file);
  if (file_contents == NULL)
    return NULL;

  line.start = file_contents;
  line.line = NULL;
  line.end = grab_str_segment(line.start, &line.line, '\n');

  while (line.end)
  {
    char *a = line.line;

    while (isspace(*a))
      a = erase_lead_char(*a, a);

    if (*a == '\0' || *a == '#')
    {
      free_cur_line_and_advance(&line);
      continue;
    }

    int r = add_key_node(&root, &cur_node);
    if (r == -1)
    {
      canfigger_free(&root);
      cleanup_1(&file_contents);
      return NULL;
    }

    // Get key
    cur_node->key = NULL;
    char *b = grab_str_segment(a, &cur_node->key, '=');

    if (err_strdup)
    {
      cleanup_1(&file_contents);
      return NULL;
    }

    // Get value
    cur_node->value = NULL;

    if (b)
    {
      a = b;
      b = grab_str_segment(a, &cur_node->value, delimiter);
    }

    if (err_strdup)
    {
      cleanup_1(&file_contents);
      return NULL;
    }

    // Handle attributes
    if (b)
    {
      cur_node->attributes = malloc(sizeof(struct attributes));
      if (!cur_node->attributes)
      {
        perror("canfigger->malloc:");
        free_cur_line_and_advance(&line);
        return NULL;
      }

      struct attributes *attr_ptr = cur_node->attributes;
      attr_ptr->current = NULL;
      attr_ptr->str = strdup(b);
      if (!attr_ptr->str)
      {
      }
      // Handle error
      attr_ptr->ptr = attr_ptr->str;
    }
    else
      cur_node->attributes = NULL;

    cur_node->next = NULL;
    free_cur_line_and_advance(&line);
  }

  if (!root)
  {
    cleanup_1(&file_contents);
    free(line.line);
    return NULL;
  }

  cleanup_1(&file_contents);
  free(line.line);
  return root;
}
