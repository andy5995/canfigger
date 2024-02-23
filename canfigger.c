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


static void
cleanup_1(char **line, FILE **fp)
{
  if (*line)
    free(*line);

  if (fclose(*fp) != 0)
    perror("canfigger->fclose:");

  return;
}


void
canfigger_free_attr(st_canfigger_attr_node **node, st_canfigger_node **key)
{
  while (*node)
    canfigger_get_next_attr(node, key);

  return;
}


void
canfigger_get_next_attr(st_canfigger_attr_node **attr_node,
                        st_canfigger_node **key)
{
  if (*attr_node)
  {
    if ((*attr_node)->str)
    {
      free((*attr_node)->str);
      (*attr_node)->str = NULL;
    }

    st_canfigger_attr_node *temp_node = (*attr_node)->next;
    free(*attr_node);
    *attr_node = temp_node;

    // Replace the pointer to the head attr_node
    (*key)->attr_node = *attr_node;
  }

  return;
}

void
canfigger_get_next_key(st_canfigger_list **node)
{
  if (*node)
  {
    canfigger_free_attr(&(*node)->attr_node, node);

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
add_attr_node(st_canfigger_attr_node **root,
              st_canfigger_attr_node **cur_node)
{
  st_canfigger_attr_node *tmp_node =
    malloc(sizeof(struct st_canfigger_attr_node));
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


st_canfigger_list *
canfigger_parse_file(const char *file, const int delimiter)
{
  err_strdup = 0;
  st_canfigger_node *root = NULL, *cur_node = NULL;

  FILE *fp = fopen(file, "r");
  if (!fp)
  {
    perror("canfigger:");
    return NULL;
  }

  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  // getline() malloc's the memory needed for line
  while ((read = getline(&line, &len, fp)) != -1)
  {
    if (!line)
    {
      fclose(fp);
      return NULL;
    }

    // fprintf(stderr, "Retrieved line of length %zu:\n", read);
    trim_whitespace(line);
    char *a = line;

    while (isspace(*a))
      a = erase_lead_char(*a, a);

    if (*a == '\0' || *a == '#')
      continue;

    int r = add_key_node(&root, &cur_node);
    if (r == -1)
    {
      canfigger_free(&root);
      cleanup_1(&line, &fp);
      return NULL;
    }

    // Get key
    cur_node->key = NULL;
    char *b = grab_str_segment(a, &cur_node->key, '=');

    if (err_strdup)
    {
      cleanup_1(&line, &fp);
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
      cleanup_1(&line, &fp);
      return NULL;
    }

    // Get attributes
    cur_node->attr_node = NULL;
    st_canfigger_attr_node *attr_root = NULL, *cur_attr_node = NULL;

    while (b)
    {
      r = add_attr_node(&attr_root, &cur_attr_node);
      if (r == -1)
      {
        canfigger_free_attr(&attr_root, &cur_node);
        canfigger_free(&root);
        cleanup_1(&line, &fp);
      }

      cur_attr_node->str = NULL;
      a = b;
      b = grab_str_segment(a, &cur_attr_node->str, delimiter);
      if (strlen(cur_attr_node->str) == 0)
        fputs("error\n", stderr);

      if (cur_attr_node->str == NULL)
        fputs("error\n", stderr);

      if (err_strdup)
      {
        cleanup_1(&line, &fp);
        return NULL;
      }

      cur_attr_node->next = NULL;
    }

    cur_node->attr_node = attr_root;
    cur_node->next = NULL;
  }

  if (!root)
  {
    cleanup_1(&line, &fp);
    return NULL;
  }

  cleanup_1(&line, &fp);
  return root;
}
