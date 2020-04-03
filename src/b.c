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
#include <string.h>

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

	wbk_b_reset(b);

	return b;
}

int
wbk_b_free(wbk_b_t *b)
{
	free(b);
	return 0;
}

int
wbk_b_reset(wbk_b_t *b)
{
	memset(b->modifier_map, 0, sizeof(wbk_mk_t) * WBK_B_MODIFER_MAP_LEN);
	memset(b->key_map, 0, sizeof(char) * WBK_B_KEY_MAP_LEN);

	return 0;
}

int
wbk_b_add(wbk_b_t *b, const wbk_be_t *be)
{
	int ret;

	ret = 1;
	if (wbk_be_get_modifier(be) != 0) {
		if (b->modifier_map[wbk_be_get_modifier(be)] == 0) {
			b->modifier_map[wbk_be_get_modifier(be)] = 1;
			ret = 0;
		}
	} else if (wbk_be_get_key(be) != 0) {
		if (b->key_map[wbk_be_get_key(be)] == 0) {
			b->key_map[wbk_be_get_key(be)] = 1;
			ret = 0;
		}
	}

	return ret;
}

int
wbk_b_remove(wbk_b_t *b, const wbk_be_t *be)
{
	int ret;

	ret = 1;
	if (wbk_be_get_modifier(be) != 0) {
		if (b->modifier_map[wbk_be_get_modifier(be)] != 0) {
			b->modifier_map[wbk_be_get_modifier(be)] = 0;
			ret = 0;
		}
	} else if (wbk_be_get_key(be) != 0) {
		if (b->key_map[wbk_be_get_key(be)] != 0) {
			b->key_map[wbk_be_get_key(be)] = 0;
			ret = 0;
		}
	}

	return ret;
}

int
wbk_b_contains(wbk_b_t *b, const wbk_be_t *be)
{
	int found;

	found = 0;
	if (b->modifier_map[wbk_be_get_modifier(be)] != 0
		|| b->key_map[wbk_be_get_key(be)] != 0) {
		found = 0;
	}

	return found;
}

int
wbk_b_compare(const wbk_b_t *b, const wbk_b_t *other)
{
	int differs;
	int i;

	differs = 0;

	for (i = 0; !differs && i < WBK_B_MODIFER_MAP_LEN; i++) {
		if (b->modifier_map[i] != other->modifier_map[i]) {
			differs = 1;
		}
	}

	for (i = 0; !differs && i < WBK_B_KEY_MAP_LEN; i++) {
		if (b->key_map[i] != other->key_map[i]) {
			differs = 1;
		}
	}

	return differs;
}

