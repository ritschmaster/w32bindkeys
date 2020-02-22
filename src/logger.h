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
 * @date 30 January 2020
 * @brief File contains the logger class definition
 */

#ifndef WBK_LOGGER_H
#define WBK_LOGGER_H

typedef enum wbk_loglevel_e
{
	DEBUG = 0,
	INFO,
	WARNING,
	SEVERE
} wbk_loglevel_t;

typedef struct wbk_logger_s
{
	char name[255];
} wbk_logger_t;

extern int
wbk_logger_set_level(wbk_loglevel_t level);

extern int
wbk_logger_log(wbk_logger_t *logger, wbk_loglevel_t level, const char *fmt, ...);

extern int
wbk_logger_err(wbk_logger_t *logger, const char *fmt, ...);

#endif // WBK_LOGGER_H
