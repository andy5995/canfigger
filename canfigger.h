/**
 * @file
 * Part of canfigger (https://github.com/andy5995/canfigger).
 *
 * @brief Header for the canfigger configuration parser.
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
 * @brief Node in the list of attributes for a config item.
 *
 * @see canfigger_get_next_attr()
 */
typedef struct st_canfigger_attr_node {
    char *str; ///< The attribute string.
    struct st_canfigger_attr_node *next; ///< Next attribute node.
} st_canfigger_attr_node;

/**
 * @struct st_canfigger_node
 * @brief Node in linked list from canfigger_parse_file().
 *
 * Represents a key-value pair with optional attributes.
 */
typedef struct st_canfigger_node {
    char *key; ///< Key string (left of '=').
    char *value; ///< Value string (between '=' and delimiter).
    st_canfigger_attr_node *attr_node; ///< Attributes list.
    struct st_canfigger_node *next; ///< Next node in list.
} st_canfigger_node;

/**
 * @brief Alias for st_canfigger_node for readability.
 */
typedef st_canfigger_node st_canfigger_list;

/**
 * @brief Parses a config file into a linked list.
 *
 * Each node represents a key-value pair with optional attributes.
 * List must be freed with canfigger_free().
 *
 * @param file Path to the config file.
 * @param delimiter Delimiter character in the file.
 * @return Pointer to the first node in the list, or NULL on error.
 *
 * @see canfigger_free()
 */
st_canfigger_list *canfigger_parse_file(const char *file, const int delimiter);
/**
 *  \example tests/test_parse_file.c
 */

/**
 * @brief Frees memory for an attribute node and subsequent nodes.
 *
 * Call after retrieving each attribute set. Returns NULL at end.
 *
 * @param attr_node Current attribute node to free.
 * @return Pointer to next attribute node, or NULL.
 */
st_canfigger_attr_node *canfigger_get_next_attr(st_canfigger_attr_node *attr_node);

/**
 * @brief Frees memory for a config node and subsequent nodes.
 *
 * Call after processing each key-value-attributes set. Returns NULL at end.
 *
 * @param list Current node to free.
 * @return Pointer to next node, or NULL.
 */
st_canfigger_list *canfigger_get_next_key(st_canfigger_list *list);

/**
 * @brief Frees the entire list of configuration nodes.
 *
 * Used to free memory allocated by canfigger_parse_file().
 *
 * @param node Root node of the list to be freed.
 */
void canfigger_free(st_canfigger_node *node);

/**
 * @brief Frees the entire list of attribute nodes.
 *
 * Used to free memory for attribute nodes of a config node.
 *
 * @param node Root attribute node of the list to be freed.
 */
void canfigger_free_attr(st_canfigger_attr_node *node);
