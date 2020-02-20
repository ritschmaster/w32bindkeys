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
 * @brief File contains the key binding class definition
 */

#ifndef WBK_KB_H
#define WBK_KB_H

#include <collectc/array.h>

/**
 * @brief Modifier key
 */
typedef enum wbk_mk_e {
	NOT_A_MODIFIER,
	WIN,
	ALT,
	CTRL,
	SHIFT,
	ENTER,
	NUMLOCK,
	CAPSLOCK,
	SCROLL,
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

/**
 * @brief Binding element
 */
typedef struct wbk_be_s
{
	wbk_mk_t modifier;
	char key;
} wbk_be_t;

/**
 * @brief Binding
 */
typedef struct wbk_b_s
{
	/**
	 * Array of wbk_be_t
	 */
	Array *comb;
} wbk_b_t;

typedef struct wbk_kb_s
{
	wbk_b_t *comb;
	char *cmd;
} wbk_kb_t;

extern wbk_be_t *
wbk_be_new(wbk_mk_t modifier, char key);

extern wbk_b_t *
wbk_b_new();

extern int
wbk_b_free(wbk_b_t *b);

/**
 * @param be Binding element to add. Contents will be copied
 */
extern int
wbk_b_add(wbk_b_t *b, wbk_be_t *be);

/**
 * @return
 */
extern Array *
wbk_b_get_comb(const wbk_b_t *b);


/**
 * @brief Creates a new key binding
 * @param comb The binding of the key binding. The object will be freed by the key binding.
 * @param cmd The command of the key binding. The passed string will be freed by the key binding.
 * @return A new key binding or NULL if allocation failed
 */
extern wbk_kb_t *
wbk_kb_new(wbk_b_t *comb, char *cmd);

/**
 * @brief Frees a key binding
 * @return Non-0 if the freeing failed
 */
extern int
wbk_kb_free(wbk_kb_t *kb);

/**
 * @brief Gets the combinations of a key binding
 * @return The combinations of a key binding. It is an array of wbk_b_t.
 */
extern const wbk_b_t *
wbk_kb_get_comb(const wbk_kb_t *kb);

/**
 * @brief Gets the command of a key binding
 * @return The command of a key binding. Do not free it.
 */
extern const char *
wbk_kb_get_cmd(const wbk_kb_t *kb);

/**
 * @brief Execute the command of a key binding
 * @return Non-0 if the execution failed
 */
extern int
wbk_kb_exec(const wbk_kb_t *kb);

#endif // WBK_KB_H
