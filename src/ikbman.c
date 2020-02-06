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
 * @param id_kb_arr Array of wbk_w32_id_kb_t
 */
static int
id_kb_arr_free(Array *id_kb_arr);

wbki_kbman_t *
wbki_kbman_new(wbk_kbman_t *kbman)
{
	wbki_kbman_t *new;

	new = NULL;
	new = malloc(sizeof(wbki_kbman_t));

	new->kbman = kbman;
	array_new(&(new->id_kb_arr));

	return new;

}

wbki_kbman_t *
wbki_kbman_free(wbki_kbman_t *kbman)
{
	id_kb_arr_free(kbman->id_kb_arr);
	wbk_kbman_free(kbman->kbman);
	free(kbman);
}

int
wbki_kbman_register_kb(wbki_kbman_t *kbman, HWND window_handler)
{
	ArrayIter kb_iter;
	ArrayIter b_iter;
	ArrayIter be_iter;
	wbk_kb_t *kb;
	wbk_b_t *b;
	wbk_be_t *be;
	UINT modifiers;
	UINT key;
	int id;
	wbki_id_kb_t *id_kb;

	id_kb_arr_free(kbman->id_kb_arr);
	kbman->id_kb_arr = NULL;
	array_new(&(kbman->id_kb_arr));

	id = 100;
	array_iter_init(&kb_iter, wbk_kbman_get_kb(kbman->kbman));
	while (array_iter_next(&kb_iter, (void *) &kb) != CC_ITER_END) {
		modifiers = 0;
		key = 0;
		array_iter_init(&be_iter, wbk_b_get_comb(wbk_kb_get_comb(kb)));
		while (array_iter_next(&be_iter, (void *) &be) != CC_ITER_END) {
			wbk_logger_log(&logger, INFO, "Binding element: %d %c\n", be->modifier, be->key);
			switch(be->modifier) {
			case WIN:
				modifiers = modifiers | MOD_WIN;
				break;

			case ALT:
				modifiers = modifiers | MOD_ALT;
				break;

			case CTRL:
				modifiers = modifiers | MOD_CONTROL;
				break;

			case SHIFT:
				modifiers = modifiers | MOD_SHIFT;
				break;

			case ENTER:
				key = VK_RETURN;
				break;

			case NUMLOCK:
				key = VK_NUMLOCK;
				break;

			case CAPSLOCK:
				key = VK_CAPITAL;
				break;

			case F1:
				key = VK_F1;
				break;

			case F2:
				key = VK_F2;
				break;

			case F3:
				key = VK_F3;
				break;

			case F4:
				key = VK_F4;
				break;

			case F5:
				key = VK_F5;
				break;

			case F6:
				key = VK_F6;
				break;

			case F7:
				key = VK_F7;
				break;

			case F8:
				key = VK_F9;
				break;

			case F9:
				key = VK_F1;
				break;

			case F10:
				key = VK_F10;
				break;

	    	case F11:
				key = VK_F11;
				break;

			case F12:
				key = VK_F12;
				break;

	    	case NOT_A_MODIFIER:
				key = toupper(be->key);
				break;

			// default:
				// TODO error
			}
		}

		if (key != 0) {
			id_kb = malloc(sizeof(wbki_id_kb_t));
			id_kb->id = id;
			id_kb->kb = kb;
			array_add(kbman->id_kb_arr, id_kb);
			id_kb = NULL;

			wbk_logger_log(&logger, INFO, "Registering hotkey: %d %c\n", modifiers, key);
			RegisterHotKey(window_handler, id, modifiers, key);
			//RegisterHotKey(window_handler, id, MOD_WIN | MOD_ALT, 'X');
			id++;
		}
	}

	return 0;
}

int
wbki_kbman_exec_kb(wbki_kbman_t *kbman, int id)
{
	char found;
	ArrayIter id_kb_arr_iter;
	wbki_id_kb_t *id_kb;

	wbk_logger_log(&logger, INFO, "Id triggered: %d\n", id);

	found = 0;
	array_iter_init(&id_kb_arr_iter, kbman->id_kb_arr);
	while ((array_iter_next(&id_kb_arr_iter, (void *) &id_kb) != CC_ITER_END)
		    && !found) {
		if (id_kb->id == id) {
			wbk_kb_exec(id_kb->kb);
			found = 1;
		}
	}

	return 0;
}

int
id_kb_arr_free(Array *id_kb_arr)
{
	ArrayIter id_kb_arr_iter;
	wbki_id_kb_t *id_kb;

	array_iter_init(&id_kb_arr_iter, id_kb_arr);
	while (array_iter_next(&id_kb_arr_iter, (void *) &id_kb) != CC_ITER_END) {
		id_kb->kb = NULL;
	}

	array_destroy_cb(id_kb_arr, free);
}
