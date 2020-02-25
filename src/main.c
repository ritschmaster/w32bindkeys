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
#include <getopt.h>
#include <unistd.h>

#include "../config.h"
#include "logger.h"
#include "util.h"
#include "datafinder.h"
#include "ikbman.h"
#include "iparser.h"
#include "kb.h"

#define WBK_RC ".w32bindkeysrc"

#define WBK_DEFAULTS_RC "w32bindkeysrc"

#define WBK_GETOPT_OPTIONS "dhvV"

static struct option B3_GETOPT_LONG_OPTIONS[] = {
    /*   NAME          ARGUMENT           FLAG  SHORTNAME */
        {"help",       no_argument,       NULL, 'h'},
        {"verbose",    no_argument,       NULL, 'v'},
        {"version",    no_argument,       NULL, 'V'},
        {"defaults",   no_argument,       NULL, 'd'},
        {NULL,         0,                 NULL, 0}
    };

static wbk_logger_t logger = { "main" };

static const char g_szClassName[] = "myWindowClass";

static wbki_kbman_t *g_kbman = NULL;

static int
print_version(void);

static int
print_help(const char *cmd_name);

static int
print_defaults(const wbk_datafinder_t *datafinder);

static int
parameterized_main(HINSTANCE hInstance, const wbk_datafinder_t *datafinder);

static int
main_loop(HINSTANCE hInstance);

static LRESULT CALLBACK
window_callback(HWND window_handler, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	wbk_datafinder_t *datafinder;
	int ret;
	char exec;
	LPWSTR *wargv;
	char **argv;
	int argc;
	int i;
	size_t size;
	int opt;
	int option_index;

	wbk_logger_set_level(SEVERE);

	datafinder = wbk_datafinder_new(PKGDATADIR);
	exec = 1;

	wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
	if (wargv) {
		argv = malloc(sizeof(char *) * argc);
		for (i = 0; i < argc; i++) {
			size = sizeof(char) * (sizeof(wchar_t) * wcslen(wargv[i]) + 1);
			argv[i] = malloc(size);
			memset(argv[i], 0, size);
			wcstombs(argv[i], wargv[i], size);
		}

		option_index = 0;
		while ((opt = getopt_long(argc, argv,
								  WBK_GETOPT_OPTIONS,
								  B3_GETOPT_LONG_OPTIONS,
								  &option_index))
				!= -1) {
			switch(opt) {
			case 'v':
#ifdef DEBUG_ENABLED
				wbk_logger_set_level(DEBUG);
#else
				wbk_logger_set_level(INFO);
#endif
				break;

			case 'V':
				ret = print_version();
				exec = 0;
				break;

			case 'd':
				ret = print_defaults(datafinder);
				exec = 0;
				break;

			case 'h':
			default:
				ret = print_help(argv[0]);
				exec = 0;
			}
		}

		free(argv);
		LocalFree(wargv);
	}

	if (exec) {
		ret = parameterized_main(hInstance, datafinder);
	}

	wbk_datafinder_free(datafinder);

	return ret;
}

int
print_version(void)
{
	fprintf(stdout, "%s version %s\n", PACKAGE, PACKAGE_VERSION);
	return 0;
}

int
print_help(const char *cmd_name)
{
	print_version();

	fprintf(stdout, "Usage: %s [options]\n", cmd_name);
	fprintf(stdout, "  where options are:\n", cmd_name);
	fprintf(stdout, "  -V, --version          Print version and exit\n", cmd_name);
	fprintf(stdout, "  -d, --defaults         Print a default rc file\n", cmd_name);
	fprintf(stdout, "  -v, --verbose          More information on w32bindkeys when it run\n", cmd_name);
	fprintf(stdout, "  -h, --help             This help!\n", cmd_name);

	return 0;
}

int
print_defaults(const wbk_datafinder_t *datafinder)
{
	char *defaults_rc;

	defaults_rc = wbk_datafinder_gen_path(datafinder, WBK_DEFAULTS_RC);

	if (defaults_rc) {
		wbk_write_file(defaults_rc, stdout);
		free(defaults_rc);
	}

	return 0;
}

int
parameterized_main(HINSTANCE hInstance, const wbk_datafinder_t *datafinder)
{
	int error;
	char *rc_filename;
	char *defaults_rc_filename;
	FILE *rc_file;
	wbki_parser_t *parser;
	wbk_kbman_t *kbman;

	error = 0;
	rc_filename = NULL;
	rc_file = NULL;
	parser = NULL;
	kbman = NULL;

	if (!error) {
		rc_filename = wbk_path_from_home(WBK_RC);
		if(access(rc_filename, F_OK)) {
			/**
			 * Create WBK_RC by WBK_DEFAULTS_RC
			 */
			defaults_rc_filename = wbk_datafinder_gen_path(datafinder, WBK_DEFAULTS_RC);
			if (defaults_rc_filename) {
				wbk_logger_log(&logger, INFO, "Copy %s to %s: %s\n", defaults_rc_filename, rc_filename);
				rc_file = fopen(rc_filename, "w");
				wbk_write_file(defaults_rc_filename, rc_file);
				fclose(rc_file);
				rc_file = NULL;
			} else {
				error = 1;
			}
		}
	}

	if (!error) {
		parser = wbki_parser_new(rc_filename);
		if (!parser) {
			error = 1;
		}
	}

	if (!error) {
		kbman = wbki_parser_parse(parser);
		if (!kbman) {
			error = 1;
		}
	}

	if (!error) {
		g_kbman = wbki_kbman_new(kbman);

		error = main_loop(hInstance);
	}

	if (rc_filename) {
		free(rc_filename);
	}

	if (parser) {
		wbki_parser_free(parser);
	}

	if (g_kbman) {
		wbki_kbman_free(g_kbman);
	}

	return error;
}

/**
 * @param c The result of GetAsyncKeyState
 */
static wbk_mk_t
win32_to_mk(unsigned char c);

/**
 * @param c The result of GetAsyncKeyState
 */
static char
win32_to_char(unsigned char c);

wbk_mk_t
win32_to_mk(unsigned char c)
{
	wbk_mk_t modifier;

	modifier = NOT_A_MODIFIER;

	if (c == 13)
		modifier = ENTER;
	//else if(c == 16 || c == 17 || c == 18) // TODO maybe
		//; // TODO maybe
	// else if(c == 160 || c == 161) // lastc == 16 // TODO maybe
	else if (c == 16)
		modifier = SHIFT;
	// else if(c == 162 || c == 163) // lastc == 17 // TODO maybe
	else if (c == 17)
		modifier = CTRL;
	// else if(c == 164) // lastc == 18 // TODO maybe
	else if (c == 18)
		modifier = ALT;
	//else if(c == 91 || c == 92) // TODO maybe
	else if(c == 91)   modifier = WIN;
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
win32_to_char(unsigned char c)
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

int
main_loop(HINSTANCE hInstance)
{
	unsigned char c;
	wbk_b_t *b;
	wbk_be_t *be;
	SHORT rv;
	char changed;

	b = wbk_b_new();

	while(1) {
		usleep(100);

		changed = 0;
		for(c = 1; c < 255; c++){
			rv = GetKeyState(c);
			be = wbk_be_new(win32_to_mk(c), win32_to_char(c));
			if (wbk_be_get_modifier(be) || wbk_be_get_key(be)) {
				if(rv < 0) {
					if (wbk_b_add(b, be) == 0) {
						changed = 1;
						wbk_logger_log(&logger, DEBUG, "pressed %d, %c\n", wbk_be_get_modifier(be), wbk_be_get_key(be));
					}
				} else {
					if (wbk_b_remove(b, be) == 0) {
						changed = 1;
						wbk_logger_log(&logger, DEBUG, "released %d, %c\n", wbk_be_get_modifier(be), wbk_be_get_key(be));
					}
				}
			}
			wbk_be_free(be);
		}

		if (changed) {
			wbk_kbman_exec(g_kbman->kbman, b);
		}

//				if(c == 1)
//					out = "[LMOUSE]"; // mouse left
//				else if(c == 2)
//					out = "[RMOUSE]"; // mouse right
//				else if(c == 4)
//					out = "[MMOUSE]"; // mouse middle
				//				else if(c == 165)
//					out = "[ALT GR]";
//				else if(c == 8)
//					out = "[BACKSPACE]";
//				else if(c == 9)
//					out = "[TAB]";
//				else if(c == 27)
//					out = "[ESC]";
//				else if(c == 33)
//					out = "[PAGE UP]";
//				else if(c == 34)
//					out = "[PAGE DOWN]";
//				else if(c == 35)
//					out = "[HOME]";
//				else if(c == 36)
//					out = "[POS1]";
//				else if(c == 37)
//					out = "[ARROW LEFT]";
//				else if(c == 38)
//					out = "[ARROW UP]";
//				else if(c == 39)
//					out = "[ARROW RIGHT]";
//				else if(c == 40)
//					out = "[ARROW DOWN]";
//				else if(c == 45)
//					out = "[INS]";
//				else if(c == 46)
//					out = "[DEL]";
//				else if((c >= 65 && c <= 90)
//					|| (c >= 48 && c <= 57)
//					|| c == 32)
//					out = c;
//

//				else if(c >= 96 && c <= 105)
//					out = "[NUM " + intToString(c - 96) + "]";
//				else if(c == 106)
//					out = "[NUM /]";
//				else if(c == 107)
//					out = "[NUM +]";
//				else if(c == 109)
//					out = "[NUM -]";
//				else if(c == 109)
//					out = "[NUM ,]";

				// else if(c == 144)
//					out = "[NUM]";
//				else if(c == 192)
//					out = "[OE]";
//				else if(c == 222)
//					out = "[AE]";
//				else if(c == 186)
//					out = "[UE]";
	}

	wbk_b_free(b);


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
		wbki_kbman_exec_kb(g_kbman, wParam);
		break;

	default:
		result = DefWindowProc(window_handler, msg, wParam, lParam);
	}

	return result;
}


