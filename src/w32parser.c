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

#include "w32parser.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "logger.h"
#include "parser.h"
#include "util.h"

static wbk_logger_t logger =  { "w32parser" };

typedef enum parser_state_s {
	NONE,
	CMD,
	BINDING
} parser_state_t;

void
parse_comment(FILE *file);

char *
parse_cmd(FILE *file);

wbk_mk_t
parse_token(const char *token);

wbk_b_t *
parse_binding(FILE *file, int first_character);

wbk_w32parser_t *
wbk_w32parser_new(const char *filename)
{
	wbk_w32parser_t *parser;
	int length;

	parser = NULL;
	parser = malloc(sizeof(wbk_w32parser_t));
	memset(parser, 0, sizeof(wbk_w32parser_t));

	parser->parser = wbk_parser_new(filename);

	return parser;
}

int
wbk_w32parser_free(wbk_w32parser_t *parser)
{
	wbk_parser_free(parser->parser);
	free(parser);

	return 0;
}

wbk_kbman_t *
wbk_w32parser_parse(wbk_w32parser_t *parser)
{
	FILE *file;
	int character;
	char *cmd;
	wbk_b_t *binding;
	wbk_kbman_t *kbman;
	wbk_kb_t *kb;

	wbk_logger_log(&logger, INFO, "Using config: %s\n", wbk_parser_get_filename(parser->parser));

	kbman = NULL;

	file = fopen(wbk_parser_get_filename(parser->parser), "r");

	if (file != NULL) {
		kbman = wbk_kbman_new();
		cmd = NULL;
		binding = NULL;
		do {
			character = fgetc(file);

			if (character == '"') {
				if (cmd) {
					free(cmd);
				}
				cmd = parse_cmd(file);
			} else if (character == '#') {
				parse_comment(file);
			} else if (character == ' ' ||
					   character == '\t' ||
					   character == '\n') {
			} else if (character != EOF) {
				if (binding) {
					wbk_b_free(binding);
				}
				binding = parse_binding(file, character);
			}

			if (cmd != NULL && binding != NULL) {
				kb = wbk_kb_new(binding, cmd);
				wbk_kbman_add(kbman, kb);
				kb = NULL;
				cmd = NULL;
				binding = NULL;
			}
		} while (character != EOF);
	} else {
		wbk_logger_err(&logger, "Could not read config: %s\n", wbk_parser_get_filename(parser->parser));
	}

	return kbman;
}

void
parse_comment(FILE *file)
{
	int character;

	do {
		character = fgetc(file);
	} while (character != '\n' && character != EOF);
}

char *
parse_cmd(FILE *file)
{
	char *cmd;
	int *character;
	int quotes;
	Array *line;

	array_new(&line);
	quotes = 1;
	do {
		character = malloc(sizeof(int));
		*character = fgetc(file);

		switch(*character) {
		case '"':
			quotes++;
			break;

		case '#':
			if (quotes % 2 == 0) {
				parse_comment(file);
				*character = EOF;
			}
			break;

		case '\n':
			*character = EOF;
			break;

		default:
			if (*character != EOF) {
				array_add(line, character);
			}
		}
	} while(*character != EOF);

	if (quotes % 2 == 0) {
		cmd = wbk_intarr_to_str(line);
	} else {
		cmd = NULL;
		// TODO throw error
	}

	array_destroy_cb(line, free);

	wbk_logger_log(&logger, INFO, "Parsed command: %s\n", cmd);

	return cmd;
}

wbk_mk_t
parse_token(const char *token)
{
	wbk_mk_t modifier_key;
	char *copy;
	int i;
	int length;

	length = strlen(token) + 1;
	copy = malloc(sizeof(char) * length);
	strcpy(copy, token);

	for (i = 0; i < length; i++) {
		copy[i] = (char) tolower(copy[i]); // TODO
	}

	if (strcmp(copy, "release") == 0) {
		modifier_key = ENTER;
	} else if (strcmp(copy,  "control") == 0) {
		modifier_key = CTRL;
	} else if (strcmp(copy,  "shift") == 0) {
		modifier_key = SHIFT;
	} else if (strcmp(copy,  "mod1") == 0) {
		modifier_key = ALT;
	} else if (strcmp(copy,  "mod2") == 0) {
		modifier_key = NUMLOCK;
	} else if (strcmp(copy,  "mod3") == 0) {
		modifier_key = CAPSLOCK;
	} else if (strcmp(copy,  "mod4") == 0) {
		modifier_key = WIN;
	} else if (strcmp(copy,  "mod5") == 0) {
		modifier_key = SCROLL;
	} else if (strcmp(copy,  "f1") == 0) {
		modifier_key = F1;
	} else if (strcmp(copy,  "f2") == 0) {
		modifier_key = F2;
	} else if (strcmp(copy,  "f3") == 0) {
		modifier_key = F3;
	} else if (strcmp(copy,  "f4") == 0) {
		modifier_key = F4;
	} else if (strcmp(copy,  "f5") == 0) {
		modifier_key = F5;
	} else if (strcmp(copy,  "f6") == 0) {
		modifier_key = F6;
	} else if (strcmp(copy,  "f7") == 0) {
		modifier_key = F7;
	} else if (strcmp(copy,  "f8") == 0) {
		modifier_key = F8;
	} else if (strcmp(copy,  "f9") == 0) {
		modifier_key = F9;
	} else if (strcmp(copy,  "f10") == 0) {
		modifier_key = F10;
	} else if (strcmp(copy,  "f11") == 0) {
		modifier_key = F11;
	} else if (strcmp(copy,  "f12") == 0) {
		modifier_key = F12;
	} else {
		modifier_key = NOT_A_MODIFIER;
	}

	free(copy);

	return modifier_key;
}

wbk_b_t *
parse_binding(FILE *file, int first_character)
{
	wbk_b_t *binding;
	wbk_be_t *be;
	int *character;
	Array *line;
	char *str;
	int length;
	char *token;
	char *rest;
	wbk_mk_t modifier_key;

	binding = wbk_b_new();

	array_new(&line);

	character = malloc(sizeof(int));
	*character = first_character;
	array_add(line, character);

	length = 0;
	do {
		character = malloc(sizeof(int));
		*character = fgetc(file);

		if (*character == '\n') {
			*character = EOF;
		} else if (*character == '#') {
			parse_comment(file);
			*character = EOF;
		} else if (*character != ' ' &&
				   *character != '\t') {
			length++;
			array_add(line, character);
		}
	} while(*character != EOF);

	str = wbk_intarr_to_str(line);

	wbk_logger_log(&logger, INFO, "Parsed binding:\n");
	if (length > 0) {
		rest = str;
		while ((token = strtok_r(rest, "+", &rest))) {
			modifier_key = parse_token(token);
			if (modifier_key == NOT_A_MODIFIER) {
				be = wbk_be_new(modifier_key, token[0]);
				wbk_logger_log(&logger, INFO, "Key: %c\n", token[0]);
			} else {
				be = wbk_be_new(modifier_key, '\0');
				wbk_logger_log(&logger, INFO, "Modifier: %d\n", modifier_key);
			}
			wbk_b_add(binding, be);
		}
	} else {
		// TODO use +
	}
	wbk_logger_log(&logger, INFO, "\n");

	free(str);
	array_destroy_cb(line, free);

	return binding;
}
