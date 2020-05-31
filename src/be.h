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
 * @date 2020-02-26
 * @brief File contains the binding element class definition
 */

#ifndef WBK_BE_H
#define WBK_BE_H

#define WIN_STR "Mod4"
#define ALT_STR "Mod1"
#define CTRL_STR "Ctrl"
#define SHIFT_STR "Shift"
#define ENTER_STR "Return"
#define NUMLOCK_STR "Numlock"
#define CAPSLOCK_STR "Capslock"
#define SCROLL_STR "Scroll"
#define SPACE_STR "Space"
#define F1_STR "F1"
#define F2_STR "F2"
#define F3_STR "F3"
#define F4_STR "F4"
#define F5_STR "F5"
#define F6_STR "F6"
#define F7_STR "F7"
#define F8_STR "F8"
#define F9_STR "F9"
#define F10_STR "F10"
#define F11_STR "F11"
#define F12_STR "F12"

/**
 * @brief Modifier key
 */
typedef enum wbk_mk_e {
	NOT_A_MODIFIER = 0,
	WIN,
	ALT,
	CTRL,
	SHIFT,
	ENTER,
	NUMLOCK,
	CAPSLOCK,
	SCROLL,
	SPACE,
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12
} wbk_mk_t;

typedef struct wbk_be_s
{
	wbk_mk_t modifier;
	char key;
} wbk_be_t;

extern wbk_be_t *
wbk_be_new(wbk_mk_t modifier, char key);

extern int
wbk_be_free(wbk_be_t *be);

extern wbk_mk_t
wbk_be_get_modifier(const wbk_be_t *be);

extern char
wbk_be_get_key(const wbk_be_t *be);

/**
 * @param be
 * @param other
 * @return If be < other, then < 0. If be > other, then > 0. If be = other, then 0.
 */
extern int
wbk_be_compare(const wbk_be_t *be, const wbk_be_t *other);

#endif // WBK_BE_H
