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
#include "kc.h"

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

static int
print_version(void);

static int
print_help(const char *cmd_name);

static int
print_defaults(const wbk_datafinder_t *datafinder);

static int
parameterized_main(HINSTANCE hInstance, const wbk_datafinder_t *datafinder);

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
	wbki_kbman_t *kbman = NULL;

	error = 0;
	rc_filename = NULL;
	rc_file = NULL;
	parser = NULL;

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
		while (1) {
			error = wbki_kbman_main(kbman);
			sleep(1000000000);
		}
		wbki_kbman_stop(kbman);
	}

	if (rc_filename) {
		free(rc_filename);
	}

	if (parser) {
		wbki_parser_free(parser);
	}

	if (kbman) {
		wbki_kbman_free(kbman);
	}

	return error;
}
