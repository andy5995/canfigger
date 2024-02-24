/**
 * @file canfigger.h
 * @brief Header file for the Canfigger configuration parser.
 *
 * This library provides functionality to parse configuration files, creating
 * and managing a linked list representation of keys, values, and attributes
 * found within the file. It supports operations for parsing, traversing,
 * and freeing the created lists.
 *
 * Part of canfigger (https://github.com/andy5995/canfigger).
 *
 * Copyright (C) 2021-2024 Andy Alt
 * (arch_stanton5995@proton.me)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#ifndef CANFIGGER_VERSION
#define CANFIGGER_VERSION "0.2.0999"
#endif

extern char *canfigger_attr;

struct attributes
{
  char *str;
  char *current;
  char *ptr;
};

typedef struct st_canfigger_node
{
  char *key;                    ///< Key string (left of '=').
  char *value;
  struct attributes *attributes;
  struct st_canfigger_node *next;       ///< Pointer to the next configuration node.
} st_canfigger_node;

typedef st_canfigger_node st_canfigger_list;


st_canfigger_list *canfigger_parse_file(const char *file,
                                        const int delimiter);
/**
 * \example example-01.c
 */

void canfigger_get_next_key(st_canfigger_list ** list);

void canfigger_get_next_attr(struct attributes *attributes);

void canfigger_init_attrs(struct attributes *attributes);

void canfigger_free(st_canfigger_node ** node);
