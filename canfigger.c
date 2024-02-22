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
  free(*line);
  fclose(*fp);
  return;
}

st_canfigger_attr_node *
canfigger_get_next_attr_list_node(st_canfigger_attr_node *attr_node)
{
  if (attr_node)
  {
    if (attr_node->str)
    {
      free(attr_node->str);
      attr_node->str = NULL; // Maybe not necessary
    }

    st_canfigger_attr_node *new_attr_node = attr_node->next;
    free(attr_node);
    attr_node = NULL; // Maybe not necessary
    return new_attr_node;
  }
  return NULL;
}

st_canfigger_list *
canfigger_get_next_node(st_canfigger_list *list)
{
  st_canfigger_node *node = list;
  if (node)
  {
    if (node->attr_node)
      canfigger_free_attr(node->attr_node);
    if (node->value)
      free(node->value);
    free(node->key);
    st_canfigger_node *new_node = node->next;
    free(node);
    return new_node;
  }
  return NULL;
}


void
canfigger_free(st_canfigger_node *node)
{
  if (node)
  {
    canfigger_free(node->next);
    free(node->key);
    free(node->value);
    free(node);
  }
  return;
}


void
canfigger_free_attr(st_canfigger_attr_node *node)
{
  if (node)
  {
    canfigger_free_attr(node->next);
    if (node->str)
      free(node->str);
    free(node);
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
  free(*dest);
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


st_canfigger_list *
canfigger_parse_file(const char *file, const int delimiter)
{
  err_strdup = 0;
  st_canfigger_node *root = NULL;
  st_canfigger_list *list = NULL;

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
    static const char *empty_str = "";

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

    st_canfigger_node *tmp_node = malloc(sizeof(struct st_canfigger_node));
    if (tmp_node)
    {
      if (list)
        list->next = tmp_node;
      else
        root = tmp_node;

      st_canfigger_attr_node *attr_root = NULL;
      st_canfigger_attr_node *attr_list = NULL;

      tmp_node->key = strdup(empty_str);
      if (!tmp_node->key)
      {
        cleanup_1(&line, &fp);
        return NULL;
      }

      char *b = grab_str_segment(a, &tmp_node->key, '=');
      if (err_strdup)
      {
        cleanup_1(&line, &fp);
        return NULL;
      }
      // fprintf(stderr, "key: '%s'\n", tmp_node->key);

      tmp_node->value = strdup(empty_str);
      if (!tmp_node->value)
      {
        cleanup_1(&line, &fp);
        return NULL;
      }

      if (b)
      {
        a = b;
        b = grab_str_segment(a, &tmp_node->value, delimiter);
        if (err_strdup)
        {
          cleanup_1(&line, &fp);
          return NULL;
        }
      }
      do
      {
        st_canfigger_attr_node *cur_attr_node =
          malloc(sizeof(struct st_canfigger_attr_node));
        if (cur_attr_node == NULL)
        {
          if (attr_root)
            canfigger_free_attr(attr_root);

          if (root)
            canfigger_free(root);

          cleanup_1(&line, &fp);
          return NULL;
        }

        if (attr_list)
          attr_list->next = cur_attr_node;
        else
          attr_root = cur_attr_node;

        cur_attr_node->str = strdup(empty_str);
        if (!cur_attr_node->str)
        {
          cleanup_1(&line, &fp);
          return NULL;
        }
        if (b)
        {
          a = b;
          b = grab_str_segment(a, &cur_attr_node->str, delimiter);
          if (err_strdup)
          {
            cleanup_1(&line, &fp);
            return NULL;
          }
        }
        attr_list = cur_attr_node;
        cur_attr_node->next = NULL;
      }
      while (b);

      tmp_node->attr_node = attr_root;
      tmp_node->next = NULL;

      list = tmp_node;
    }
    else
    {
      if (root)
        canfigger_free(root);

      cleanup_1(&line, &fp);
      return NULL;
    }
  }

  if (line)
    free(line);

  if (fclose(fp) != 0)
    perror("canfigger:");

  list = root;
  return list;
}
