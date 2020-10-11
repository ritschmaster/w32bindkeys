/******************************************************************************
  This file is part of w32bindkeys.

  Copyright 2020 Richard Paul Baeck <richard.baeck@mailbox.org>

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
*******************************************************************************/

/**
 * @author Richard Bäck
 * @date 29 January 2020
 * @brief File contains utility functions
 */

#ifndef WBK_UTIL_H
#define WBK_UTIL_H

#include <collectc/array.h>
#include <stdio.h>

/**
 * @return A new string. Free it by yourself.
 */
extern char *
wbk_intarr_to_str(Array *array);

/**
 * @brief Produces an absolute path to the home directory
 * @return A new string. Free it by yourself.
 */
extern char *
wbk_get_path_of_home(void);

/**
 * @brief Produces an absolute path from the home directory
 * @return A new string. Free it by yourself.
 */
extern char *
wbk_path_from_home(const char *relative_path);

extern int
wbk_write_file(const char *src_path, FILE *dest);

#endif // WBK_UTIL_H
