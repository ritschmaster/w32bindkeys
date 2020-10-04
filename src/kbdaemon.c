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
#include <wtsapi32.h>

#include "logger.h"
#include "kbdaemon.h"

static wbk_logger_t logger =  { "kbdaemon" };

struct kbhook_s
{
	int arr_len;
	wbk_kbdaemon_t **arr;
	HHOOK hook_id;
	LRESULT CALLBACK (*hook_fn)(int , WPARAM , LPARAM );
	wbk_b_t *cur_b;
};

typedef struct kbhook_s *kbhook_t;

static int g_hook_entered = 0;
static int g_hook_left = 0;

/**
 * Reset all currently tracked pressed keys.
 */
static int
wbk_kbhook_reset_all_b(void);

/**
 * Track the current session and reset the tracked pressed keys if the session
 * changes (e.g. the user locks the current session).
 */
static LRESULT CALLBACK
wbk_kbhook_session_watcher_wnd_proc(HWND window_handler, UINT msg, WPARAM wParam, LPARAM lParam);

/**
 * Start the tracking of the current session.
 */
static int
wbk_kbhook_session_watcher_start(void);

/**
 * Watches if actually some keys are currently not pressed at all. If no key is
 * pressed then the internal key binding objects are reset. This is necessary to
 * avoid the breakage of w32bindkeys' low level keyboard hook by other low level
 * keyboard hooks or by a heavy load on the system.
 */
static DWORD WINAPI
wbk_kbhook_hook_watcher(LPVOID param);

/**
 * Starts the watcher to the low level keyboard hook.
 */
static int
wbk_kbhook_hook_watcher_start(void);

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
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 0th global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook0(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 1st global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook1(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 2nd global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook2(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * Internal function to change an element of the global data.
 *
 * @param kbdaemon Will not be freed.
 * @param g_kbdaemon_arr_len Will not be freed.
 * @param g_kbdaemon_arr Will not be freed.
 * @return 0 if the add was successful.
 */
static int
wbk_kbhook_add_kbdaemon_internal(wbk_kbdaemon_t *kbdaemon,
								 int *g_kbdaemon_arr_len,
								 wbk_kbdaemon_t ***g_kbdaemon_arr);

/**
 * Use this function to add a kbdaemon.
 *
 * @param kbdaemon Will not be freed.
 * @return 0 if the add was successful.
 */
static int
wbk_kbhook_add_kbdaemon(wbk_kbdaemon_t *kbdaemon);

/**
 * Internal function to change an element of the global data.
 *
 * @param kbdaemon Will not be freed.
 * @param g_kbdaemon_arr_len Will not be freed.
 * @param g_kbdaemon_arr Will not be freed.
 * @return 0 if the removal was successful.
 */
static int
wbk_kbhook_remove_kbdaemon_internal(wbk_kbdaemon_t *kbdaemon,
									int *g_kbdaemon_arr_len,
									wbk_kbdaemon_t ***g_kbdaemon_arr);

/**
 * Use this function to remove a kbdaemon.
 *
 * @param kbdaemon Will not be freed.
 * @return 0 if the removal was successful.
 */
static int
wbk_kbhook_remove_kbdaemon(wbk_kbdaemon_t *kbdaemon);

static int g_kbhook_arr_len = 3;
static int g_kbhook_arr_i = 0;
static struct kbhook_s g_kbhook_arr[] = {
		{ 0, NULL, NULL, wbk_kbhook_windows_hook0, NULL },
		{ 0, NULL, NULL, wbk_kbhook_windows_hook1, NULL },
		{ 0, NULL, NULL, wbk_kbhook_windows_hook2, NULL }
};

static char g_session_watcher_created = 0;

static HANDLE g_active_win_watcher_handler = NULL;

int
wbk_kbhook_reset_all_b(void)
{
	wbk_logger_log(&logger, DEBUG, "Reseting all tracked pressed keys\n");

	wbk_b_reset(g_kbhook_arr[0].cur_b);
	wbk_b_reset(g_kbhook_arr[1].cur_b);
	wbk_b_reset(g_kbhook_arr[2].cur_b);

	return 0;
}

LRESULT CALLBACK
wbk_kbhook_session_watcher_wnd_proc(HWND window_handler, UINT msg, WPARAM wParam, LPARAM lParam)
{
	char reset;

	reset = 0;
	switch(msg) {
	case WM_WTSSESSION_CHANGE:
		reset = 1;
	}

	if (reset) {
		wbk_kbhook_reset_all_b();
	}

	return DefWindowProc(window_handler, msg, wParam, lParam);
}


int
wbk_kbhook_session_watcher_start(void)
{
	HINSTANCE hInstance;
	WNDCLASSEX wc;
	HWND window_handler;
	int error;
	char classname[] = "w32bindkeys session watcher";

	error = 0;

	if (!error) {
		hInstance = GetModuleHandle(NULL);

		wc.cbSize		= sizeof(WNDCLASSEX);
		wc.style		 = 0;
		wc.lpfnWndProc   = wbk_kbhook_session_watcher_wnd_proc;
		wc.cbClsExtra	= 0;
		wc.cbWndExtra	= 0;
		wc.hInstance	 = hInstance;
		wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor	   = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = classname;
		wc.hIconSm	   = LoadIcon(NULL, IDI_APPLICATION);

		if(!RegisterClassEx(&wc)) {
			error = 1;
		}
	}

	if (!error) {
		window_handler = CreateWindowEx(WS_EX_NOACTIVATE,
						classname,
						classname,
						WS_DISABLED,
						0, 0,
						0, 0,
						NULL, NULL, hInstance, NULL);

		ShowWindow(window_handler, SW_HIDE);

		WTSRegisterSessionNotification(window_handler, NOTIFY_FOR_THIS_SESSION);
	}

	return error;
}

DWORD WINAPI
wbk_kbhook_hook_watcher(LPVOID param)
{
	char reset;
	char c;
	SHORT rv;
	int i;

	while (1) {
		reset = 0;

		if (!reset) {
			reset = 1;
			for(c = 1; reset && c < 255; c++){
				rv = GetAsyncKeyState(c);
				if (rv & 0x8000) {
					/**
					 * A key is currently pressed
					 */
					reset = 0;
				}
			}
		}

		if (!reset) {
			/**
			 * Give the user i changes to stop typing
			 */
			for (i = 0; i < 5; i++) {
				reset = 0;
				if (g_hook_entered != g_hook_left) {
					reset = 1;
				}
				Sleep(5);
			}
		}

		if (reset) {
			wbk_kbhook_reset_all_b();
			g_hook_entered = 0;
			g_hook_left = 0;
		}
	}

	return 0;
}

int
wbk_kbhook_hook_watcher_start(void)
{
	int error;

	error = 0;
	g_active_win_watcher_handler = CreateThread(NULL,
								   0,
								   wbk_kbhook_hook_watcher,
								   NULL,
								   0,
								   NULL);

	if (!g_active_win_watcher_handler) {
		error = 1;
		wbk_logger_log(&logger, SEVERE, "Starting of the active window watcher thread failed.\n");
	}

	return error;
}


int
wbk_kbhook_add_kbdaemon_internal(wbk_kbdaemon_t *kbdaemon,
								 int *g_kbdaemon_arr_len,
								 wbk_kbdaemon_t ***g_kbdaemon_arr)
{
	int arr_len;

	arr_len = *g_kbdaemon_arr_len;

	/**
	 * Act like the array is empty to not break any concurrently running thread
	 */
	*g_kbdaemon_arr_len = 0;

	arr_len++;

	*g_kbdaemon_arr = realloc(*g_kbdaemon_arr, sizeof(wbk_kbdaemon_t **) * arr_len);
	(*g_kbdaemon_arr)[arr_len - 1] = kbdaemon;

	/**
	 * Increment the array length after everything is done to not break any concurrently running thread
	 */
	*g_kbdaemon_arr_len = arr_len;

	return 0;
}

int
wbk_kbhook_add_kbdaemon(wbk_kbdaemon_t *kbdaemon)
{
	int error;

	error = wbk_kbhook_add_kbdaemon_internal(kbdaemon, &(g_kbhook_arr[g_kbhook_arr_i].arr_len), &(g_kbhook_arr[g_kbhook_arr_i].arr));
	g_kbhook_arr_i++;
	if (g_kbhook_arr_i >= g_kbhook_arr_len) {
		g_kbhook_arr_i = 0;
	}

	return error;
}

int
wbk_kbhook_remove_kbdaemon_internal(wbk_kbdaemon_t *kbdaemon,
									int *g_kbdaemon_arr_len,
									wbk_kbdaemon_t ***g_kbdaemon_arr)
{
	int error;
	int arr_len;
	int i;

	error = 1;

	arr_len = *g_kbdaemon_arr_len;

	/**
	 * Act like the array is empty to not break any concurrently running thread
	 */
	*g_kbdaemon_arr_len = 0;

	error = 1;
	for (int i = 0; i < arr_len; i++) {
		if ((*g_kbdaemon_arr)[i] == kbdaemon) {
			(*g_kbdaemon_arr)[i] = NULL;
			error = 0;
		}
	}

	/**
	 * Decrement the array length after everything is done to not break any concurrently running thread
	 */
	*g_kbdaemon_arr_len = arr_len;

	return error;
}

int
wbk_kbhook_remove_kbdaemon(wbk_kbdaemon_t *kbdaemon)
{
	int error;
	int i;

	error = 1;

	for (i = 0; error && i < g_kbhook_arr_len; i++) {
		error = wbk_kbhook_remove_kbdaemon_internal(kbdaemon, &(g_kbhook_arr[i].arr_len), &(g_kbhook_arr[i].arr));
	}

	return error;
}

int
wbk_kbhook_start()
{
	int error;
	int i;
	HINSTANCE h_instance;

	error = 0;

	for (i = 0; i < g_kbhook_arr_len; i++) {
		if (g_kbhook_arr[i].cur_b == NULL) {
			g_kbhook_arr[i].cur_b = wbk_b_new();
		}

		if (g_kbhook_arr[i].hook_id == NULL) {
			h_instance = GetModuleHandle(NULL);
			g_kbhook_arr[i].hook_id = SetWindowsHookExA(WH_KEYBOARD_LL, g_kbhook_arr[i].hook_fn, h_instance, 0);
		}
	}

	return error;
}

int
wbk_kbhook_stop()
{
	int i;

	for (i = 0; i < g_kbhook_arr_len; i++) {
		if (g_kbhook_arr[i].hook_id) {
			UnhookWindowsHookEx(g_kbhook_arr[i].hook_id);
			g_kbhook_arr[i].hook_id = NULL;
		}

		wbk_b_free(g_kbhook_arr[i].cur_b);
		g_kbhook_arr[i].cur_b = NULL;
	}

	return 0;
}

static LRESULT CALLBACK
wbk_kbhook_windows(int nCode, WPARAM wParam, LPARAM lParam,
				   int *g_kbdaemon_arr_len,
				   wbk_kbdaemon_t ***g_kbdaemon_arr,
				   wbk_b_t **g_cur_b)
{
	int ret;
	KBDLLHOOKSTRUCT *hookstruct;
	wbk_be_t be;
	int changed_any;
	int i;

	g_hook_entered++;
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
					if (wbk_b_add(*g_cur_b, &be) == 0) {
						changed_any = 1;
					}
					break;

				case WM_KEYUP:
				case WM_SYSKEYUP:
					be.modifier = wbk_kbdaemon_win32_to_mk(hookstruct->vkCode);
					be.key = wbk_kbdaemon_win32_to_char(hookstruct->vkCode);
					if (wbk_b_remove(*g_cur_b, &be) == 0) {
						changed_any = 1;
					}
					break;
			}

			if (changed_any) {
				for (i = 0; i < *g_kbdaemon_arr_len; i++) {
					if ((*g_kbdaemon_arr)[i]
						&& wbk_kbdaemon_exec((*g_kbdaemon_arr)[i], *g_cur_b) == 0) {
						ret = 1;
					}
				}
			}
		}
	}

	g_hook_left++;
	if (!ret) {
		ret = CallNextHookEx(NULL, nCode, wParam, lParam);
	}

	return ret;
}

LRESULT CALLBACK
wbk_kbhook_windows_hook0(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
						   &(g_kbhook_arr[0].arr_len),
						   &(g_kbhook_arr[0].arr),
						   &(g_kbhook_arr[0].cur_b));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook1(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
						   &(g_kbhook_arr[1].arr_len),
						   &(g_kbhook_arr[1].arr),
						   &(g_kbhook_arr[1].cur_b));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook2(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
						   &(g_kbhook_arr[2].arr_len),
						   &(g_kbhook_arr[2].arr),
						   &(g_kbhook_arr[2].cur_b));
}

wbk_kbdaemon_t *
wbk_kbdaemon_new(int (*exec_fn)(wbk_kbdaemon_t *kbdaemon, wbk_b_t *b))
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
	return kbdaemon->exec_fn(kbdaemon, b);
}

int
wbk_kbdaemon_start(wbk_kbdaemon_t *kbdaemon)
{
	WaitForSingleObject(kbdaemon->global_mutex, INFINITE);

	if (!g_session_watcher_created) {
		wbk_kbhook_session_watcher_start();
		g_session_watcher_created = 1;
	}


	if (!g_active_win_watcher_handler) {
		wbk_kbhook_hook_watcher_start();
	}

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
