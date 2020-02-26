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

#if defined(WIN32)
#include <windows.h>
#endif

#include "logger.h"

static wbk_logger_t logger =  { "kb" };

static DWORD WINAPI
wbk_kbthread_exec(LPVOID param);

wbk_kb_t *
wbk_kb_new(wbk_b_t *comb, char *cmd)
{
	wbk_kb_t *kb;
	int length;

	kb = NULL;
	kb = malloc(sizeof(wbk_kb_t));
	memset(kb, 0, sizeof(wbk_kb_t));

	if (kb != NULL) {
		kb->binding = comb;
	}

	if (kb != NULL) {
		kb->cmd = cmd;
	}

	return kb;
}

int
wbk_kb_free(wbk_kb_t *kb)
{
	if (kb->binding) {
		wbk_b_free(kb->binding);
		kb->binding = NULL;
	}

	if (kb->cmd) {
		free(kb->cmd);
	}

	free(kb);
	return 0;
}

const wbk_b_t *
wbk_kb_get_binding(const wbk_kb_t *kb)
{
	return kb->binding;
}

const char *
wbk_kb_get_cmd(const wbk_kb_t *kb)
{
	return kb->cmd;
}

DWORD WINAPI
wbk_kbthread_exec(LPVOID param)
{
	char *cmd;

	cmd = (char *) param;

	return system(cmd);
}

int
wbk_kb_exec(const wbk_kb_t *kb)
{
#if defined(WIN32)
	HANDLE thread_handler;

	thread_handler = CreateThread(NULL,
					 0,
					 wbk_kbthread_exec,
					 kb->cmd,
					 0,
					 NULL);
	if (thread_handler) {
		wbk_logger_log(&logger, INFO, "Exec: %s\n", kb->cmd);
	} else {
		wbk_logger_log(&logger, SEVERE, "Exec failed: %s\n", kb->cmd);
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
