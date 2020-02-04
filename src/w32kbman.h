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
 * @brief File contains w32kbman class definition
 */

#ifndef WBK_W32KBMAN_H
#define WBK_W32KBMAN_H

#include <collectc/array.h>
#include <windows.h>

#include "kbman.h"
#include "kb.h"

typedef struct wbk_w32_id_kb_s
{
	int id;
	wbk_kb_t *kb;
} wbk_w32_id_kb_t;

typedef struct wbk_w32kbman_s
{
	wbk_kbman_t *kbman;

	/**
	 * Array of wbk_w32_id_kb_t
	 */
	Array *id_kb_arr;
} wbk_w32kbman_t;

/**
 * @param kbman The passed object will be freed by the w32kbman!
 */
extern wbk_w32kbman_t *
wbk_w32kbman_new(wbk_kbman_t *kbman);

extern wbk_w32kbman_t *
wbk_w32kbman_free(wbk_w32kbman_t *kbman);

/**
 * @brief Register key bindings to Windows
 */
extern int
wbk_w32kbman_register_kb(wbk_w32kbman_t *kbman, HWND window_handler);

/**
 * @brief Exec key bindings
 */
extern int
wbk_w32kbman_exec_kb(wbk_w32kbman_t *kbman, int id);

#endif // WBK_W32KBMAN_H
