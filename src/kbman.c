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

#include <windows.h>

#include "logger.h"
#include "kbman.h"
#include "kbman.h"

static wbk_logger_t logger =  { "kbman" };

static wbk_kbman_t *
wbk_kbman_free_impl(wbk_kbman_t *kbman);

static int
wbk_kbman_add_impl(wbk_kbman_t *kbman, wbk_kc_sys_t *kc_sys);

static wbk_kbman_t **
wbk_kbman_split_impl(wbk_kbman_t *kbman, int nominator);

static int
wbk_kbman_exec_impl(wbk_kbman_t *kbman, wbk_b_t *b);

wbk_kbman_t *
wbk_kbman_new()
{
	wbk_kbman_t *kbman;

	kbman = NULL;
	kbman = malloc(sizeof(wbk_kbman_t));

  if (kbman) {
    kbman->kbman_free = wbk_kbman_free_impl;
    kbman->kbman_add = wbk_kbman_add_impl;
    kbman->kbman_split = wbk_kbman_split_impl;
    kbman->kbman_exec = wbk_kbman_exec_impl;

    kbman->kc_sys_arr_len = 0;
    kbman->kc_sys_arr = NULL;
  }

  return kbman;
}

wbk_kbman_t *
wbk_kbman_free(wbk_kbman_t *kbman)
{
  return kbman->kbman_free(kbman);
}

int
wbk_kbman_add(wbk_kbman_t *kbman, wbk_kc_sys_t *kc_sys)
{
  return kbman->kbman_add(kbman, kc_sys);
}

wbk_kbman_t **
wbk_kbman_split(wbk_kbman_t *kbman, int nominator)
{
  return kbman->kbman_split(kbman, nominator);
}

int
wbk_kbman_exec(wbk_kbman_t *kbman, wbk_b_t *b)
{
  return kbman->kbman_exec(kbman, b);
}

wbk_kbman_t *
wbk_kbman_free_impl(wbk_kbman_t *kbman)
{
	int i;

	for (i = 0; i < kbman->kc_sys_arr_len; i++) {
		wbk_kc_free((wbk_kc_t *) kbman->kc_sys_arr[i]);
		kbman->kc_sys_arr[i] = NULL;
	}
	free(kbman->kc_sys_arr);
	kbman->kc_sys_arr = NULL;

	free(kbman);
}

int
wbk_kbman_add_impl(wbk_kbman_t *kbman, wbk_kc_sys_t *kc_sys)
{
	kbman->kc_sys_arr_len++;
	kbman->kc_sys_arr = realloc(kbman->kc_sys_arr,
							    sizeof(wbk_kc_sys_t **) * kbman->kc_sys_arr_len);
	kbman->kc_sys_arr[kbman->kc_sys_arr_len - 1] = kc_sys;
	return 0;
}

wbk_kbman_t **
wbk_kbman_split_impl(wbk_kbman_t *kbman, int nominator)
{
	wbk_kbman_t **kbmans;
	int i;
	int j;

	kbmans = malloc(sizeof(wbk_kbman_t **) * nominator);

	for (i = 0; i < nominator; i++) {
		kbmans[i] = wbk_kbman_new();
		for (j = 0; j < kbman->kc_sys_arr_len; j++) {
			if (j % nominator == i) {
				wbk_kbman_add(kbmans[i], wbk_kc_sys_clone(kbman->kc_sys_arr[j]));
			}
		}
	}

	return kbmans;
}

int
wbk_kbman_exec_impl(wbk_kbman_t *kbman, wbk_b_t *b)
{
	int error;
	int i;
	int found_at;

	error = 1;

	found_at = -1;
	for (i = 0; found_at < 0 && i < kbman->kc_sys_arr_len; i++) {
		if (wbk_b_compare(wbk_kc_get_binding((wbk_kc_t *) kbman->kc_sys_arr[i]), b) == 0) {
			found_at = i;
		}
	}

	if (found_at >= 0) {
		error = wbk_kc_exec((wbk_kc_t *) kbman->kc_sys_arr[found_at]);
	}

	return error;
}
