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

static int g_kbdaemon_arr_len = 0;
static wbk_kbdaemon_t **g_kbdaemon_arr = NULL;
static HHOOK g_hook_id = NULL;
static wbk_b_t *g_cur_b = NULL;

/**
 * Starts tracking keyboard activity by adding a low level keyboard hook to
 * Windows.
 *
 * @return 0 if the tracking could be started.
 */
static int
wbk_kbhook_start();

/**
 * Stops tracking keyboard activity by removing the low level keyboard hook from
 * Windows.
 *
 * @return 0 if the tracking could be stopped.
 */
static int
wbk_kbhook_stop();

/**
 * The low level keyboard hook started and stopped by wbk_kbhook_start()  and
 * wbk_kbhook_stop().
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * @param kbdaemon Will not be freed.
 * @return 0 if the add was successful.
 */
static int
wbk_kbhook_add_kbdaemon(wbk_kbdaemon_t *kbdaemon);

/**
 * @param kbdaemon Will not be freed.
 * @return 0 if the removal was successful.
 */
static int
wbk_kbhook_remove_kbdaemon(wbk_kbdaemon_t *kbdaemon);


int
wbk_kbhook_add_kbdaemon(wbk_kbdaemon_t *kbdaemon)
{
	int arr_len;

	arr_len = g_kbdaemon_arr_len;
	g_kbdaemon_arr_len = 0;
	arr_len++;

	g_kbdaemon_arr = realloc(g_kbdaemon_arr, sizeof(wbk_kbdaemon_t **) * arr_len);
	g_kbdaemon_arr[arr_len - 1] = kbdaemon;

	g_kbdaemon_arr_len = arr_len;

	return 0;
}

int
wbk_kbhook_remove_kbdaemon(wbk_kbdaemon_t *kbdaemon)
{
	int error;
	int arr_len;
	int i;

	error = 1;

	arr_len = g_kbdaemon_arr_len;
	g_kbdaemon_arr_len = 0;

	error = 1;
	for (int i = 0; i < arr_len; i++) {
		if (g_kbdaemon_arr[i] == kbdaemon) {
			g_kbdaemon_arr[i] = NULL;
			error = 0;
		}
	}

	g_kbdaemon_arr_len = arr_len;

	return error;
}

int
wbk_kbhook_start()
{
	int error;
	HINSTANCE h_instance;

	error = 0;

	if (g_cur_b == NULL) {
		g_cur_b = wbk_b_new();
	}

	if (g_hook_id == NULL) {
		h_instance = GetModuleHandle(NULL);
		g_hook_id = SetWindowsHookExA(WH_KEYBOARD_LL, wbk_kbhook_windows_hook, h_instance, 0);
	}

	return error;
}

int
wbk_kbhook_stop()
{
	if (g_hook_id) {
		UnhookWindowsHookEx(g_hook_id);
	}

	wbk_b_free(g_cur_b);
	g_cur_b = NULL;

	return 0;
}

LRESULT CALLBACK
wbk_kbhook_windows_hook(int nCode, WPARAM wParam, LPARAM lParam)
{
	int ret;
	KBDLLHOOKSTRUCT *hookstruct;
	wbk_be_t be;
	int changed_any;
	int i;

	ret = 0;
	if (nCode >= 0) {
		switch (wParam) {
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			hookstruct = (KBDLLHOOKSTRUCT *)lParam;

			changed_any = 0;

			be.modifier = wbk_kbdaemon_win32_to_mk(hookstruct->vkCode);
			be.key = wbk_kbdaemon_win32_to_char(hookstruct->vkCode);

			switch (wParam) {
				case WM_KEYDOWN:
				case WM_SYSKEYDOWN:
					if (wbk_b_add(g_cur_b, &be) == 0) {
						changed_any = 1;
					}
					break;

				case WM_KEYUP:
				case WM_SYSKEYUP:
					be.modifier = wbk_kbdaemon_win32_to_mk(hookstruct->vkCode);
					be.key = wbk_kbdaemon_win32_to_char(hookstruct->vkCode);
					if (wbk_b_remove(g_cur_b, &be) == 0) {
						changed_any = 1;
					}
					break;
			}

			if (changed_any) {
				for (i = 0; i < g_kbdaemon_arr_len; i++) {
					if (g_kbdaemon_arr[i]
						&& wbk_kbdaemon_exec(g_kbdaemon_arr[i], g_cur_b) == 0) {
						ret = 1;
					}
				}
			}
		}
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

inline int
wbk_kbdaemon_exec(wbk_kbdaemon_t *kbdaemon, wbk_b_t *b)
{
	return kbdaemon->exec_fn(b);
}

int
wbk_kbdaemon_start(wbk_kbdaemon_t *kbdaemon)
{
	WaitForSingleObject(kbdaemon->global_mutex, INFINITE);

	wbk_kbdaemon_stop(kbdaemon);
	wbk_kbhook_add_kbdaemon(kbdaemon);
	wbk_kbhook_start();

	ReleaseMutex(kbdaemon->global_mutex);

	return 0;
}

int
wbk_kbdaemon_stop(wbk_kbdaemon_t *kbdaemon)
{
	WaitForSingleObject(kbdaemon->global_mutex, INFINITE);

	wbk_kbhook_remove_kbdaemon(kbdaemon);

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
