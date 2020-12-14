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
 * @date 2020-02-26
 * @brief File contains the key binding system command class implementation and private methods
 */

#include "kc_sys.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <windows.h>

#include "logger.h"

static wbk_logger_t logger =  { "kc_sys" };

/**
 * Implementation of wbk_kc_clone().
 *
 * Clones a key binding system command
 */
static wbk_kc_t *
wbk_kc_sys_clone_impl(const wbk_kc_t *super_other);

/**
 * Implementaiton of wbk_kc_free().
 *
 * @brief Frees a key binding system command
 * @return Non-0 if the freeing failed
 */
static int
wbk_kc_sys_free_impl(wbk_kc_t *kc);

/**
 * Implementation of wbk_kc_sys_get_cmd().
 *
 */
static const char *
wbk_kc_sys_get_cmd_impl(const wbk_kc_sys_t *kc_sys);

/**
 * Implementation of wbk_kc_exec().
 *
 * @brief Execute the system command of a key binding system command
 * @return Non-0 if the execution failed
 */
static int
wbk_kc_sys_exec_impl(const wbk_kc_t *kc);

static DWORD WINAPI
wbk_kbthread_exec(LPVOID param);

wbk_kc_sys_t *
wbk_kc_sys_new(wbk_b_t *comb, char *cmd)
{
  wbk_kc_t *kc;
	wbk_kc_sys_t *kc_sys;
	int length;

	kc_sys = NULL;
	kc_sys = malloc(sizeof(wbk_kc_sys_t));
	memset(kc_sys, 0, sizeof(wbk_kc_sys_t));

  if (kc_sys) {
    kc = wbk_kc_new(comb);
    memcpy(kc_sys, kc, sizeof(wbk_kc_t));
    free(kc); /* Just free the top level element */

    kc_sys->super_kc_clone = kc_sys->kc.kc_clone;
    kc_sys->super_kc_free = kc_sys->kc.kc_free;
    kc_sys->super_kc_exec = kc_sys->kc.kc_exec;

    kc_sys->kc.kc_clone = wbk_kc_sys_clone_impl;
    kc_sys->kc.kc_free = wbk_kc_sys_free_impl;
    kc_sys->kc.kc_exec = wbk_kc_sys_exec_impl;
    kc_sys->kc_sys_get_cmd = wbk_kc_sys_get_cmd_impl;
  }

	if (kc_sys) {
		kc_sys->cmd = cmd;
	}

	return kc_sys;
}

wbk_kc_t *
wbk_kc_sys_clone_impl(const wbk_kc_t *super_other)
{
	const wbk_kc_sys_t *other;
  wbk_b_t *comb;
	char *cmd;
	int cmd_len;
	wbk_kc_sys_t *kc_sys;

  other = (const wbk_kc_sys_t *) super_other;

	kc_sys = NULL;
	if (other) {
		comb = wbk_b_clone(wbk_kc_get_binding((wbk_kc_t *) other));

		cmd_len = strlen(wbk_kc_sys_get_cmd(other)) + 1;
		cmd = malloc(sizeof(char) * cmd_len);
		memcpy(cmd, wbk_kc_sys_get_cmd(other), sizeof(char) * cmd_len);

		kc_sys = wbk_kc_sys_new(comb, cmd);
	}

	return (wbk_kc_t *) kc_sys;
}

const char *
wbk_kc_sys_get_cmd(const wbk_kc_sys_t *kc_sys)
{
	return kc_sys->kc_sys_get_cmd(kc_sys);
}


int
wbk_kc_sys_free_impl(wbk_kc_t *kc)
{
  wbk_kc_sys_t *kc_sys;

  kc_sys = (wbk_kc_sys_t *) kc;

  free(kc_sys->cmd);
	kc_sys->cmd = NULL;

  kc_sys->super_kc_free(kc);

	return 0;
}

const char *
wbk_kc_sys_get_cmd_impl(const wbk_kc_sys_t *kc_sys)
{
	return kc_sys->cmd;
}

DWORD WINAPI
wbk_kbthread_exec(LPVOID param)
{
	char *cmd;

	cmd = (char *) param;

	return system(cmd);
}

int
wbk_kc_sys_exec_impl(const wbk_kc_t *kc)
{
  const wbk_kc_sys_t *kc_sys;

  kc_sys = (const wbk_kc_sys_t *) kc;
#ifdef DEBUG_ENABLED
	char *binding;

	binding = wbk_b_to_str(wbk_kc_get_binding(kc));
	wbk_logger_log(&logger, DEBUG, "Exec binding: %s\n", binding);
	free(binding);
	binding = NULL;
#endif

	HANDLE thread_handler;

	thread_handler = CreateThread(NULL,
					 0,
					 wbk_kbthread_exec,
					 kc_sys->cmd,
					 0,
					 NULL);
	if (thread_handler) {
		wbk_logger_log(&logger, INFO, "Exec: %s\n", kc_sys->cmd);
	} else {
		wbk_logger_log(&logger, SEVERE, "Exec failed: %s\n", kc_sys->cmd);
	}

	return 0;
}

