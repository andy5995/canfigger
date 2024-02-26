/**
 * @file canfigger.h
 * @brief Header file for the Canfigger configuration parser.
 *
 * Canfigger is a lightweight C language library designed to parse configuration
 * files. It provides functionality to read them and represent their contents as
 * a linked list of key-value pairs, along with associated attributes for each
 * pair.
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

/**
 * \example example.c
 */

/**
 * @var canfigger_attr
 * @brief Global variable used to access the current attribute being processed.
 */
extern char *canfigger_attr;

/**
 * @struct attributes
 * @brief Structure to hold attribute details of a configuration key.
 *
 * @var attributes::str Original string containing all attributes.
 * @var attributes::current Pointer to the current attribute being processed.
 * @var attributes::ptr Pointer used for iterating through attributes.
 */
struct attributes
{
  char *str;
  char *current;
  char *ptr;
};

/**
 * @struct Canfigger
 * @brief Structure to represent a key-value pair with attributes in the configuration.
 *
 * @var Canfigger::key The key in a key-value pair.
 * @var Canfigger::value The value associated with the key.
 * @var Canfigger::attributes Attributes associated with the key.
 * @var Canfigger::next Pointer to the next key-value pair in the list.
 */
struct Canfigger
{
  char *key;
  char *value;
  struct attributes *attributes;
  struct Canfigger *next;
};

/**
 * @brief Parses a configuration file and creates a linked list of key-value pairs.
 *
 * @param file Path to the configuration file to parse.
 * @param delimiter The character used to delimit key-value pairs in the file.
 * @return Pointer to the head of the linked list of configuration entries.
 */
struct Canfigger *canfigger_parse_file(const char *file, const int delimiter);

/**
 * @brief Frees the current key node and advances to the next node in the list.
 *
 * @param list Double pointer to the current node in the linked list.
 */
void canfigger_free_current_key_node_advance(struct Canfigger **list);

/**
 * @brief Frees the current attribute string and advances to the next attribute.
 *
 * @param attributes Pointer to the attributes structure of the current node.
 */
void canfigger_free_current_attr_str_advance(struct attributes *attributes);
