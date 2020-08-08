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
 * @date 2020-02-26
 * @brief File contains the binding class definition
 */

#include "be.h"

#ifndef WBK_B_H
#define WBK_B_H

#define WBK_B_MODIFER_MAP_LEN 30
#define WBK_B_KEY_MAP_LEN 256

typedef struct wbk_b_s
{
	wbk_mk_t modifier_map[WBK_B_MODIFER_MAP_LEN];

	char key_map[WBK_B_KEY_MAP_LEN];
} wbk_b_t;

/**
 * Creates a new binding
 */
extern wbk_b_t *
wbk_b_new();

/**
 * Clones an existing binding.
 */
extern wbk_b_t *
wbk_b_clone(const wbk_b_t *other);

extern int
wbk_b_free(wbk_b_t *b);

/**
 * @brief Removes all binding elements from the binding.
 */
extern int
wbk_b_reset(wbk_b_t *b);

/**
 * @brief Add a binding element. The binding will only be added if was not already added.
 * @param be Binding element to add.
 * @return 0 if the element was added. Non-0 otherwise.
 */
extern int
wbk_b_add(wbk_b_t *b, const wbk_be_t *be);

/**
 * @param be Binding element to remove.
 * @return 0 if the element was removed. Non-0 otherwise.
 */
extern int
wbk_b_remove(wbk_b_t *b, const wbk_be_t *be);

/**
 * @param be Binding element to check.
 * @return Non-0 if the binding element is within the binding. 0 otherwise.
 */
extern int
wbk_b_contains(wbk_b_t *b, const wbk_be_t *be);

/**
 * @param b
 * @param other
 * @return If b < other, then < 0. If b > other, then > 0. If b = other, then 0.
 */
extern int
wbk_b_compare(const wbk_b_t *b, const wbk_b_t *other);

/**
 * @return A new string containing the binding in a human readable form. Free it
 * by yourself!
 */
extern char *
wbk_b_to_str(const wbk_b_t *b);

#endif // WBK_B_H
