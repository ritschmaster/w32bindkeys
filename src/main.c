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


#include <stdio.h>

#include <windows.h>

#include "logger.h"
#include "w32parser.h"
#include "kbman.h"
#include "w32util.h"

// #define DEFAULT_CONFIG "~/.w32bindkeys"
#define DEFAULT_CONFIG "/home/user/.w32bindkeys"

static LRESULT CALLBACK
wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	wbk_w32parser_t *parser;
	wbk_kbman_t *kbman;

	wbk_logger_set_level(SEVERE);
	wbk_logger_set_level(INFO);

	parser = wbk_w32parser_new(DEFAULT_CONFIG);
	kbman = wbk_w32parser_parse(parser);

	wbk_w32util_register_kb(kbman);

	wbk_w32parser_free(parser);
	wbk_kbman_free(kbman);
	return 0;
}

LRESULT CALLBACK
wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT CALLBACK result;

	result = 0;

	switch(msg)
	{
		case WM_HOTKEY:
			// wbk_w32util_exec_kb(kbman); // TODO
			break;

		default:
			result = DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return result;
}
