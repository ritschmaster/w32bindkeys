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

#ifndef WBK_KBMAN_H
#define WBK_KBMAN_H

#include <windows.h>

#include "kc.h"
typedef struct wbk_kbman_s wbk_kbman_t;

struct wbk_kbman_s
{
  wbk_kbman_t *(*kbman_free)(wbk_kbman_t *kbman);
  int (*kbman_add)(wbk_kbman_t *kbman, wbk_kc_t *kc);
  wbk_kbman_t **(*kbman_split)(wbk_kbman_t *kbman, int nominator);
  int (*kbman_exec)(wbk_kbman_t *kbman, wbk_b_t *b);

	int kc_arr_len;
	wbk_kc_t **kc_arr;
};

/**
 */
extern wbk_kbman_t *
wbk_kbman_new();

extern wbk_kbman_t *
wbk_kbman_free(wbk_kbman_t *kbman);

/**
 * @param kb The key binding to add. The added key binding will be freed by the key binding manager
 */
extern int
wbk_kbman_add(wbk_kbman_t *kbman, wbk_kc_t *kc);

/**
 * Create an array of nominator new key board managers and divide the internal
 * key commands by nominator over those new key board managers. The key commands
 * are copied during this processes. The returned array and the returned key
 * board managers need to be freed by yourself!
 */
extern wbk_kbman_t **
wbk_kbman_split(wbk_kbman_t *kbman, int nominator);

/**
 * @brief Execute a key binding matching a combination
 * @return Non-0 if the combination was not found.
 */
extern int
wbk_kbman_exec(wbk_kbman_t *kbman, wbk_b_t *b);

#endif // WBK_KBMAN_H
