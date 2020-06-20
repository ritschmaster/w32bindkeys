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

#include "logger.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

/**
 * - 4 characters for the year
 * - 1 character for a dash
 * - 2 characters for the month
 * - 1 character for a dash
 * - 2 characters for the day
 *
 * - 1 character for a blank
 *
 * - 2 characters for the hour
 * - 1 character for a colon
 * - 2 characters for the minute
 * - 1 character for a colon
 * - 2 characters for the second
 *
 * - 1 character for a blank
 * - 4 characters for the log level
 * - 1 character for a blank
 *
 * - 1 character for a colon
 * - 1 character for a blank
 */
#define FMT_ADDITIONAL_LEN 27

#define FMT "%04d-%02d-%02d %02d:%02d:%02d %s %s: "

#define LEVEL_DEBUG "DEBU"
#define LEVEL_INFO "INFO"
#define LEVEL_WARNING "WARN"
#define LEVEL_SEVERE "SEVE"

static wbk_loglevel_t global_level;

int
wbk_logger_set_level(wbk_loglevel_t level)
{
	global_level = level;
}

int
wbk_logger_log(wbk_logger_t *logger, wbk_loglevel_t level, const char *fmt, ...)
{
	char *extended_fmt;
	int name_length;
	int length;
	time_t rawtime;
	struct tm *tm;
	va_list argptr;

	if (level >= global_level) {
		time(&rawtime);
		tm = gmtime(&rawtime);

		name_length = strlen(logger->name);
		length = strlen(fmt) + FMT_ADDITIONAL_LEN + name_length + 1;
		extended_fmt = malloc(sizeof(char) * length);

		switch (level) {
		case DEBUG:
			sprintf(extended_fmt, FMT,
					tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
					tm->tm_hour, tm->tm_min, tm->tm_sec,
					LEVEL_DEBUG,
					logger->name);
			break;

		case INFO:
			sprintf(extended_fmt, FMT,
					tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
					tm->tm_hour, tm->tm_min, tm->tm_sec,
					LEVEL_INFO,
					logger->name);
			break;

		case WARNING:
			sprintf(extended_fmt, FMT,
					tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
					tm->tm_hour, tm->tm_min, tm->tm_sec,
					LEVEL_WARNING,
					logger->name);
			break;

		case SEVERE:
			sprintf(extended_fmt, FMT,
					tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
					tm->tm_hour, tm->tm_min, tm->tm_sec,
					LEVEL_SEVERE,
					logger->name);
			break;
		}

		strcpy(extended_fmt + FMT_ADDITIONAL_LEN + name_length, fmt);


		va_start(argptr, fmt);
		vfprintf(stdout, extended_fmt, argptr);
		va_end(argptr);
	}

	if (global_level == DEBUG) {
		fflush(stdout);
	}

	return 0;
}

int
wbk_logger_err(wbk_logger_t *logger, const char *fmt, ...)
{
	va_list argptr;
	va_start(argptr, fmt);

	vfprintf(stderr, fmt, argptr);

	va_end(argptr);
}

