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

#include <string.h>

int main(void)
{
	Array *arr;
	int *i;
	char *str;

	array_new(&arr);

	i = malloc(sizeof(int));
	*i = 'a';
	array_add(arr, i);
	i = malloc(sizeof(int));
	*i = 'b';
	array_add(arr, i);
	i = malloc(sizeof(int));
	*i = 'c';
	array_add(arr, i);
	i = malloc(sizeof(int));
	*i = 'd';
	array_add(arr, i);
	i = malloc(sizeof(int));
	*i = '\0';
	array_add(arr, i);


	str = wbk_intarr_to_str(arr);
	if (strcmp(str, "abcd") != 0) {
		return 1;
	}

	free(str);

	array_destroy_cb(arr, free);

	return 0;
}
