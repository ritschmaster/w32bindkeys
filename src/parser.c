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

#include "parser.h"

#include <stdlib.h>
#include <stdio.h>

#include "util.h"

typedef enum parser_state_s {
	NONE,
	CMD,
	BINDING
} parser_state_t;

wbk_parser_t *
wbk_parser_new(const char *filename)
{
	wbk_parser_t *parser;
	int length;

	parser = NULL;
	parser = malloc(sizeof(wbk_parser_t));
	memset(parser, 0, sizeof(wbk_parser_t));

	if (parser != NULL) {
		length = strlen(filename);
		parser->filename = malloc(sizeof(char) * length);

		if (parser->filename != NULL) {
			strcpy(parser->filename, filename);
		}
	}

	return parser;
}

int
wbk_parser_free(wbk_parser_t *parser)
{
	if (parser->filename != NULL) {
		free(parser->filename);
	}
	free(parser);

	return 0;
}

wbk_kbman_t *
wbk_parser_parse(wbk_parser_t *parser)
{
	return NULL;
}

const char *
wbk_parser_get_filename(wbk_parser_t *parser)
{
	return parser->filename;
}
