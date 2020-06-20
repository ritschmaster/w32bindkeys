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
 * @brief File contains interpreter keyboard daemon class implementation and
 * private methods
 */

#include <windows.h>

#include "logger.h"
#include "kbdaemon.h"

static wbk_logger_t logger =  { "kbdaemon" };

typedef struct wbk_hookman_s
{
	HANDLE global_mutex;

	int kbdaemon_arr_len;
	wbk_kbdaemon_t **kbdaemon_arr;

	char stop;

	HHOOK hook_id;

	wbk_b_t *cur_b;
} wbk_hookman_t;

static wbk_hookman_t *g_hookman = NULL;

static wbk_hookman_t *
wbk_hookman_new();

static int
wbk_hookman_free(wbk_hookman_t *hookman);

static wbk_hookman_t *
wbk_hookman_get_instance();

/**
 * @param kbdaemon Will not be freed.
 */
static int
wbk_hookman_add_kbdaemon(wbk_hookman_t *hookman, wbk_kbdaemon_t *kbdaemon);

static int
wbk_hookman_remove_kbdaemon(wbk_hookman_t *hookman, wbk_kbdaemon_t *kbdaemon);

/**
 * @brief Adds a handler to the Windows message loop
 * @return 0 if the thread was started.
 */
static int
wbk_hookman_start(wbk_hookman_t *hookman);

static int
wbk_hookman_stop(wbk_hookman_t *hookman);

static LRESULT CALLBACK
wbk_hookman_windows_hook(int nCode, WPARAM wParam, LPARAM lParam);

wbk_hookman_t *
wbk_hookman_new()
{
	wbk_hookman_t *hookman;

	hookman = malloc(sizeof(hookman));

	hookman->global_mutex = CreateMutex(NULL, FALSE, NULL);

	hookman->kbdaemon_arr_len = 0;
	hookman->kbdaemon_arr = NULL;

	hookman->stop = 1;

	hookman->cur_b = wbk_b_new();

	return hookman;
}

int
wbk_hookman_free(wbk_hookman_t *hookman)
{
	int i;

	wbk_hookman_stop(hookman);

	ReleaseMutex(hookman->global_mutex);
	CloseHandle(hookman->global_mutex);

	for (i = 0; i < hookman->kbdaemon_arr_len; i++) {
		hookman->kbdaemon_arr[i] = NULL;
	}
	free(hookman->kbdaemon_arr);
	hookman->kbdaemon_arr = NULL;

	wbk_b_free(hookman->cur_b);
	hookman->cur_b = NULL;

	free(hookman);

	return 0;
}

wbk_hookman_t *
wbk_hookman_get_instance()
{
	if (g_hookman == NULL) {
		g_hookman = wbk_hookman_new();
	}

	return g_hookman;
}

int
wbk_hookman_add_kbdaemon(wbk_hookman_t *hookman, wbk_kbdaemon_t *kbdaemon)
{
	WaitForSingleObject(hookman->global_mutex, INFINITE);

	hookman->kbdaemon_arr_len++;
	hookman->kbdaemon_arr = realloc(hookman->kbdaemon_arr,
									sizeof(wbk_kbdaemon_t **) * hookman->kbdaemon_arr_len);
	hookman->kbdaemon_arr[hookman->kbdaemon_arr_len - 1] = kbdaemon;

	ReleaseMutex(hookman->global_mutex);

	return 0;
}

int
wbk_hookman_remove_kbdaemon(wbk_hookman_t *hookman, wbk_kbdaemon_t *kbdaemon)
{
	int i;
	int error;

	WaitForSingleObject(hookman->global_mutex, INFINITE);

	error = 1;
	for (int i = 0; i < hookman->kbdaemon_arr_len; i++) {
		if (hookman->kbdaemon_arr[i] == kbdaemon) {
			hookman->kbdaemon_arr[i] = NULL;
			error = 0;
		}
	}

	ReleaseMutex(hookman->global_mutex);

	return error;
}

int
wbk_hookman_start(wbk_hookman_t *hookman)
{
	HINSTANCE h_instance;
	int ret;

	WaitForSingleObject(hookman->global_mutex, INFINITE);

	if (hookman->stop) {
		hookman->stop = 0;
		h_instance = GetModuleHandle(NULL);
		hookman->hook_id = SetWindowsHookExA(WH_KEYBOARD_LL, wbk_hookman_windows_hook, h_instance, 0);
	} else {
		ret = 1;
	}

	ReleaseMutex(hookman->global_mutex);

	return ret;
}

int
wbk_hookman_stop(wbk_hookman_t *hookman)
{
	WaitForSingleObject(hookman->global_mutex, INFINITE);

	if (!hookman->stop) {
		UnhookWindowsHookEx(hookman->hook_id);
	}

	hookman->stop = 1;

	ReleaseMutex(hookman->global_mutex);

	return 0;
}

LRESULT CALLBACK
wbk_hookman_windows_hook(int nCode, WPARAM wParam, LPARAM lParam)
{
	int ret;
	wbk_hookman_t *hookman;
	KBDLLHOOKSTRUCT *hookstruct;
	wbk_be_t be;
	int changed_any;
	int i;

	ret = 0;
	if (nCode >= 0) {
		hookstruct = (KBDLLHOOKSTRUCT *)lParam;

		hookman = wbk_hookman_get_instance();
		changed_any = 0;

		WaitForSingleObject(hookman->global_mutex, INFINITE);

		switch (wParam) {
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
				be.modifier = wbk_kbdaemon_win32_to_mk(hookstruct->vkCode);
				be.key = wbk_kbdaemon_win32_to_char(hookstruct->vkCode);
				if (wbk_b_add(hookman->cur_b, &be) == 0) {
					changed_any = 1;
				}
				break;

			case WM_KEYUP:
			case WM_SYSKEYUP:
				be.modifier = wbk_kbdaemon_win32_to_mk(hookstruct->vkCode);
				be.key = wbk_kbdaemon_win32_to_char(hookstruct->vkCode);
				if (wbk_b_remove(hookman->cur_b, &be) == 0) {
					changed_any = 1;
				}
				break;
		}

		if (changed_any) {
			for (i = 0; i < hookman->kbdaemon_arr_len; i++) {
				if (hookman->kbdaemon_arr[i]
					&& wbk_kbdaemon_exec(hookman->kbdaemon_arr[i], hookman->cur_b) == 0) {
					ret = 1;
				}
			}
		}

		ReleaseMutex(hookman->global_mutex);
	}

	if (!ret) {
		ret = CallNextHookEx(NULL, nCode, wParam, lParam);
	}

	return ret;
}

wbk_kbdaemon_t *
wbk_kbdaemon_new(int (*exec_fn)(wbk_b_t *b))
{
	wbk_kbdaemon_t *kbdaemon;

	wbk_hookman_get_instance();

	kbdaemon = NULL;
	kbdaemon = malloc(sizeof(wbk_kbdaemon_t));

	kbdaemon->global_mutex = CreateMutex(NULL, FALSE, NULL);

	kbdaemon->exec_fn = exec_fn;

	return kbdaemon;

}

int
wbk_kbdaemon_free(wbk_kbdaemon_t *kbdaemon)
{
	wbk_kbdaemon_stop(kbdaemon);

	ReleaseMutex(kbdaemon->global_mutex);
	CloseHandle(kbdaemon->global_mutex);

	kbdaemon->exec_fn = NULL;

	free(kbdaemon);

	return 0;
}

int
wbk_kbdaemon_exec(wbk_kbdaemon_t *kbdaemon, wbk_b_t *b)
{
	return kbdaemon->exec_fn(b);
}

int
wbk_kbdaemon_start(wbk_kbdaemon_t *kbdaemon)
{
	wbk_hookman_t *hookman;

	WaitForSingleObject(kbdaemon->global_mutex, INFINITE);

	hookman = wbk_hookman_get_instance();
	wbk_kbdaemon_stop(kbdaemon);
	wbk_hookman_add_kbdaemon(hookman, kbdaemon);
	wbk_hookman_start(hookman);

	ReleaseMutex(kbdaemon->global_mutex);

	return 0;
}

int
wbk_kbdaemon_stop(wbk_kbdaemon_t *kbdaemon)
{
	wbk_hookman_t *hookman;

	WaitForSingleObject(kbdaemon->global_mutex, INFINITE);

	hookman = wbk_hookman_get_instance();
	wbk_hookman_remove_kbdaemon(hookman, kbdaemon);

	ReleaseMutex(kbdaemon->global_mutex);

	return 0;
}

wbk_mk_t
wbk_kbdaemon_win32_to_mk(unsigned char c)
{
	wbk_mk_t modifier;

	modifier = NOT_A_MODIFIER;

	if (c == 13)
		modifier = ENTER;
	else if (c == 16 || c == 160 || c == 161)
		modifier = SHIFT;
	else if (c == 17 || c == 162 || c == 163)
		modifier = CTRL;
	else if (c == 18 || c == 164 || c == 165)
		modifier = ALT;
	else if (c == VK_SPACE) modifier = SPACE;
	else if (c == 91 || c == 92)  modifier = WIN;
	else if (c == 112) modifier = F1;
	else if (c == 113) modifier = F2;
	else if (c == 114) modifier = F3;
	else if (c == 115) modifier = F4;
	else if (c == 116) modifier = F5;
	else if (c == 117) modifier = F6;
	else if (c == 118) modifier = F7;
	else if (c == 119) modifier = F8;
	else if (c == 120) modifier = F9;
	else if (c == 121) modifier = F10;
	else if (c == 122) modifier = F11;
	else if (c == 123) modifier = F12;

	return modifier;
}


char
wbk_kbdaemon_win32_to_char(unsigned char c)
{
	char key;

	key = 0;

	if(c == 186)
		c = '+';
	else if(c == 188)
		c = ',';
	else if(c == 189)
		c = '-';
	else if(c == 190)
		c = '.';
	else if(c == 191)
		c = '#';
	else if(c == 226)
		c = '<';
	else if (c >= 48 && c <= 90
			 || c == 32)
		key = tolower(c);

	return key;
}
