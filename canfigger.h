/*
This file is part of canfigger<https://github.com/andy5995/canfigger>

Copyright (C) 2021-2022 Andy Alt (andy400-dev@yahoo.com)

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

#pragma once

#ifndef CANFIGGER_VERSION
#define CANFIGGER_VERSION "0.2.0"
#endif

// The max length of a line in a configuration file; a longer line will
// get truncated when fgets() is called to read the file.
#define __CFG_LEN_MAX_LINE (512 + 1)

// Member of st_canfigger_node
// @see canfigger_free_attr()
typedef struct st_canfigger_attr_node st_canfigger_attr_node;
struct st_canfigger_attr_node
{
  char str[__CFG_LEN_MAX_LINE];
  st_canfigger_attr_node* next;
};

// Node in the linked list returned by canfigger_parse_file()
typedef struct st_canfigger_node st_canfigger_node;
struct st_canfigger_node
{
  // Contains the string that precedes the '=' sign
  char key[__CFG_LEN_MAX_LINE];

  // Contains the string between the '=' sign and the delimiter
  char value[__CFG_LEN_MAX_LINE];

  // Linked list of attributes
  st_canfigger_attr_node *attr_node;

  // A pointer to the next node in the list
  st_canfigger_node *next;
};

// Can be used interchangeably for code readability and developer preference
typedef st_canfigger_node st_canfigger_list;


//
// Opens a config file and returns a memory-allocated linked list
// that must be freed later
// @see canfigger_free()
//
// Each node is of type st_canfigger_node.
st_canfigger_list *canfigger_parse_file (const char *file,
                                         const char delimiter);

//
// Frees the list returned by canfigger_parse_file();
// The root node must be used when this is called
void canfigger_free (st_canfigger_node * node);


// Frees the attribute node (which may be a linked list of attributes);
// The root node must be used when this is called.
void
canfigger_free_attr (st_canfigger_attr_node * node);
