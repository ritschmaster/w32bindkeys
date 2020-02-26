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

#include "ikbman.h"

#include <collectc/array.h>
#include <windows.h>

#include "logger.h"
#include "kbman.h"

static wbk_logger_t logger =  { "ibkman" };

wbki_kbman_t *
wbki_kbman_new()
{
	wbki_kbman_t *new;

	new = NULL;
	new = malloc(sizeof(wbki_kbman_t));

	array_new(&(new->kc_sys));

	return new;

}

wbki_kbman_t *
wbki_kbman_free(wbki_kbman_t *kbman)
{
	array_destroy_cb(kbman->kc_sys, free); // TODO wbk_kb_sys_free
	kbman->kc_sys = NULL;
	free(kbman);
}

Array *
wbki_kbman_get_kb(wbki_kbman_t* kbman)
{
	return kbman->kc_sys;
}

int
wbki_kbman_add(wbki_kbman_t *kbman, wbk_kc_sys_t *kc_sys)
{
	array_add(kbman->kc_sys, kc_sys);
}

int
wbki_kbman_exec(wbki_kbman_t *kbman, wbk_b_t *b)
{
	int ret;
	char found;
	ArrayIter kb_iter;
	wbk_kc_sys_t *kc_sys;

	ret = -1;

	found = 0;
	array_iter_init(&kb_iter, wbki_kbman_get_kb(kbman));
	while (!found && array_iter_next(&kb_iter, (void *) &kc_sys) != CC_ITER_END) {
		if (wbk_b_compare(wbk_kc_sys_get_binding(kc_sys), b) == 0) {
			found = 1;
		}
	}

	if (found) {
		wbk_kc_sys_exec(kc_sys);
		ret = 0;
	}

	return ret;

}

