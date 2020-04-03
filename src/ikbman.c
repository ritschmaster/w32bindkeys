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

#include "ikbman.h"

#include <collectc/array.h>
#include <windows.h>
#include <pthread.h>

#include "logger.h"
#include "kbman.h"

static wbk_logger_t logger =  { "ikbman" };

typedef struct wbki_hookman_s
{
	/**
	 * Array of wbki_kbman_t *
	 */
	Array *kbman_arr;

	pthread_t thread;

	char stop;

	HHOOK hook_id;

	wbk_b_t *cur_b;
} wbki_hookman_t;

static wbki_hookman_t *g_hookman = NULL;

static wbki_hookman_t *
wbki_hookman_create();

static int
wbki_hookman_free(wbki_hookman_t *hookman);

static wbki_hookman_t *
wbki_hookman_get_instance();

/**
 * @param kbman Will not be freed.
 */
static int
wbki_hookman_add_kbman(wbki_hookman_t *hookman, wbki_kbman_t *kbman);

static int
wbki_hookman_remove_kbman(wbki_hookman_t *hookman, wbki_kbman_t *kbman);

/**
 * @brief Runs in its own thread
 * @return 0 if the thread was started.
 */
static int
wbki_hookman_start(wbki_hookman_t *hookman);

/**
 * @brief To be used for pthread_create()
 * @param arg Actually a wbki_hookman_t *
 */
static void *
wbki_hookman_start_fn(void *arg);

static int
wbki_hookman_stop(wbki_hookman_t *hookman);

static LRESULT CALLBACK
wbki_hookman_windows_hook(int nCode, WPARAM wParam, LPARAM lParam);

wbki_hookman_t *
wbki_hookman_create()
{
	wbki_hookman_t *hookman;

	hookman = malloc(sizeof(hookman));

	array_new(&(hookman->kbman_arr));

	memset(&(hookman->thread), 0, sizeof(pthread_t));

	hookman->stop = 1;

	hookman->cur_b = wbk_b_new();

	return hookman;
}

int
wbki_hookman_free(wbki_hookman_t *hookman)
{
	wbki_hookman_stop(hookman);

	array_destroy(hookman->kbman_arr);

	hookman->kbman_arr = NULL;

	wbk_b_free(hookman->cur_b);
	hookman->cur_b = NULL;

	free(hookman);

	return 0;
}

wbki_hookman_t *
wbki_hookman_get_instance()
{
	if (g_hookman == NULL) {
		g_hookman = wbki_hookman_create();
	}

	return g_hookman;
}

int
wbki_hookman_add_kbman(wbki_hookman_t *hookman, wbki_kbman_t *kbman)
{
	return array_add(hookman->kbman_arr, kbman);
}

int
wbki_hookman_remove_kbman(wbki_hookman_t *hookman, wbki_kbman_t *kbman)
{
	return array_remove(hookman->kbman_arr, kbman, NULL);
}

int
wbki_hookman_start(wbki_hookman_t *hookman)
{
	int ret;

	if (hookman->stop) {
		hookman->stop = 0;
		ret = pthread_create(&(hookman->thread), NULL, wbki_hookman_start_fn, hookman);
	} else {
		ret = 1;
	}

	return ret;
}

void *
wbki_hookman_start_fn(void *arg)
{
	wbki_hookman_t *hookman;
	MSG msg;
	HINSTANCE h_instance;

	hookman = (wbki_hookman_t *) arg;

	h_instance = GetModuleHandle(NULL);
	hookman->hook_id = SetWindowsHookExA(WH_KEYBOARD_LL, wbki_hookman_windows_hook, h_instance, 0);

	while (!hookman->stop && GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(hookman->hook_id);
}

int
wbki_hookman_stop(wbki_hookman_t *hookman)
{
	hookman->stop = 1;
	return 0;
}

LRESULT CALLBACK
wbki_hookman_windows_hook(int nCode, WPARAM wParam, LPARAM lParam)
{
	int ret;
	wbki_hookman_t *hookman;
	KBDLLHOOKSTRUCT *hookstruct;
	ArrayIter iter;
	wbki_kbman_t *kbman;
	wbk_be_t be;
	int changed_any;

	ret = 0;
	if (nCode >= 0) {
		hookstruct = (KBDLLHOOKSTRUCT *)lParam;

		hookman = wbki_hookman_get_instance();
		changed_any = 0;

		switch (wParam) {
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
				be.modifier = wbki_kbman_win32_to_mk(hookstruct->vkCode);
				be.key = wbki_kbman_win32_to_char(hookstruct->vkCode);
				if (wbk_b_add(hookman->cur_b, &be) == 0) {
					changed_any = 1;
				}
				break;

			case WM_KEYUP:
			case WM_SYSKEYUP:
				be.modifier = wbki_kbman_win32_to_mk(hookstruct->vkCode);
				be.key = wbki_kbman_win32_to_char(hookstruct->vkCode);
				if (wbk_b_remove(hookman->cur_b, &be) == 0) {
					changed_any = 1;
				}
				break;
		}

		if (changed_any) {
			array_iter_init(&iter, hookman->kbman_arr);
			while (array_iter_next(&iter, (void *) &kbman) != CC_ITER_END) {
				if (wbki_kbman_exec(kbman, hookman->cur_b) == 0) {
					ret = 1;
				}
			}
		}
	}

	if (!ret) {
		ret = CallNextHookEx(NULL, nCode, wParam, lParam);
	}

	return ret;
}

wbki_kbman_t *
wbki_kbman_new()
{
	wbki_kbman_t *kbman;

	kbman = NULL;
	kbman = malloc(sizeof(wbki_kbman_t));

	array_new(&(kbman->kc_sys_arr));

	return kbman;

}

wbki_kbman_t *
wbki_kbman_free(wbki_kbman_t *kbman)
{
	ArrayIter kb_iter;
	wbk_kc_sys_t *kc_sys;

	array_iter_init(&kb_iter, wbki_kbman_get_kb(kbman));
	while (array_iter_next(&kb_iter, (void *) &kc_sys) != CC_ITER_END) {
		array_iter_remove(&kb_iter, NULL);
		wbk_kc_sys_free(kc_sys);
	}
	array_destroy_cb(kbman->kc_sys_arr, free);
	kbman->kc_sys_arr = NULL;

	free(kbman);
}

Array *
wbki_kbman_get_kb(wbki_kbman_t* kbman)
{
	return kbman->kc_sys_arr;
}

int
wbki_kbman_add(wbki_kbman_t *kbman, wbk_kc_sys_t *kc_sys)
{
	array_add(kbman->kc_sys_arr, kc_sys);
}

int
wbki_kbman_exec(wbki_kbman_t *kbman, wbk_b_t *b)
{
	int ret;
	char found;
	ArrayIter kb_iter;
	wbk_kc_sys_t *kc_sys;

	ret = -1;

	found = 0;
	array_iter_init(&kb_iter, wbki_kbman_get_kb(kbman));
	while (!found && array_iter_next(&kb_iter, (void *) &kc_sys) != CC_ITER_END) {
		if (wbk_b_compare(wbk_kc_sys_get_binding(kc_sys), b) == 0) {
			found = 1;
		}
	}

	if (found) {
		wbk_kc_sys_exec(kc_sys);
		ret = 0;
	}

	return ret;

}

int
wbki_kbman_main(wbki_kbman_t *kbman)
{
	wbki_hookman_t *hookman;

	hookman = wbki_hookman_get_instance();
	wbki_kbman_stop(kbman);
	wbki_hookman_add_kbman(hookman, kbman);
	wbki_hookman_start(hookman);

	return 0;
}

int
wbki_kbman_stop(wbki_kbman_t *kbman)
{
	wbki_hookman_t *hookman;

	hookman = wbki_hookman_get_instance();
	wbki_hookman_remove_kbman(hookman, kbman);

	return 0;
}

wbk_mk_t
wbki_kbman_win32_to_mk(unsigned char c)
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
	else if(c == 91 || c == 92)   modifier = WIN;
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
wbki_kbman_win32_to_char(unsigned char c)
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
