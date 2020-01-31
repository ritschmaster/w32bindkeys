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

#include <stdlib.h>

char *
wbk_intarr_to_str(Array *array)
{
	char *str;
	int *character;
	int pos;
	int temp;
	ArrayIter iter;

	pos = 0;
	array_iter_init(&iter, array);
	str = malloc(0);
	while (array_iter_next(&iter, (void *) &character) != CC_ITER_END) {
		temp = *character;

		str = realloc(str, pos + 1); // TODO
		str[pos] = temp;
		pos = pos + 1;

//		temp = temp << 24;
//		if (temp) {
//			temp = temp << 16;
//			if (temp) {
//				temp = temp << 8;
//				if (temp) {
//					str = realloc(str, pos + 4);
//					str[pos] = *character;
//					str[pos + 1] = *character << 8;
//					str[pos + 2] = *character << 16;
//					str[pos + 3] = *character << 24;
//					pos = pos + 4;
//				} else {
//					str = realloc(str, pos + 3);
//					str[pos] = *character << 8;
//					str[pos + 1] = *character << 16;
//					str[pos + 2] = *character << 24;
//					pos = pos + 3;
//				}
//			} else {
//				str = realloc(str, pos + 2);
//				str[pos] = *character << 16;
//				str[pos + 1] = *character << 24;
//				pos = pos + 2;
//			}
//		} else {
//			str = realloc(str, pos + 1);
//			str[pos] = *character << 24;
//			pos = pos + 1;
//		}
	}

	str = realloc(str, pos + 1);
	str[pos] = '\0';
	pos = pos + 1;

	return str;
}
