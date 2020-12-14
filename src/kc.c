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
 * @date 2020-01-26
 * @brief File contains the key binding command class implementation and private methods
 */

#include "kc.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <windows.h>

#include "logger.h"

static wbk_logger_t logger =  { "kc" };

static wbk_kc_t *
wbk_kc_clone_impl(const wbk_kc_t *other);

/**
 * Implementation of wbk_kc_free().
 */
static int
wbk_kc_free_impl(wbk_kc_t *kc);

/**
 * Implementation of wbk_kc_get_binding().
 */
static const wbk_b_t *
wbk_kc_get_binding_impl(const wbk_kc_t *kc);

/**
 * Implementation of wbk_kc_exec().
 */
static int
wbk_kc_exec_impl(const wbk_kc_t *kc);


static DWORD WINAPI
wbk_kbthread_exec(LPVOID param);

wbk_kc_t *
wbk_kc_new(wbk_b_t *comb)
{
	wbk_kc_t *kc;
	int length;

	kc = NULL;
	kc = malloc(sizeof(wbk_kc_t));
	memset(kc, 0, sizeof(wbk_kc_t));

  kc->kc_clone = wbk_kc_clone_impl;
  kc->kc_free = wbk_kc_free_impl;
  kc->kc_get_binding = wbk_kc_get_binding_impl;
  kc->kc_exec = wbk_kc_exec_impl;

	if (kc != NULL) {
		kc->binding = comb;
	}

	return kc;
}

wbk_kc_t *
wbk_kc_clone(const wbk_kc_t *other)
{
  return other->kc_clone(other);
}

int
wbk_kc_free(wbk_kc_t *kc)
{
  return kc->kc_free(kc);
}

const wbk_b_t *
wbk_kc_get_binding(const wbk_kc_t *kc)
{
  return kc->kc_get_binding(kc);
}

int
wbk_kc_exec(const wbk_kc_t *kc)
{
  return kc->kc_exec(kc);
}

wbk_kc_t *
wbk_kc_clone_impl(const wbk_kc_t *other)
{
	wbk_b_t *b;
	wbk_kc_t *kc;

	kc = NULL;
	if (other) {
		b = wbk_b_clone(other->binding);
		kc = wbk_kc_new(b);
	}

	return kc;
}

int
wbk_kc_free_impl(wbk_kc_t *kc)
{
	if (kc->binding) {
		wbk_b_free(kc->binding);
		kc->binding = NULL;
	}

	free(kc);
	return 0;
}

const wbk_b_t *
wbk_kc_get_binding_impl(const wbk_kc_t *kc)
{
	return kc->binding;
}

int
wbk_kc_exec_impl(const wbk_kc_t *kc)
{
	return 1;
}
