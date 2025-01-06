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
**/
/* This file is part of canfigger<https://github.com/andy5995/canfigger>

MIT License

Copyright (c) 2024 Andy Alt(arch_stanton5995@proton.me)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \example example.c
 */

/**
 * @struct attributes
 * @brief Structure to hold attribute details of a configuration key.
 *
 * @var attributes::str Original string containing all attributes.
 * @var attributes::current Pointer to the current attribute being processed.
 * @var attributes::iter_ptr Pointer used for iterating through attributes.
 */
struct attributes
{
  char *str;
  char *current;
  char *iter_ptr;
};

/**
 * @struct Canfigger
 * @brief Structure to represent a key-value pair with attributes in the configuration.
 *
 * @var Canfigger::key The key in a key-value pair.
 * @var Canfigger::value The value associated with the key.
 * @var Canfigger::attributes pointer to an attributes struct associated with the key.
 * @var Canfigger::next Pointer to the next node in the list.
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
 * @param delimiter The character used to delimit the attributes following 'value'.
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
 * @param attr Current attribute that will get reassigned after the function call.
 */
void canfigger_free_current_attr_str_advance(struct attributes *attributes, char **attr);

#ifdef __cplusplus
}
#endif
