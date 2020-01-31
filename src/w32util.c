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

#include "util.h"

#include <collectc/array.h>
// #include <windows.h>

#include "kbman.h"

int
wbk_w32util_register_kb(wbk_kbman_t *kbman)
{
	ArrayIter kb_iter;
	ArrayIter b_iter;
	ArrayIter be_iter;
	wbk_kb_t *kb;
	wbk_b_t *b;
	wbk_be_t *be;

	array_iter_init(&kb_iter, wbk_kbman_get_kb(kbman));
	while (array_iter_next(&kb_iter, (void *) &kb) != CC_ITER_END) {
		array_iter_init(&be_iter, wbk_b_get_comb(wbk_kb_get_comb(kb)));
		while (array_iter_next(&be_iter, (void *) &be) != CC_ITER_END) {
			// TODO
		}
	}

	return 0;
}

int
wbk_w32util_exec_kb(wbk_kbman_t *kbman)
{
	return 0;
}

