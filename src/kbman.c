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

#include "kbman.h"

#include <stdlib.h>

#include "logger.h"

static wbk_logger_t logger =  { "kbman" };

wbk_kbman_t *
wbk_kbman_new(void)
{
	wbk_kbman_t *kbman;

	kbman = NULL;
	kbman = malloc(sizeof(wbk_kbman_t));

	array_new(&(kbman->kb));

	return kbman;
}

int
wbk_kbman_free(wbk_kbman_t *kbman)
{
	array_destroy_cb(kbman->kb, free); // TODO wbk_kb_free
	kbman->kb = NULL;
	free(kbman);
	return 0;
}

Array *
wbk_kbman_get_kb(wbk_kbman_t* kbman)
{
	return kbman->kb;
}

int
wbk_kbman_add(wbk_kbman_t *kbman, wbk_kb_t *kb)
{
	array_add(kbman->kb, kb);
}

int
wbk_kbman_exec(wbk_kbman_t *kbman, wbk_b_t *b)
{
	int ret;
	char found;
	ArrayIter kb_iter;
	wbk_kb_t *kb;

	ret = -1;

	found = 0;
	array_iter_init(&kb_iter, wbk_kbman_get_kb(kbman));
	while (!found && array_iter_next(&kb_iter, (void *) &kb) != CC_ITER_END) {
		if (wbk_b_compare(wbk_kb_get_binding(kb), b) == 0) {
			found = 1;
		}
	}

	if (found) {
		wbk_kb_exec(kb);
		ret = 0;
	}

	return ret;
}
