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

#ifndef WBKI_PARSER_H
#define WBKI_PARSER_H

#include "parser.h"
#include "ikbman.h"

typedef struct wbki_parser_s
{
	wbk_parser_t *parser;
} wbki_parser_t;

extern wbki_parser_t *
wbki_parser_new(const char *filename);

extern int
wbki_parser_free(wbki_parser_t *parser);

extern wbki_kbman_t *
wbki_parser_parse(wbki_parser_t *parser);

/**
 * @brief Gets the filename used by the parser
 * @return The filename used by the parser. Do not free the returned string.
 */
extern const char *
wbki_parser_get_filename(wbki_parser_t *parser);

#endif // WBKI_PARSER_H
