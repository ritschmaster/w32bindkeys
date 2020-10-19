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

typedef struct kbhook_s
{
	int arr_len;
	wbk_kbdaemon_t **arr;
	HHOOK hook_id;
	LRESULT CALLBACK (*hook_fn)(int , WPARAM , LPARAM );
	wbk_b_t *cur_b;
	int hook_entered;
	int hook_left;
} kbhook_t;

/**
 * Reset all currently tracked pressed keys.
 */
static int
wbk_kbhook_reset_all_b(void);

static int
wbk_kbhook_any_hook_broken(void);

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
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 3rd global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook3(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 4th global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook4(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 5th global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook5(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 6th global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook6(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 7th global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook7(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 8th global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook8(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 9th global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook9(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 10th global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook10(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 11th global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook11(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 12th global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook12(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 13th global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook13(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 14th global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook14(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 15th global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook15(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 16th global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook16(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 17th global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook17(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 18th global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook18(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 19th global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook19(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 20th global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook20(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 21st global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook21(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 22nd global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook22(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 23rd global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook23(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 24th global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook24(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 25th global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook25(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 26th global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook26(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 27th global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook27(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 28th global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook28(int nCode, WPARAM wParam, LPARAM lParam);

/**
 * A low level keyboard hook started and stopped by wbk_kbhook_start() and
 * wbk_kbhook_stop(). It is used for the 29th global element.
 */
static LRESULT CALLBACK
wbk_kbhook_windows_hook29(int nCode, WPARAM wParam, LPARAM lParam);

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

static int g_kbhook_arr_len = 30;
static int g_kbhook_arr_i = 0;
static struct kbhook_s g_kbhook_arr[] = {
		{ 0, NULL, NULL, wbk_kbhook_windows_hook0, NULL, 0, 0 },
		{ 0, NULL, NULL, wbk_kbhook_windows_hook1, NULL, 0, 0 },
		{ 0, NULL, NULL, wbk_kbhook_windows_hook2, NULL, 0, 0 },
		{ 0, NULL, NULL, wbk_kbhook_windows_hook3, NULL, 0, 0 },
		{ 0, NULL, NULL, wbk_kbhook_windows_hook4, NULL, 0, 0 },
		{ 0, NULL, NULL, wbk_kbhook_windows_hook5, NULL, 0, 0 },
		{ 0, NULL, NULL, wbk_kbhook_windows_hook6, NULL, 0, 0 },
		{ 0, NULL, NULL, wbk_kbhook_windows_hook7, NULL, 0, 0 },
		{ 0, NULL, NULL, wbk_kbhook_windows_hook8, NULL, 0, 0 },
		{ 0, NULL, NULL, wbk_kbhook_windows_hook9, NULL, 0, 0 },
    { 0, NULL, NULL, wbk_kbhook_windows_hook10, NULL, 0, 0 },
		{ 0, NULL, NULL, wbk_kbhook_windows_hook11, NULL, 0, 0 },
		{ 0, NULL, NULL, wbk_kbhook_windows_hook12, NULL, 0, 0 },
		{ 0, NULL, NULL, wbk_kbhook_windows_hook13, NULL, 0, 0 },
		{ 0, NULL, NULL, wbk_kbhook_windows_hook14, NULL, 0, 0 },
		{ 0, NULL, NULL, wbk_kbhook_windows_hook15, NULL, 0, 0 },
		{ 0, NULL, NULL, wbk_kbhook_windows_hook16, NULL, 0, 0 },
		{ 0, NULL, NULL, wbk_kbhook_windows_hook17, NULL, 0, 0 },
		{ 0, NULL, NULL, wbk_kbhook_windows_hook18, NULL, 0, 0 },
		{ 0, NULL, NULL, wbk_kbhook_windows_hook19, NULL, 0, 0 },
    { 0, NULL, NULL, wbk_kbhook_windows_hook20, NULL, 0, 0 },
		{ 0, NULL, NULL, wbk_kbhook_windows_hook21, NULL, 0, 0 },
		{ 0, NULL, NULL, wbk_kbhook_windows_hook22, NULL, 0, 0 },
		{ 0, NULL, NULL, wbk_kbhook_windows_hook23, NULL, 0, 0 },
		{ 0, NULL, NULL, wbk_kbhook_windows_hook24, NULL, 0, 0 },
		{ 0, NULL, NULL, wbk_kbhook_windows_hook25, NULL, 0, 0 },
		{ 0, NULL, NULL, wbk_kbhook_windows_hook26, NULL, 0, 0 },
		{ 0, NULL, NULL, wbk_kbhook_windows_hook27, NULL, 0, 0 },
		{ 0, NULL, NULL, wbk_kbhook_windows_hook28, NULL, 0, 0 },
		{ 0, NULL, NULL, wbk_kbhook_windows_hook29, NULL, 0, 0 }
};

static char g_session_watcher_created = 0;

static HANDLE g_active_win_watcher_handler = NULL;

int
wbk_kbhook_reset_all_b(void)
{
	BYTE keyboard[256];
	int i;

	wbk_logger_log(&logger, DEBUG, "Reseting all tracked pressed keys\n");

	memset(keyboard, 0, 256);
	SetKeyboardState(keyboard);

	Sleep(5);

	for (i = 0; i < g_kbhook_arr_len; i++) {
		g_kbhook_arr[i].hook_entered = 0;
		g_kbhook_arr[i].hook_left = 0;
		wbk_b_reset(g_kbhook_arr[i].cur_b);
	}

	Sleep(5);

	SetKeyboardState(keyboard);

	return 0;
}

int
wbk_kbhook_any_hook_broken(void)
{
	int broken;

	broken = 0;

	if ((g_kbhook_arr[0].hook_entered != g_kbhook_arr[0].hook_left)
		|| (g_kbhook_arr[1].hook_entered != g_kbhook_arr[1].hook_left)
		|| (g_kbhook_arr[2].hook_entered != g_kbhook_arr[2].hook_left)) {
		broken = 1;
	}

	return broken;
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
				if (wbk_kbhook_any_hook_broken()) {
					reset = 1;
				}
				Sleep(5);
			}
		}

		if (reset) {
			wbk_kbhook_reset_all_b();
		}

		Sleep(5);
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
				   wbk_b_t **g_cur_b,
				   int *hook_entered,
				   int *hook_left)
{
	int ret;
	KBDLLHOOKSTRUCT *hookstruct;
	wbk_be_t be;
	int changed_any;
	int i;

	*hook_entered++;
	ret = 0;

	if (*hook_entered != *hook_left) {
		nCode = 0;
		ret = 0;
	}

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

	*hook_left++;
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
						   &(g_kbhook_arr[0].cur_b),
						   &(g_kbhook_arr[0].hook_entered),
						   &(g_kbhook_arr[0].hook_left));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook1(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
						   &(g_kbhook_arr[1].arr_len),
						   &(g_kbhook_arr[1].arr),
						   &(g_kbhook_arr[1].cur_b),
						   &(g_kbhook_arr[1].hook_entered),
						   &(g_kbhook_arr[1].hook_left));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook2(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
						   &(g_kbhook_arr[2].arr_len),
						   &(g_kbhook_arr[2].arr),
						   &(g_kbhook_arr[2].cur_b),
						   &(g_kbhook_arr[2].hook_entered),
						   &(g_kbhook_arr[2].hook_left));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook3(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
                            &(g_kbhook_arr[3].arr_len),
                            &(g_kbhook_arr[3].arr),
                            &(g_kbhook_arr[3].cur_b),
                            &(g_kbhook_arr[3].hook_entered),
                            &(g_kbhook_arr[3].hook_left));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook4(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
                            &(g_kbhook_arr[4].arr_len),
                            &(g_kbhook_arr[4].arr),
                            &(g_kbhook_arr[4].cur_b),
                            &(g_kbhook_arr[4].hook_entered),
                            &(g_kbhook_arr[4].hook_left));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook5(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
                            &(g_kbhook_arr[5].arr_len),
                            &(g_kbhook_arr[5].arr),
                            &(g_kbhook_arr[5].cur_b),
                            &(g_kbhook_arr[5].hook_entered),
                            &(g_kbhook_arr[5].hook_left));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook6(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
                            &(g_kbhook_arr[6].arr_len),
                            &(g_kbhook_arr[6].arr),
                            &(g_kbhook_arr[6].cur_b),
                            &(g_kbhook_arr[6].hook_entered),
                            &(g_kbhook_arr[6].hook_left));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook7(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
                            &(g_kbhook_arr[7].arr_len),
                            &(g_kbhook_arr[7].arr),
                            &(g_kbhook_arr[7].cur_b),
                            &(g_kbhook_arr[7].hook_entered),
                            &(g_kbhook_arr[7].hook_left));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook8(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
                            &(g_kbhook_arr[8].arr_len),
                            &(g_kbhook_arr[8].arr),
                            &(g_kbhook_arr[8].cur_b),
                            &(g_kbhook_arr[8].hook_entered),
                            &(g_kbhook_arr[8].hook_left));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook9(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
                            &(g_kbhook_arr[9].arr_len),
                            &(g_kbhook_arr[9].arr),
                            &(g_kbhook_arr[9].cur_b),
                            &(g_kbhook_arr[9].hook_entered),
                            &(g_kbhook_arr[9].hook_left));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook10(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
						   &(g_kbhook_arr[10].arr_len),
						   &(g_kbhook_arr[10].arr),
						   &(g_kbhook_arr[10].cur_b),
						   &(g_kbhook_arr[10].hook_entered),
						   &(g_kbhook_arr[10].hook_left));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook11(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
						   &(g_kbhook_arr[11].arr_len),
						   &(g_kbhook_arr[11].arr),
						   &(g_kbhook_arr[11].cur_b),
						   &(g_kbhook_arr[11].hook_entered),
						   &(g_kbhook_arr[11].hook_left));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook12(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
						   &(g_kbhook_arr[12].arr_len),
						   &(g_kbhook_arr[12].arr),
						   &(g_kbhook_arr[12].cur_b),
						   &(g_kbhook_arr[12].hook_entered),
						   &(g_kbhook_arr[12].hook_left));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook13(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
                            &(g_kbhook_arr[13].arr_len),
                            &(g_kbhook_arr[13].arr),
                            &(g_kbhook_arr[13].cur_b),
                            &(g_kbhook_arr[13].hook_entered),
                            &(g_kbhook_arr[13].hook_left));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook14(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
                            &(g_kbhook_arr[14].arr_len),
                            &(g_kbhook_arr[14].arr),
                            &(g_kbhook_arr[14].cur_b),
                            &(g_kbhook_arr[14].hook_entered),
                            &(g_kbhook_arr[14].hook_left));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook15(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
                            &(g_kbhook_arr[15].arr_len),
                            &(g_kbhook_arr[15].arr),
                            &(g_kbhook_arr[15].cur_b),
                            &(g_kbhook_arr[15].hook_entered),
                            &(g_kbhook_arr[15].hook_left));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook16(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
                            &(g_kbhook_arr[16].arr_len),
                            &(g_kbhook_arr[16].arr),
                            &(g_kbhook_arr[16].cur_b),
                            &(g_kbhook_arr[16].hook_entered),
                            &(g_kbhook_arr[16].hook_left));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook17(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
                            &(g_kbhook_arr[17].arr_len),
                            &(g_kbhook_arr[17].arr),
                            &(g_kbhook_arr[17].cur_b),
                            &(g_kbhook_arr[17].hook_entered),
                            &(g_kbhook_arr[17].hook_left));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook18(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
                            &(g_kbhook_arr[18].arr_len),
                            &(g_kbhook_arr[18].arr),
                            &(g_kbhook_arr[18].cur_b),
                            &(g_kbhook_arr[18].hook_entered),
                            &(g_kbhook_arr[18].hook_left));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook19(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
                            &(g_kbhook_arr[19].arr_len),
                            &(g_kbhook_arr[19].arr),
                            &(g_kbhook_arr[19].cur_b),
                            &(g_kbhook_arr[19].hook_entered),
                            &(g_kbhook_arr[19].hook_left));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook20(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
						   &(g_kbhook_arr[20].arr_len),
						   &(g_kbhook_arr[20].arr),
						   &(g_kbhook_arr[20].cur_b),
						   &(g_kbhook_arr[20].hook_entered),
						   &(g_kbhook_arr[20].hook_left));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook21(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
						   &(g_kbhook_arr[21].arr_len),
						   &(g_kbhook_arr[21].arr),
						   &(g_kbhook_arr[21].cur_b),
						   &(g_kbhook_arr[21].hook_entered),
						   &(g_kbhook_arr[21].hook_left));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook22(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
						   &(g_kbhook_arr[22].arr_len),
						   &(g_kbhook_arr[22].arr),
						   &(g_kbhook_arr[22].cur_b),
						   &(g_kbhook_arr[22].hook_entered),
						   &(g_kbhook_arr[22].hook_left));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook23(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
                            &(g_kbhook_arr[23].arr_len),
                            &(g_kbhook_arr[23].arr),
                            &(g_kbhook_arr[23].cur_b),
                            &(g_kbhook_arr[23].hook_entered),
                            &(g_kbhook_arr[23].hook_left));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook24(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
                            &(g_kbhook_arr[24].arr_len),
                            &(g_kbhook_arr[24].arr),
                            &(g_kbhook_arr[24].cur_b),
                            &(g_kbhook_arr[24].hook_entered),
                            &(g_kbhook_arr[24].hook_left));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook25(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
                            &(g_kbhook_arr[25].arr_len),
                            &(g_kbhook_arr[25].arr),
                            &(g_kbhook_arr[25].cur_b),
                            &(g_kbhook_arr[25].hook_entered),
                            &(g_kbhook_arr[25].hook_left));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook26(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
                            &(g_kbhook_arr[26].arr_len),
                            &(g_kbhook_arr[26].arr),
                            &(g_kbhook_arr[26].cur_b),
                            &(g_kbhook_arr[26].hook_entered),
                            &(g_kbhook_arr[26].hook_left));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook27(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
                            &(g_kbhook_arr[27].arr_len),
                            &(g_kbhook_arr[27].arr),
                            &(g_kbhook_arr[27].cur_b),
                            &(g_kbhook_arr[27].hook_entered),
                            &(g_kbhook_arr[27].hook_left));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook28(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
                            &(g_kbhook_arr[28].arr_len),
                            &(g_kbhook_arr[28].arr),
                            &(g_kbhook_arr[28].cur_b),
                            &(g_kbhook_arr[28].hook_entered),
                            &(g_kbhook_arr[28].hook_left));
}

LRESULT CALLBACK
wbk_kbhook_windows_hook29(int nCode, WPARAM wParam, LPARAM lParam)
{
	return wbk_kbhook_windows(nCode, wParam, lParam,
                            &(g_kbhook_arr[29].arr_len),
                            &(g_kbhook_arr[29].arr),
                            &(g_kbhook_arr[29].cur_b),
                            &(g_kbhook_arr[29].hook_entered),
                            &(g_kbhook_arr[29].hook_left));
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
