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
 * @brief File contains interpreter keyboard manager class definition
 */

#ifndef WBKI_KBMAN_H
#define WBKI_KBMAN_H

#include <collectc/array.h>
#include <windows.h>

#include "kc_sys.h"

typedef struct wbk_kbman_s
{
	/**
	 * Array of wbk_kc_sys *
	 */
	Array *kc_sys_arr;
} wbk_kbman_t;

/**
 */
extern wbk_kbman_t *
wbk_kbman_new();

extern wbk_kbman_t *
wbk_kbman_free(wbk_kbman_t *kbman);

/**
 * @brief Gets the key bindings of a key binding manager
 * @return Array of wbk_kc_sys *
 */
extern Array *
wbk_kbman_get_kb(wbk_kbman_t* kbman);

/**
 * @param kb The key binding to add. The added key binding will be freed by the key binding manager
 */
extern int
wbk_kbman_add(wbk_kbman_t *kbman, wbk_kc_sys_t *kc_sys);

/**
 * @brief Execute a key binding matching a combination
 * @return Non-0 if the combination was not found.
 */
extern int
wbk_kbman_exec(wbk_kbman_t *kbman, wbk_b_t *b);

/**
 * @brief Main loop, runs forever.
 */
extern int
wbk_kbman_start(wbk_kbman_t *kbman);

extern int
wbk_kbman_stop(wbk_kbman_t *kbman);

/**
 * @param c The result of GetAsyncKeyState (a virtual key code).
 * @return A virtual key code as modifier key
 */
extern wbk_mk_t
wbk_kbman_win32_to_mk(unsigned char c);

/**
 * @param c The result of GetAsyncKeyState (a virtual key code).
 * @return An actual character (e.g. 'a').
 */
extern char
wbk_kbman_win32_to_char(unsigned char c);

#endif // WBKI_KBMAN_H
