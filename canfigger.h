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

#pragma once

#ifndef CANFIGGER_VERSION
#define CANFIGGER_VERSION "0.2.0999"
#endif

#ifdef _MSC_VER
#define DEPRECATED __declspec(deprecated)
#elif defined(__GNUC__) || defined(__clang__)
#define DEPRECATED __attribute__((deprecated))
#else
#define DEPRECATED
#endif

// Member of st_canfigger_node
// @see canfigger_get_next_attr_list_node()
typedef struct st_canfigger_attr_node st_canfigger_attr_node;
struct st_canfigger_attr_node
{
  char *str;
  st_canfigger_attr_node *next;
};


// Node in the linked list returned by canfigger_parse_file()
typedef struct st_canfigger_node st_canfigger_node;
struct st_canfigger_node
{
  // Contains the string that precedes the '=' sign
  char *key;

  // Contains the string between the '=' sign and the delimiter
  char *value;

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
// @see canfigger_get_next_node()
//
// Each node is of type st_canfigger_node.
st_canfigger_list *canfigger_parse_file(const char *file,
                                        const int delimiter);


// Frees each member of the current attributes node, then frees the
// current attribute node and returns a pointer to the next one.
// Call this after retrieving each attribute
// set. NULL will be returned when the end is reached.
st_canfigger_attr_node *
canfigger_get_next_attr_list_node(st_canfigger_attr_node *attr_node);


// Frees each member of the current node, then frees the current node
// and returns a pointer to the next
// one. Call this after retrieving each key/value/attribute
// set. NULL will be returned when the end is reached.
st_canfigger_list *
canfigger_get_next_node(st_canfigger_list *list);


//
// Frees the list returned by canfigger_parse_file();
// The root node must be used when this is called
void canfigger_free(st_canfigger_node * node);


// Frees the attribute node (which may be a linked list of attributes);
// The root node must be used when this is called.
void canfigger_free_attr(st_canfigger_attr_node * node);
