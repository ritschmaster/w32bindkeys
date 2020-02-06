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
 * @date 26 January 2020
 * @brief File contains the main function
 */

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

#include "ikbman.h"
#include "iparser.h"
#include "logger.h"
#include "util.h"

#define DEFAULT_CONFIG ".w32bindkeys"

static const char g_szClassName[] = "myWindowClass";

static wbki_kbman_t *kbman = NULL;

static int
main_loop(HINSTANCE hInstance);

static LRESULT CALLBACK
window_callback(HWND window_handler, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	char *filename;
	wbki_parser_t *parser;

	wbk_logger_set_level(SEVERE);
	wbk_logger_set_level(INFO);

	filename = wbk_path_from_home(DEFAULT_CONFIG);
	parser = wbki_parser_new(filename);
	if (parser != NULL) {
		kbman = wbki_kbman_new(wbki_parser_parse(parser));

		main_loop(hInstance);

		wbki_parser_free(parser);
		wbki_kbman_free(kbman);
	}
	free(filename);

	return 0;
}

int
main_loop(HINSTANCE hInstance)
{
	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;
	PTITLEBARINFO titlebar_info;

	wc.cbSize		= sizeof(WNDCLASSEX);
	wc.style		 = 0;
	wc.lpfnWndProc   = window_callback;
	wc.cbClsExtra	= 0;
	wc.cbWndExtra	= 0;
	wc.hInstance	 = hInstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor	   = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm	   = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}


	hwnd = CreateWindowEx(WS_EX_NOACTIVATE | WS_EX_TOPMOST,
		                  g_szClassName,
						  "w32bindkeys",
						  WS_DISABLED | WS_BORDER,
						  0, 0,
						  10, 10,
						  NULL, NULL, hInstance, NULL);

	UpdateWindow(hwnd);

	wbki_kbman_register_kb(kbman, hwnd);

	while(GetMessage(&Msg, NULL, 0, 0) > 0) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	return 0;
}

LRESULT CALLBACK
window_callback(HWND window_handler, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT CALLBACK result;
	int id;

	result = 0;

	switch(msg)
	{
	case WM_CLOSE:
		DestroyWindow(window_handler);
		break;

	case WM_HOTKEY:
		wbki_kbman_exec_kb(kbman, wParam);
		break;

	default:
		result = DefWindowProc(window_handler, msg, wParam, lParam);
	}

	return result;
}
