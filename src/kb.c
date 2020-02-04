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

#include "kb.h"

#include <stdlib.h>
// #include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "logger.h"

static wbk_logger_t logger =  { "kb" };

wbk_be_t *
wbk_be_new(wbk_mk_t modifier, char key)
{
	wbk_be_t *be;

	be = NULL;
	be = malloc(sizeof(wbk_be_t));

	be->modifier = modifier;
	be->key = key;

	return be;
}

wbk_b_t *
wbk_b_new()
{
	wbk_b_t *b;

	b = NULL;
	b = malloc(sizeof(wbk_b_t));

	if (array_new(&(b->comb))) {
		free(b);
		b = NULL;
	}

	return b;
}

int
wbk_b_free(wbk_b_t *b)
{
	array_destroy_cb(b->comb, free);
	b->comb = NULL;
	free(b);
	return 0;
}

int
wbk_b_add(wbk_b_t *b, wbk_be_t *be)
{
	wbk_be_t *copy;

	copy = malloc(sizeof(wbk_be_t));
	copy = be;

	return array_add(b->comb, copy);
}

Array *
wbk_b_get_comb(wbk_b_t *b)
{
	return b->comb;
}

wbk_kb_t *
wbk_kb_new(wbk_b_t *comb, char *cmd)
{
	wbk_kb_t *kb;
	int length;

	kb = NULL;
	kb = malloc(sizeof(wbk_kb_t));
	memset(kb, 0, sizeof(wbk_kb_t));

	if (kb != NULL) {
		kb->comb = comb;
	}

	if (kb != NULL) {
		kb->cmd = cmd;
	}

	return kb;
}

int
wbk_kb_free(wbk_kb_t *kb)
{
	if (kb->comb) {
		wbk_b_free(kb->comb);
		kb->comb = NULL;
	}

	if (kb->cmd) {
		free(kb->cmd);
	}

	free(kb);
	return 0;
}

wbk_b_t *
wbk_kb_get_comb(wbk_kb_t *kb)
{
	return kb->comb;
}

const char *
wbk_kb_get_cmd(wbk_kb_t *kb)
{
	return kb->cmd;
}

int
wbk_kb_exec(wbk_kb_t *kb)
{
	pid_t pid;

	pid = fork();

	if (pid == 0) {
		system(kb->cmd);
		_exit(EXIT_SUCCESS);
	}

	return 0;
}
