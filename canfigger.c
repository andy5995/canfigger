/*
This file is part of canfigger<https://github.com/andy5995/canfigger>

Copyright (C) 2021-2022 Andy Alt (arch_stanton5995@proton.me)

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


void
canfigger_free(st_canfigger_node * node)
{
  if (node != NULL)
  {
    canfigger_free(node->next);
    free(node);
  }
  return;
}


void
canfigger_free_attr(st_canfigger_attr_node * node)
{
  if (node != NULL)
  {
    canfigger_free_attr(node->next);
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
  if (str == NULL)
    return;

  char *pos_0 = str;
  /* Advance pointer until NULL terminator is found */
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
grab_str_segment(char *a, char *dest, const int c)
{
  a = erase_lead_char(' ', a);
  char *b = strchr(a, c);
  if (b == NULL)
  {
    strcpy(dest, a);
    trim_whitespace(dest);
    return NULL;
  }

  char *dest_ptr = dest;
  while (a != b)
    *dest_ptr++ = *a++;

  *dest_ptr = '\0';
  trim_whitespace(dest);

  return b + 1;
}


st_canfigger_list *
canfigger_parse_file(const char *file, const int delimiter)
{
  static st_canfigger_node *root = NULL;
  st_canfigger_list *list = NULL;

  FILE *fp = fopen(file, "r");
  if (fp == NULL)
    return NULL;

  char line[__CFG_LEN_MAX_LINE];
  while (fgets(line, sizeof line, fp) != NULL)
  {
    trim_whitespace(line);
    char *a = line;

    while (isspace(*a))
      a = erase_lead_char(*a, a);

    if (*a == '\0' || *a == '#')
      continue;

    st_canfigger_node *tmp_node = malloc(sizeof(struct st_canfigger_node));
    if (tmp_node != NULL)
    {
      if (list != NULL)
        list->next = tmp_node;
      else
        root = tmp_node;

      st_canfigger_attr_node *attr_root = NULL;
      st_canfigger_attr_node *attr_list = NULL;

      *tmp_node->key = '\0';
      *tmp_node->value = '\0';

      char *b = grab_str_segment(a, tmp_node->key, '=');
      if (b != NULL)
      {
        a = b;
        b = grab_str_segment(a, tmp_node->value, delimiter);
      }
      do
      {
        st_canfigger_attr_node *cur_attr_node =
          malloc(sizeof(struct st_canfigger_attr_node));
        if (cur_attr_node == NULL)
        {
          if (attr_root != NULL)
            canfigger_free_attr(attr_root);

          if (root)
            canfigger_free(root);
          fclose(fp);
          return NULL;
        }

        if (attr_list != NULL)
          attr_list->next = cur_attr_node;
        else
          attr_root = cur_attr_node;

        *cur_attr_node->str = '\0';

        if (b != NULL)
        {
          a = b;
          b = grab_str_segment(a, cur_attr_node->str, delimiter);
        }
        attr_list = cur_attr_node;
        cur_attr_node->next = NULL;
      }
      while (b != NULL);

      tmp_node->attr_node = attr_root;
      tmp_node->next = NULL;

      list = tmp_node;
    }
    else
    {
      if (root != NULL)
        canfigger_free(root);

      fclose(fp);
      return NULL;
    }
  }

  int r = fclose(fp);
  if (r != 0)
  {
    return NULL;
  }

  list = root;
  return list;
}
