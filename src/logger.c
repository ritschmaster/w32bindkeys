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
	int length;
	va_list argptr;
	va_start(argptr, fmt);

	if (global_level >= level) {
		length = strlen(fmt) + 6 + 1;
		extended_fmt = malloc(sizeof(char) * length);

		switch (level) {
		case INFO:
			strcpy(extended_fmt, "INFO: ");
			break;

		case WARNING:
			strcpy(extended_fmt, "WARN: ");
			break;

		case SEVERE:
			strcpy(extended_fmt, "SEVE: ");
			break;
		}

		strcpy(extended_fmt + 6, fmt);

		vfprintf(stdout, extended_fmt, argptr);
	}

	va_end(argptr);
}

int
wbk_logger_err(wbk_logger_t *logger, const char *fmt, ...)
{
	va_list argptr;
	va_start(argptr, fmt);

	vfprintf(stderr, fmt, argptr);

	va_end(argptr);
}
