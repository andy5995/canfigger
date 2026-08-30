/**
 * @file canfigger.h
 * @brief Public API for the Canfigger configuration file parser.
 *
 * Canfigger parses plain-text configuration files into a singly-linked list of
 * key-value nodes. Each node may also carry a list of attributes — extra
 * comma-separated (or user-specified delimiter) fields that follow the value on
 * the same line.
 *
 * **File format** (one entry per line):
 * @code
 *   key = value
 *   key = value, attr1, attr2
 *   key = list, item1, item2, item3
 *   # comment lines are ignored
 *   [section headers are ignored]
 * @endcode
 *
 * The `=` sign separates the key from the value. The delimiter character
 * (passed to canfigger_parse_file()) separates the value from the first
 * attribute, and each subsequent attribute from the next.  A UTF-8 BOM at the
 * start of the file is silently skipped.
 *
 * **Typical usage:**
 * @code
 *   struct Canfigger *list = canfigger_parse_file("app.conf", ',');
 *   while (list) {
 *     // use list->key and list->value
 *     canfigger_free_current_key_node_advance(&list);
 *   }
 * @endcode
 *
 * Part of canfigger (https://github.com/andy5995/canfigger).
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

#include "canfigger_version.h"

/**
 * @brief Compile-time version check.
 *
 * Evaluates to a non-zero value if the canfigger headers are at least the
 * given major and minor version.  Useful for conditional compilation when
 * a feature was added in a known release:
 *
 * @code
 *   #if CANFIGGER_CHECK_VERSION(0, 4)
 *     // use API added in 0.4
 *   #endif
 * @endcode
 *
 * @param maj Required major version.
 * @param min Required minor version.
 */
#define CANFIGGER_CHECK_VERSION(maj, min) \
  (CANFIGGER_VERSION_MAJOR > (maj) || \
   (CANFIGGER_VERSION_MAJOR == (maj) && CANFIGGER_VERSION_MINOR >= (min)))

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @struct attributes
 * @brief Internal iteration state for a node's attribute list.
 *
 * This struct is allocated and owned by the library. Callers should not
 * read or modify its fields directly; use
 * canfigger_free_current_attr_str_advance() to iterate.
 *
 * @var attributes::str    Heap-allocated copy of the raw attribute string.
 * @var attributes::current Last attribute string returned to the caller;
 *                          freed on the next call to
 *                          canfigger_free_current_attr_str_advance().
 * @var attributes::iter_ptr Read position within @p str; advanced by each
 *                           call to canfigger_free_current_attr_str_advance().
 */
  struct attributes
  {
    char *str;
    char *current;
    char *iter_ptr;
  };

/**
 * @struct Canfigger
 * @brief A single node in the parsed configuration linked list.
 *
 * Each node represents one key-value entry from the configuration file.
 * Nodes are heap-allocated by canfigger_parse_file() and must be freed
 * with canfigger_free_current_key_node_advance() or canfigger_free_list().
 *
 * @var Canfigger::key        The key string (never NULL).
 * @var Canfigger::value      The value string, or NULL if no @c = sign was
 *                            present on the line.
 * @var Canfigger::attributes Attribute list, or NULL if the line had no
 *                            attributes following the value.
 * @var Canfigger::next       Next node, or NULL at end of list.
 */
  struct Canfigger
  {
    char *key;
    char *value;
    struct attributes *attributes;
    struct Canfigger *next;
  };

/**
 * @brief Parse a configuration file into a linked list of key-value nodes.
 *
 * Reads @p file, strips a leading UTF-8 BOM if present, and returns a
 * singly-linked list where each node holds one key-value entry.  Lines
 * beginning with @c # or @c [ and blank lines are ignored.
 *
 * The @p delimiter character separates the value from the first attribute
 * and each subsequent attribute from the next.  Pass a character that does
 * not appear in your values if you do not use attributes (e.g. @c ',').
 *
 * The caller owns the returned list and must free it with
 * canfigger_free_current_key_node_advance() (while iterating) or
 * canfigger_free_list() (to discard the whole list at once).
 *
 * @param file      Path to the configuration file.
 * @param delimiter Character that separates the value from attributes on a line.
 * @return Head of the linked list, or NULL if the file cannot be opened,
 *         is empty, or a memory allocation failure occurs.
 */
  struct Canfigger *canfigger_parse_file(const char *file,
                                         const int delimiter);

/**
 * @brief Free the current node and advance the list pointer to the next node.
 *
 * Releases all memory owned by @c *node (key, value, and any attributes),
 * then sets @c *node to the next node in the list.  Call this at the end of
 * each loop iteration when walking the list:
 *
 * @code
 *   while (list)
 *     canfigger_free_current_key_node_advance(&list);
 * @endcode
 *
 * @param node Double pointer to the current node; updated to point to the
 *             next node (or NULL at end of list) before returning.
 */
  void canfigger_free_current_key_node_advance(struct Canfigger **node);

/**
 * @brief Free the current attribute string and advance to the next attribute.
 *
 * On the first call for a given node, @c *attr must be NULL; the function
 * loads the first attribute into @c *attr.  On each subsequent call it frees
 * the previous attribute string and loads the next.  Sets @c *attr to NULL
 * when no more attributes remain, or if @p attributes is NULL.
 *
 * Typical usage:
 * @code
 *   char *attr = NULL;
 *   canfigger_free_current_attr_str_advance(node->attributes, &attr);
 *   while (attr) {
 *     // use attr
 *     canfigger_free_current_attr_str_advance(node->attributes, &attr);
 *   }
 * @endcode
 *
 * @param attributes Pointer to the attributes structure of the current node
 *                   (may be NULL, in which case @c *attr is set to NULL).
 * @param attr       Output parameter; set to the next attribute string on
 *                   success, or NULL when the list is exhausted.
 */
  void canfigger_free_current_attr_str_advance(struct attributes *attributes,
                                               char **attr);

/**
 * @brief Free all remaining nodes in the list.
 *
 * Equivalent to calling canfigger_free_current_key_node_advance() in a loop
 * until the list is empty.  Use this for early-exit cleanup when you need to
 * discard a partially-iterated list.
 *
 * @param node Double pointer to the current (or head) node; set to NULL
 *             on return.
 */
  void canfigger_free_list(struct Canfigger **node);

/**
 * @brief Return the platform config directory for an application.
 *
 * On Unix, honours @c $XDG_CONFIG_HOME if set; otherwise uses
 * @c $HOME/.config/appname.  On Windows, uses @c %APPDATA%\\appname.
 *
 * The returned string is heap-allocated; the caller must free it.
 *
 * @param appname Application name appended as a subdirectory.
 * @return Malloc'd path string, or NULL on failure or if @p appname is
 *         NULL/empty.
 *
 * @snippet examples/canfigger_config_dir.c canfigger_config_dir
 */
  char *canfigger_config_dir(const char *appname);

/**
 * @brief Return the path to a config file in the platform base config directory.
 *
 * Joins the base config directory with @p filename, without inserting an
 * application-name subdirectory.  On Unix, honours @c $XDG_CONFIG_HOME if
 * set; otherwise uses @c $HOME/.config/filename.  On Windows, uses
 * @c %APPDATA%\\filename.
 *
 * Use this when the config file lives directly under the config root rather
 * than in a per-application subdirectory (e.g. @c $HOME/.config/apprc rather
 * than @c $HOME/.config/app/apprc).
 *
 * The returned string is heap-allocated; the caller must free it.
 *
 * @param filename Config file name (or relative sub-path) to append.
 * @return Malloc'd path string, or NULL on failure or if @p filename is
 *         NULL or empty.
 *
 * @snippet examples/canfigger_config_file.c canfigger_config_file
 */
  char *canfigger_config_file(const char *filename);

/**
 * @brief Return the platform data directory for an application.
 *
 * Intended for user-generated data (saves, state, cache) — not bundled
 * application assets.  On Unix, honours @c $XDG_DATA_HOME if set; otherwise
 * uses @c $HOME/.local/share/appname.  On Windows, uses
 * @c %LOCALAPPDATA%\\appname.
 *
 * The returned string is heap-allocated; the caller must free it.
 *
 * @param appname Application name appended as a subdirectory.
 * @return Malloc'd path string, or NULL on failure or if @p appname is
 *         NULL/empty.
 */
  char *canfigger_data_dir(const char *appname);

/**
 * @brief Return the platform cache directory for an application.
 *
 * On Unix, honours @c $XDG_CACHE_HOME if set; otherwise uses
 * @c $HOME/.cache/appname.  On Windows, uses @c %LOCALAPPDATA%\\appname.
 *
 * The returned string is heap-allocated; the caller must free it.
 *
 * @param appname Application name appended as a subdirectory.
 * @return Malloc'd path string, or NULL on failure or if @p appname is
 *         NULL/empty.
 *
 * @since 0.3.2
 */
  char *canfigger_cache_dir(const char *appname);

/**
 * @brief Return the platform state directory for an application.
 *
 * For data that should persist between runs but is not configuration and is
 * not worth backing up - logs, history, recently-used lists, and similar.
 * On Unix, honours @c $XDG_STATE_HOME if set; otherwise uses
 * @c $HOME/.local/state/appname.  On Windows, uses @c %LOCALAPPDATA%\\appname.
 *
 * The returned string is heap-allocated; the caller must free it.
 *
 * @param appname Application name appended as a subdirectory.
 * @return Malloc'd path string, or NULL on failure or if @p appname is
 *         NULL/empty.
 *
 * @since 0.3.3
 */
  char *canfigger_state_dir(const char *appname);

/**
 * @brief Return the runtime directory for an application, or NULL.
 *
 * For short-lived per-session files: sockets, lock and PID files, and secrets
 * that should not outlive the login session.
 *
 * On Unix this is @c $XDG_RUNTIME_DIR.  Unlike the other directory helpers
 * there is deliberately no @c $HOME fallback, because the specification
 * attaches requirements to this directory that an arbitrary fallback would not
 * meet: it must exist, be owned by the calling user, and have mode 0700.  Those
 * conditions are checked, and NULL is returned if any of them fails or the
 * variable is unset - a common case in cron, container and other sessions that
 * never set one.
 *
 * On Windows there is no equivalent concept and NULL is always returned.  A
 * path under @c %LOCALAPPDATA% would persist across sessions, so returning one
 * would misrepresent what the caller asked for.
 *
 * A NULL return is therefore an ordinary outcome, not an error: choose a
 * fallback appropriate to the data, such as canfigger_cache_dir() for anything
 * that can be regenerated or safely lost.
 *
 * The returned string is heap-allocated; the caller must free it.
 *
 * @param appname Application name appended as a subdirectory.
 * @return Malloc'd path string, or NULL if no usable runtime directory exists
 *         or @p appname is NULL/empty.
 *
 * @since 0.3.3
 */
  char *canfigger_runtime_dir(const char *appname);

/**
 * @brief Return the system config search path, most important first.
 *
 * The directories in @c $XDG_CONFIG_DIRS, defaulting to @c /etc/xdg when it is
 * unset or empty.  These are searched @e after canfigger_config_dir(), which
 * always takes precedence.  Entries that are relative or empty are skipped, as
 * the specification requires.
 *
 * On Windows there is no equivalent and NULL is always returned.
 *
 * @return Malloc'd NULL-terminated array of malloc'd strings, to be released
 *         with canfigger_free_dirs(), or NULL on Windows or allocation
 *         failure.
 *
 * @since 0.3.3
 */
  char **canfigger_config_dirs(void);

/**
 * @brief Return the system data search path, most important first.
 *
 * The directories in @c $XDG_DATA_DIRS, defaulting to
 * @c /usr/local/share:/usr/share when it is unset or empty.  These are searched
 * @e after canfigger_data_dir(), which always takes precedence.  Entries that
 * are relative or empty are skipped, as the specification requires.
 *
 * On Windows there is no equivalent and NULL is always returned.
 *
 * @return Malloc'd NULL-terminated array of malloc'd strings, to be released
 *         with canfigger_free_dirs(), or NULL on Windows or allocation
 *         failure.
 *
 * @since 0.3.3
 */
  char **canfigger_data_dirs(void);

/**
 * @brief Free an array returned by canfigger_config_dirs() or
 *        canfigger_data_dirs().
 *
 * Frees each string and then the array itself.  Passing NULL is a no-op.
 *
 * @param dirs Array to free.
 *
 * @since 0.3.3
 */
  void canfigger_free_dirs(char **dirs);

/**
 * @brief Join a directory path and a filename with the platform separator.
 *
 * A separator is inserted between @p dir and @p file unless @p dir already
 * ends with @c / or @c \\.
 *
 * The returned string is heap-allocated; the caller must free it.
 *
 * @param dir  Directory portion of the path.
 * @param file Filename (or relative sub-path) to append.
 * @return Malloc'd joined path string, or NULL if either argument is NULL or
 *         empty, or on allocation failure.
 *
 * @snippet examples/canfigger_path_join.c canfigger_path_join
 */
  char *canfigger_path_join(const char *dir, const char *file);

#ifdef __cplusplus
}
#endif
