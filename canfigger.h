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

/**
 * @struct st_canfigger_attr_node
 * @brief Represents an attribute node in a linked list.
 *
 * Each node contains a string (`str`) that represents the attribute itself,
 * and a pointer (`next`) to the next attribute node in the list, if any.
 */
typedef struct st_canfigger_attr_node
{
  char *str;                    ///< The attribute string.
  struct st_canfigger_attr_node *next;  ///< Pointer to the next attribute node.
} st_canfigger_attr_node;

/**
 * @struct st_canfigger_node
 * @brief Represents a node in the configuration linked list.
 *
 * Each node contains a key-value pair (`key` and `value`), a linked list of
 * attributes associated with the key (`attr_node`), and a pointer (`next`) to
 * the next configuration node in the list.
 */
typedef struct st_canfigger_node
{
  char *key;                    ///< Key string (left of '=').
  char *value;                  ///< Value string (between '=' and delimiter).
  st_canfigger_attr_node *attr_node;    ///< Linked list of attributes.
  struct st_canfigger_node *next;       ///< Pointer to the next configuration node.
} st_canfigger_node;

/**
 * @brief Alias for st_canfigger_node for readability and convenience.
 */
typedef st_canfigger_node st_canfigger_list;

/**
 * @brief Parses a configuration file into a linked list of key-value pairs.
 *
 * Parses the specified file, creating a linked list where each node represents
 * a key-value pair with optional attributes. The list and all associated
 * resources must be freed by the caller using canfigger_free().
 *
 * @param file The path to the configuration file.
 * @param delimiter The character used to separate attributes in the file.
 * @return A pointer to the first node in the created list, or NULL on failure.
 */
st_canfigger_list *canfigger_parse_file(const char *file,
                                        const int delimiter);
/**
 * \example example-01.c
 */

/**
 * @brief Advances to the next attribute node, freeing the current one.
 *
 * This function is used to traverse and free a list of attribute nodes. It
 * frees the current attribute node and advances the pointer to the next node
 * in the list.
 *
 * @param attr_node A pointer to the current attribute node pointer.
 */
void canfigger_get_next_attr(st_canfigger_attr_node ** attr_node,
                             st_canfigger_node ** key);

/**
 * @brief Advances to the next configuration node, freeing the current one.
 *
 * This function is used to traverse and free a list of configuration nodes. It
 * frees the current node, including its key, value, and attributes, and
 * advances the pointer to the next node in the list.
 *
 * @param list A pointer to the current configuration node pointer.
 */
void canfigger_get_next_key(st_canfigger_list ** list);

/**
 * @brief Frees the entire configuration linked list.
 *
 * Frees all nodes in the list, including their keys, values, and attributes.
 * This function should be used to release all resources allocated by
 * canfigger_parse_file().
 *
 * @param node A pointer to the pointer of the root node of the list.
 */
void canfigger_free(st_canfigger_node ** node);

/**
 * @brief Frees an entire list of attribute nodes.
 *
 * Frees all attribute nodes in the list, including their strings. This
 * function is typically used internally by canfigger_free() and
 * canfigger_get_next_key().
 *
 * @param node A pointer to the pointer of the root attribute node of the list.
 */
void canfigger_free_attr(st_canfigger_attr_node ** node,
                         st_canfigger_node ** key);
