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
 * @date 2020-05-17
 * @brief File contains interpreter keyboard daemon class definition
 */

#ifndef WBK_DAEMON_H
#define WBK_DAEMON_H

#include <windows.h>

#include "b.h"

struct wbk_kbdaemon_s;

typedef struct wbk_kbdaemon_s wbk_kbdaemon_t;

typedef struct wbk_kbdaemon_s
{
	HANDLE global_mutex;

	/**
	 * Function is called, when the user holds a any key combination. Make sure
	 * the function executes fast. Otherwise Windows automatically de-registers
	 * the handler and thus breaks your application!
	 *
	 * If the function returns 0, then it overwrites any other application's
	 * behavior for that key combination.
	 */
	int (*exec_fn)(wbk_kbdaemon_t *kbdaemon, wbk_b_t *b);
} wbk_kbdaemon_t;

/**
 */
extern wbk_kbdaemon_t *
wbk_kbdaemon_new(int (*exec_fn)(wbk_kbdaemon_t *kbdaemon, wbk_b_t *b));

extern int
wbk_kbdaemon_free(wbk_kbdaemon_t *kbdaemon);
/**
 * @brief Execute a key binding matching a combination
 * @return Non-0 if the combination was not found.
 */
extern int
wbk_kbdaemon_exec(wbk_kbdaemon_t *kbdaemon, wbk_b_t *b);

/**
 * @brief Main loop, runs forever.
 */
extern int
wbk_kbdaemon_start(wbk_kbdaemon_t *kbdaemon);

extern int
wbk_kbdaemon_stop(wbk_kbdaemon_t *kbdaemon);

/**
 * @param c The result of GetAsyncKeyState (a virtual key code).
 * @return A virtual key code as modifier key
 */
extern wbk_mk_t
wbk_kbdaemon_win32_to_mk(unsigned char c);

/**
 * @param c The result of GetAsyncKeyState (a virtual key code).
 * @return An actual character (e.g. 'a').
 */
extern char
wbk_kbdaemon_win32_to_char(unsigned char c);

#endif // WBK_DAEMON_H
