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
 * @date 2020-02-20
 * @brief File contains the data finder class definition
 */

#ifndef WBK_DATAFINDER_H
#define WBK_DATAFINDER_H

#include <collectc/array.h>

typedef struct wbk_datafinder_s
{
	/**
	 * Array of char *
	 */
	Array *datadir_arr;

	char *execdir;

	/**
	 * Length of exec_dir. It includes the terminating byte.
	 */
	int execdir_len;
} wbk_datafinder_t;

extern wbk_datafinder_t *
wbk_datafinder_new(const char *datadir);

extern int
wbk_datafinder_free(wbk_datafinder_t *datafinder);

extern int
wbk_datafinder_add_datadir(wbk_datafinder_t *datafinder, const char *datadir);

/**
 * @param data_file The base name of a file to search for.
 * @return A new string containing the absolute path to data_file, free it
 *         yourself. If NULL, then the file could not be found.
 */
extern char *
wbk_datafinder_gen_path(const wbk_datafinder_t *datafinder, const char *data_file);

#endif // WBK_DATAFINDER_H
