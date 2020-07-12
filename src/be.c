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
 * @brief File contains the binding element class implementation and private methods
 */

#include "be.h"

#include <stdlib.h>
#include <string.h>

#include "logger.h"

static wbk_logger_t logger =  { "be" };

static int
wbk_be_compare_key(char a, char b);

inline int
wbk_be_compare_key(char a, char b)
{
	if (a < b)
        return -1;
    else if (a > b)
        return 1;
    else
        return 0;
}

wbk_be_t *
wbk_be_new(wbk_mk_t modifier, char key)
{
	wbk_be_t *be;

	be = NULL;
	be = malloc(sizeof(wbk_be_t));

	be->modifier = modifier;
	be->key = key;

	return be;
}

int
wbk_be_free(wbk_be_t *be)
{
	free(be);

	return 0;
}

inline wbk_mk_t
wbk_be_get_modifier(const wbk_be_t *be)
{
	return be->modifier;
}

inline char
wbk_be_get_key(const wbk_be_t *be)
{
	return be->key;
}

inline int
wbk_be_compare(const wbk_be_t *be, const wbk_be_t *other)
{
	return memcmp(be, other, sizeof(wbk_be_t));
}


