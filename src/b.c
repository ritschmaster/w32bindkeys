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
#include <ctype.h>

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

wbk_b_t *
wbk_b_clone(const wbk_b_t *other)
{
	wbk_b_t *b;

	b = NULL;
	if (other) {
		b = wbk_b_new();
		memcpy(b->modifier_map, other->modifier_map, sizeof(wbk_mk_t) * WBK_B_MODIFER_MAP_LEN);
		memcpy(b->key_map, other->key_map, sizeof(char) * WBK_B_KEY_MAP_LEN);
	}

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

inline int
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

inline int
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

inline int
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

inline int
wbk_b_compare(const wbk_b_t *b, const wbk_b_t *other)
{
	return memcmp(b->modifier_map, other->modifier_map, WBK_B_MODIFER_MAP_LEN * sizeof(wbk_mk_t))
		   || memcmp(b->key_map, other->key_map, WBK_B_KEY_MAP_LEN * sizeof(char));
}

char *
wbk_b_to_str(const wbk_b_t *b)
{
	int i;
	int str_cur_pos;
	int length;
	char *str;

	str = malloc(sizeof(char) * 255);
	str[0] = '\0';
	str_cur_pos = 0;

	for (i = 0; i < WBK_B_MODIFER_MAP_LEN; i++) {
		if (b->modifier_map[i] == 1) {
			if (str_cur_pos > 0) {
				str[str_cur_pos++] = ' ';
				str[str_cur_pos++] = '+';
				str[str_cur_pos++] = ' ';
			}

			switch (i) {
			case WIN:
				strcpy(str+str_cur_pos, WIN_STR);
				str_cur_pos += strlen(WIN_STR);
				break;

			case ALT:
				strcpy(str+str_cur_pos, ALT_STR);
				str_cur_pos += strlen(ALT_STR);
				break;

			case CTRL:
				strcpy(str+str_cur_pos, CTRL_STR);
				str_cur_pos += strlen(CTRL_STR);
				break;

			case SHIFT:
				strcpy(str+str_cur_pos, SHIFT_STR);
				str_cur_pos += strlen(SHIFT_STR);
				break;

			case ENTER:
				strcpy(str+str_cur_pos, ENTER_STR);
				str_cur_pos += strlen(ENTER_STR);
				break;

			case NUMLOCK:
				strcpy(str+str_cur_pos, NUMLOCK_STR);
				str_cur_pos += strlen(NUMLOCK_STR);
				break;

			case CAPSLOCK:
				strcpy(str+str_cur_pos, CAPSLOCK_STR);
				str_cur_pos += strlen(CAPSLOCK_STR);
				break;

			case SCROLL:
				strcpy(str+str_cur_pos, SCROLL_STR);
				str_cur_pos += strlen(SCROLL_STR);
				break;

			case SPACE:
				strcpy(str+str_cur_pos, SPACE_STR);
				str_cur_pos += strlen(SPACE_STR);
				break;

			case F1:
				strcpy(str+str_cur_pos, F1_STR);
				str_cur_pos += strlen(F1_STR);
				break;

			case F2:
				strcpy(str+str_cur_pos, F2_STR);
				str_cur_pos += strlen(F2_STR);
				break;

			case F3:
				strcpy(str+str_cur_pos, F3_STR);
				str_cur_pos += strlen(F3_STR);
				break;

			case F4:
				strcpy(str+str_cur_pos, F4_STR);
				str_cur_pos += strlen(F4_STR);
				break;

			case F5:
				strcpy(str+str_cur_pos, F5_STR);
				str_cur_pos += strlen(F5_STR);
				break;

			case F6:
				strcpy(str+str_cur_pos, F6_STR);
				str_cur_pos += strlen(F6_STR);
				break;

			case F7:
				strcpy(str+str_cur_pos, F7_STR);
				str_cur_pos += strlen(F7_STR);
				break;

			case F8:
				strcpy(str+str_cur_pos, F8_STR);
				str_cur_pos += strlen(F8_STR);
				break;

			case F9:
				strcpy(str+str_cur_pos, F9_STR);
				str_cur_pos += strlen(F9_STR);
				break;

			case F10:
				strcpy(str+str_cur_pos, F10_STR);
				str_cur_pos += strlen(F10_STR);
				break;

			case F11:
				strcpy(str+str_cur_pos, F11_STR);
				str_cur_pos += strlen(F11_STR);
				break;

			case F12:
				strcpy(str+str_cur_pos, F12_STR);
				str_cur_pos += strlen(F12_STR);
				break;

			default:
				break;
			}
		}
	}

	for (i = 0; i < WBK_B_KEY_MAP_LEN; i++) {
		if (b->key_map[i] == 1) {
			if (str_cur_pos > 0) {
				str[str_cur_pos++] = ' ';
				str[str_cur_pos++] = '+';
				str[str_cur_pos++] = ' ';
			}

			str[str_cur_pos++] = tolower(i);
			str[str_cur_pos] = '\0';
		}
	}

	return str;
}
