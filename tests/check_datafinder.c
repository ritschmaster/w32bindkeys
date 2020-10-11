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

#include "datafinder.h"

#include <stdlib.h>

#include "datafinder.h"

int
test_new(void)
{
	wbk_datafinder_t *datafinder;

	datafinder = wbk_datafinder_new(PKGDATADIR);
	if (datafinder == NULL)
		exit(1);

	wbk_datafinder_free(datafinder);

	return 0;
}

int
test_simple(void)
{
	wbk_datafinder_t *datafinder;
	char *path;

	datafinder = wbk_datafinder_new(PKGSRCDIR);
	if (datafinder == NULL)
		exit(1);


	path = wbk_datafinder_gen_path(datafinder, "main.c");
	if (path == NULL)
		exit(2);

	if (!(strstr(path, "main.c")
		  && strstr(path, PKGSRCDIR)))
		exit(3);

	free(path);

	path = wbk_datafinder_gen_path(datafinder, "Makefile.am");
	if (path == NULL)
		exit(4);

	if (strcmp(path, "Makefile.am"))
		exit(5);

	free(path);

	wbk_datafinder_free(datafinder);

	return 0;
}

int
test_multi(void)
{
	wbk_datafinder_t *datafinder;
	char *path;

	datafinder = wbk_datafinder_new(PKGDATADIR);
	if (datafinder == NULL)
		exit(1);

	if (wbk_datafinder_add_datadir(datafinder, PKGSRCDIR))
		exit(2);

	path = wbk_datafinder_gen_path(datafinder, "main.c");

	if (path == NULL)
		exit(3);

	if (!(strstr(path, "main.c")
		  && strstr(path, PKGSRCDIR)))
		exit(4);

	free(path);

	path = wbk_datafinder_gen_path(datafinder, "w32bindkeysrc");

	if (path == NULL)
		exit(3);

	if (!(strstr(path, "w32bindkeysrc")
		  && strstr(path, PKGDATADIR)))
		exit(4);

	free(path);

	path = wbk_datafinder_gen_path(datafinder, "Makefile.am");

	if (path == NULL)
		exit(3);

	if (strcmp(path, "Makefile.am"))
		exit(4);

	free(path);

	wbk_datafinder_free(datafinder);

	return 0;
}

int main(void)
{
	test_new();
	test_simple();
	test_multi();

	return 0;
}
