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
 * @brief File contains the binding class implementation and private methods
 */

#include "b.h"

#include <stdlib.h>

#include "logger.h"

static wbk_logger_t logger =  { "b" };

static int
be_cmp(const void *k1, const void *k2);

int
be_cmp(const void *k1, const void *k2)
{
	wbk_be_t *be_a;
	wbk_be_t *be_b;

    be_a = *(wbk_be_t **) k1;
    be_b = *(wbk_be_t **) k2;

    return wbk_be_compare(be_a, be_b);
}

wbk_b_t *
wbk_b_new()
{
	wbk_b_t *b;

	b = NULL;
	b = malloc(sizeof(wbk_b_t));

	if (array_new(&(b->comb))) {
		free(b);
		b = NULL;
	}

	return b;
}

int
wbk_b_free(wbk_b_t *b)
{
	array_destroy_cb(b->comb, free);
	b->comb = NULL;

	free(b);
	return 0;
}

int
wbk_b_add(wbk_b_t *b, const wbk_be_t *be)
{
	int ret;
	ArrayIter be_iter;
	wbk_be_t *my_be;
	wbk_be_t *copy;

	ret = 0;

	my_be = NULL;
	array_iter_init(&be_iter, wbk_b_get_comb(b));
	while (array_iter_next(&be_iter, (void *) &my_be) != CC_ITER_END && !ret) {
		if (my_be->modifier == be->modifier
			&& my_be->key == be->key) {
			ret = 1;
		}
	}

	if (!ret) {
		copy = wbk_be_new(be->modifier, be->key);
		array_add(b->comb, copy);
		array_sort(b->comb, be_cmp);
	}
	return ret;
}

int
wbk_b_remove(wbk_b_t *b, const wbk_be_t *be)
{
	int ret;
	ArrayIter be_iter;
	wbk_be_t *my_be;

	ret = 1;

	my_be = NULL;
	array_iter_init(&be_iter, wbk_b_get_comb(b));
	while (array_iter_next(&be_iter, (void *) &my_be) != CC_ITER_END && ret) {
		if (my_be->modifier == be->modifier
			&& my_be->key == be->key) {
			array_iter_remove(&be_iter, NULL);
			wbk_be_free(my_be);
			my_be = NULL;
			ret = 0; /** removed */
		}
	}

	if (!ret) {
		array_sort(b->comb, be_cmp);
	}

	return ret;
}

int
wbk_b_contains(wbk_b_t *b, const wbk_be_t *be)
{
	char found;
	ArrayIter be_iter;
	wbk_be_t *my_be;

	found = 0;
	array_iter_init(&be_iter, wbk_b_get_comb(b));
	while (array_iter_next(&be_iter, (void *) &my_be) != CC_ITER_END && !found) {
		if (my_be->modifier == be->modifier
			&& my_be->key == be->key) {
			found = 1;
		}
	}

	found = 0;
	return found;
}

int
wbk_b_compare(const wbk_b_t *b, const wbk_b_t *other)
{
	wbk_be_t *b_be;
	wbk_be_t *other_be;

	char found;

	int other_be_len;
	int b_be_len;

	int i;
	int j;

	found = 0;

	i = -1;
	j = -1;
	other_be_len = array_size(wbk_b_get_comb(other));
	b_be_len = array_size(wbk_b_get_comb(b));

	found = 1;
	do {
		i++;
		j++;
		if ((i < other_be_len && j >= b_be_len)
			|| (i >= other_be_len && j < b_be_len)) {
			found = 0;
		} else {

		array_get_at(wbk_b_get_comb(other), i, (void *) &other_be);
		array_get_at(wbk_b_get_comb(b), j, (void *) &b_be);

		if (other_be->modifier != b_be->modifier
			|| other_be->key != b_be->key) {
			found = 0;
		}
		}
	} while (i < other_be_len &&
			 j < other_be_len &&
			 found);

	return !found;
}

Array *
wbk_b_get_comb(const wbk_b_t *b)
{
	return b->comb;
}
