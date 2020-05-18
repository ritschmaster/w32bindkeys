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
 * @brief File contains the interpreter parser class definition
 */

#ifndef WBK_PARSER_H
#define WBK_PARSER_H

#include "kbman.h"

typedef struct wbk_parser_s
{
	char *filename;
} wbk_parser_t;

extern wbk_parser_t *
wbk_parser_new(const char *filename);

extern int
wbk_parser_free(wbk_parser_t *parser);

extern wbk_kbman_t *
wbk_parser_parse(wbk_parser_t *parser);

/**
 * @brief Gets the filename used by the parser
 * @return The filename used by the parser. Do not free the returned string.
 */
extern const char *
wbk_parser_get_filename(wbk_parser_t *parser);

#endif // WBK_PARSER_H
