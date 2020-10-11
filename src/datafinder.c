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
 * @date 2020-02-20
 * @brief File contains data finder class implementation and private methods
 */

#include "datafinder.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#if defined(WIN32)
#include <windows.h>
#endif

#include "logger.h"

static wbk_logger_t logger = { "datafinder" };

wbk_datafinder_t *
wbk_datafinder_new(const char *datadir)
{
	wbk_datafinder_t *datafinder;
	size_t size;
	wchar_t w32_buffer[MAX_PATH];

	datafinder = malloc(sizeof(wbk_datafinder_t));

	array_new(&(datafinder->datadir_arr));

	wbk_datafinder_add_datadir(datafinder, datadir);

	GetModuleFileNameA(NULL, (LPSTR) w32_buffer, MAX_PATH);

	size = sizeof(char) * (sizeof(wchar_t) * wcslen(w32_buffer) + 1);
	datafinder->execdir = malloc(size);
	memset(datafinder->execdir, 0, size);
	wcstombs(datafinder->execdir, w32_buffer, size);
	datafinder->execdir_len = strlen(datafinder->execdir) + 1;

	return datafinder;
}

extern int
wbk_datafinder_free(wbk_datafinder_t *datafinder)
{
	ArrayIter iter;
	char *datadir_iter;

	array_iter_init(&iter, datafinder->datadir_arr);
	while (array_iter_next(&iter, (void *) &datadir_iter) != CC_ITER_END) {
		free(datadir_iter);
		array_iter_remove(&iter, NULL);
	}

	datafinder->datadir_arr = NULL;

	free(datafinder->execdir);
	datafinder->execdir = NULL;

	free(datafinder);

	return 0;
}

int
wbk_datafinder_add_datadir(wbk_datafinder_t *datafinder, const char *datadir)
{
	char *path;

	path = malloc(sizeof(char) * (strlen(datadir) + 1));
	strcpy(path, datadir);

	return array_add(datafinder->datadir_arr, path);
}

char *
wbk_datafinder_gen_path(const wbk_datafinder_t *datafinder, const char *data_file)
{
	ArrayIter iter;
	char *datadir_iter;
	char *absolute_path;
	int datadir_len;
	int data_file_len;

	data_file_len = strlen(data_file) + 1;

	absolute_path = malloc(sizeof(char) * (datafinder->execdir_len + data_file_len));
	strcpy(absolute_path, datafinder->execdir);
	if (datafinder->execdir[0] != '\0') {
		absolute_path[datafinder->execdir_len - 1] = '/';
		strcpy(absolute_path + datafinder->execdir_len, data_file);
	} else {
		strcpy(absolute_path, data_file);
	}


	wbk_logger_log(&logger, INFO, "Probing file: %s\n", absolute_path);
	array_iter_init(&iter, datafinder->datadir_arr);
	while (access(absolute_path, F_OK)
		   && array_iter_next(&iter, (void *) &datadir_iter) != CC_ITER_END) {
		free(absolute_path);

		datadir_len = strlen(datadir_iter) + 1;
		absolute_path = malloc(sizeof(char) * (datadir_len + data_file_len));
		strcpy(absolute_path, datadir_iter);
		if (datadir_iter[0] != '\0') {
			absolute_path[datadir_len - 1] = '/';
			strcpy(absolute_path + datadir_len, data_file);
		} else {
			strcpy(absolute_path, data_file);
		}

		wbk_logger_log(&logger, INFO, "Probing file: %s\n", absolute_path);
	}

	if(access(absolute_path, F_OK)) {
		free(absolute_path);
		absolute_path = NULL;
	}

	if (absolute_path) {
		wbk_logger_log(&logger, INFO, "Using file: %s\n", absolute_path);
	} else {
		wbk_logger_log(&logger, WARNING, "Cannot find file: %s\n", data_file);
	}

	return absolute_path;
}
