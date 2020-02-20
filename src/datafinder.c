
#include "datafinder.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#if defined(WIN32)
#include <windows.h>
#endif

#include "logger.h"

wbk_logger_t logger = { "datafinder" };

wbk_datafinder_t *
wbk_datafinder_new(const char *datadir)
{
	wbk_datafinder_t *datafinder;
	size_t size;
#if defined(WIN32)
	wchar_t w32_buffer[MAX_PATH];
#endif

	datafinder = malloc(sizeof(wbk_datafinder_t));
	datafinder->datadir_len = strlen(datadir) + 1;
	datafinder->datadir = malloc(sizeof(char) * datafinder->datadir_len);
	strcpy(datafinder->datadir, datadir);

#if defined(WIN32)
	GetModuleFileNameA(NULL, w32_buffer, MAX_PATH);

	size = sizeof(char) * (sizeof(wchar_t) * wcslen(w32_buffer) + 1);
	datafinder->execdir = malloc(size);
	memset(datafinder->execdir, 0, size);
	wcstombs(datafinder->execdir, w32_buffer, size);
	datafinder->execdir_len = strlen(datafinder->execdir) + 1;
#else
	datafinder->execdir_len = sizeof(char) * 1;
	datafinder->execdir = malloc(datafinder->execdir_len);
	datafinder->execdir[0] = '\0';
#endif

	return datafinder;
}

extern int
wbk_datafinder_free(wbk_datafinder_t *datafinder)
{
	free(datafinder->datadir);
	datafinder->datadir = NULL;

	free(datafinder->execdir);
	datafinder->execdir = NULL;

	free(datafinder);

	return 0;
}

char *
wbk_datafinder_gen_path(const wbk_datafinder_t *datafinder, const char *data_file)
{
	char *absolute_path;
	int data_file_len;

	data_file_len = strlen(data_file) + 1;

	absolute_path = malloc(sizeof(char) * (datafinder->datadir_len + data_file_len));
	strcpy(absolute_path, datafinder->datadir);
	if (datafinder->datadir[0] != '\0') {
		absolute_path[datafinder->datadir_len - 1] = '/';
		strcpy(absolute_path + datafinder->datadir_len, data_file);
	} else {
		strcpy(absolute_path, data_file);
	}

	wbk_logger_log(&logger, INFO, "Probing file: %s\n", absolute_path);
	if(access(absolute_path, F_OK)) {
		free(absolute_path);

		absolute_path = malloc(sizeof(char) * (datafinder->execdir_len + data_file_len));
		strcpy(absolute_path, datafinder->execdir);
		if (datafinder->execdir[0] != '\0') {
			absolute_path[datafinder->execdir_len - 1] = '/';
			strcpy(absolute_path + datafinder->execdir_len, data_file);
		} else {
			strcpy(absolute_path, data_file);
		}

		wbk_logger_log(&logger, INFO, "Probing file: %s\n", absolute_path);
		if(access(absolute_path, F_OK)) {
			free(absolute_path);
			absolute_path = NULL;
		}
	}

	if (absolute_path) {
		wbk_logger_log(&logger, INFO, "Using file: %s\n", absolute_path);
	}

	return absolute_path;
}
