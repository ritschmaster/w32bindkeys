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

#include "logger.h"
#include "kbman.h"

static wbk_logger_t logger =  { "ibkman" };

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

wbki_kbman_t *
wbki_kbman_new()
{
	wbki_kbman_t *new;

	new = NULL;
	new = malloc(sizeof(wbki_kbman_t));

	array_new(&(new->kc_sys));

	return new;

}

wbki_kbman_t *
wbki_kbman_free(wbki_kbman_t *kbman)
{
	array_destroy_cb(kbman->kc_sys, free); // TODO wbk_kb_sys_free
	kbman->kc_sys = NULL;
	free(kbman);
}

Array *
wbki_kbman_get_kb(wbki_kbman_t* kbman)
{
	return kbman->kc_sys;
}

int
wbki_kbman_add(wbki_kbman_t *kbman, wbk_kc_sys_t *kc_sys)
{
	array_add(kbman->kc_sys, kc_sys);
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
			wbki_kbman_exec(kbman, b);
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
