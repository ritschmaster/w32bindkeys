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
#include <sys/types.h>
#include <unistd.h>
#include <collectc/treeset.h>

#if defined(WIN32)
#include <windows.h>
#endif

#include "logger.h"

static wbk_logger_t logger =  { "kb" };

static int
be_cmp(const void *k1, const void *k2);

int
be_cmp(const void *k1, const void *k2)
{
	wbk_be_t *be_a;
	wbk_be_t *be_b;

	int a;
	int b;

    be_a = (wbk_be_t *) k1;
    be_b = (wbk_be_t *) k2;

    a = be_a->modifier + be_a->key;
    b = be_b->modifier + be_a->key;

    if (a < b)
        return -1;
    else if (a > b)
        return 1;
    else
        return 0;
}

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

int
wbk_be_free(wbk_be_t *be)
{
	free(be);

	return 0;
}

wbk_b_t *
wbk_b_new()
{
	wbk_b_t *b;

	b = NULL;
	b = malloc(sizeof(wbk_b_t));

	if (treeset_new(be_cmp, &(b->comb))) {
		free(b);
		b = NULL;
	}

	return b;
}

int
wbk_b_free(wbk_b_t *b)
{
	TreeSetIter be_iter;
	wbk_be_t *my_be;

	my_be = NULL;
	treeset_iter_init(&be_iter, wbk_b_get_comb(b));
	while (treeset_iter_next(&be_iter, (void *) &my_be) != CC_ITER_END) {
		treeset_iter_remove(&be_iter, NULL);
		wbk_be_free(my_be);
		my_be = NULL;
	}

	treeset_destroy(b->comb);
	b->comb = NULL;

	free(b);
	return 0;
}

int
wbk_b_add(wbk_b_t *b, const wbk_be_t *be)
{
	int ret;
//	ArrayIter be_iter;
//	wbk_be_t *my_be;
	wbk_be_t *copy;

//	ret = 0;
//	array_iter_init(&be_iter, wbk_b_get_comb(b));
//	while (array_iter_next(&be_iter, (void *) &my_be) != CC_ITER_END && !ret) {
//		if (my_be->modifier == be->modifier
//			&& my_be->key == be->key) {
//			ret = 1;
//		}
//	}
//
//	if (!ret) {
//		copy = wbk_be_new(be->modifier, be->key);
//		array_add(b->comb, copy);
//	}

	copy = wbk_be_new(be->modifier, be->key);

	if (treeset_add(b->comb, copy) == CC_OK) {
		ret = 0;
	} else {
		ret = 1;
	}

	return ret;
}

int
wbk_b_remove(wbk_b_t *b, const wbk_be_t *be)
{
	int ret;
	TreeSetIter be_iter;
	wbk_be_t *my_be;

	ret = 1;

	my_be = NULL;
	treeset_iter_init(&be_iter, wbk_b_get_comb(b));
	while (treeset_iter_next(&be_iter, (void *) &my_be) != CC_ITER_END && ret) {
		if (my_be->modifier == be->modifier
			&& my_be->key == be->key) {
			treeset_iter_remove(&be_iter, NULL);
			wbk_be_free(my_be);
			my_be = NULL;
			ret = 0; /** removed */
		}
	}

	return ret;
}

TreeSet *
wbk_b_get_comb(const wbk_b_t *b)
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

const wbk_b_t *
wbk_kb_get_comb(const wbk_kb_t *kb)
{
	return kb->comb;
}

const char *
wbk_kb_get_cmd(const wbk_kb_t *kb)
{
	return kb->cmd;
}

int
wbk_kb_exec(const wbk_kb_t *kb)
{
#if defined(WIN32)
	if (system(kb->cmd)) {
		wbk_logger_log(&logger, SEVERE, "Exec failed: %s\n", kb->cmd);
	} else {
		wbk_logger_log(&logger, INFO, "Exec: %s\n", kb->cmd);
	}
#else
	pid_t pid;

	pid = fork();

	if (pid == 0) {
		if (system(kb->cmd)) {
			wbk_logger_log(&logger, SEVERE, "Exec failed: %s\n", kb->cmd);
		} else {
			wbk_logger_log(&logger, INFO, "Exec: %s\n", kb->cmd);
		}
		_exit(EXIT_SUCCESS);
	}
#endif

	return 0;
}
