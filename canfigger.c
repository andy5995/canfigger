/*
This file is part of canfigger<https://github.com/andy5995/canfigger>

Copyright (C) 2021  Andy Alt (andy400-dev@yahoo.com)

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
canfigger_free (st_canfigger_node * node)
{
  if (node != NULL)
  {
    canfigger_free (node->next);
    free (node);
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
erase_lead_char (const int lc, char *haystack)
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
trim_whitespace (char *str)
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

  while (isspace (*str))
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
grab_str_segment (char *a, char *dest, const int c)
{
  char *b = strchr (a, c);
  if (b == NULL)
  {
    strcpy (dest, a);
    trim_whitespace (dest);
    return NULL;
  }

  char tmp_dest[__CFG_LEN_MAX_LINE];
  char *dest_ptr = tmp_dest;
  while (a != b)
    *dest_ptr++ = *a++;

  *dest_ptr = '\0';

  // reset the pointer back to the beginning of tmp_dest and erase any
  // leading spaces
  dest_ptr = erase_lead_char (' ', tmp_dest);

  trim_whitespace (dest_ptr);
  strcpy (dest, dest_ptr);

  return b + 1;
}


st_canfigger_list *
canfigger_parse_file (const char *file, const char delimiter)
{
  static st_canfigger_node *root = NULL;
  st_canfigger_list *list = NULL;

  FILE *fp = fopen (file, "r");
  if (fp == NULL)
    return NULL;

  char line[__CFG_LEN_MAX_LINE];
  while (fgets (line, sizeof line, fp) != NULL)
  {
    trim_whitespace (line);
    char *a = line;
    a = erase_lead_char (' ', a);
    a = erase_lead_char ('\t', a);
    switch (*a)
    {
    case '#':
      continue;
    case '\0':
      continue;
    }

    st_canfigger_node *tmp_node = malloc (sizeof (struct st_canfigger_node));
    if (tmp_node != NULL)
    {
      if (list != NULL)
        list->next = tmp_node;

      *tmp_node->key = '\0';
      *tmp_node->value = '\0';
      *tmp_node->attribute = '\0';

      char *b = grab_str_segment (a, tmp_node->key, '=');
      if (b != NULL)
      {
        a = b;
        b = grab_str_segment (a, tmp_node->value, delimiter);
        if (b != NULL)
        {
          a = b;
          a = erase_lead_char (' ', a);
          strcpy (tmp_node->attribute, a);
          trim_whitespace (tmp_node->attribute);
        }
      }

      tmp_node->next = NULL;
      list = tmp_node;

      if (root == NULL)
        root = list;
    }
    else
    {
      if (root != NULL)
        canfigger_free (root);

      fclose (fp);
      return NULL;
    }
  }

  int r = fclose (fp);
  if (r != 0)
  {
    return NULL;
  }

  list = root;
  return list;
}
