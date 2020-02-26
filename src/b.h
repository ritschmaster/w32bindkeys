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

#include <collectc/array.h>

#include "be.h"

#ifndef WBK_B_H
#define WBK_B_H

typedef struct wbk_b_s
{
	/**
	 * Array of wbk_be_t. Actually it is a Set.
	 */
	Array *comb;
} wbk_b_t;

extern wbk_b_t *
wbk_b_new();

extern int
wbk_b_free(wbk_b_t *b);

/**
 * @brief Add a binding element.
 * @param be Binding element to add. Contents will be copied
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
 * @return
 */
extern Array *
wbk_b_get_comb(const wbk_b_t *b);

#endif // WBK_B_H
