/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
 *
 * This file is part of FreeWPC.
 *
 * FreeWPC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * FreeWPC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with FreeWPC; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _WINDOW_H
#define _WINDOW_H

/** The window operations structure (window class).
 * In test mode, these are callbacks that get invoked to
 * perform specific operations, depending on which
 * buttons are pressed.
 */
struct window_ops
{
	/** A constructor function that executes right after
	 * the window is created */
	void (*init) (void);

	/** A destructor function that executes just before
	 * the window is destroyed */
	void (*exit) (void);

	/** The draw function, called just about every
	 * time something changes.  Draw functions are
	 * always called after the key handlers below.
	 * It must be fast, and should not sleep. */
	void (*draw) (void);

	/** Thread function.  This should be set to NULL
	 * if the window doesn't need a thread.   Otherwise,
	 * a thread will be spawned to this function whenever
	 * the window is active.  (Note, this is different
	 * than the behavior of init/exit.  When a new submenu
	 * is entered, the old window's thread will be stopped,
	 * but its exit isn't called.)
	 */
	void (*thread) (void);

	/** Key handlers **/
	void (*escape) (void);
	void (*enter) (void);
	void (*up) (void);
	void (*down) (void);
	void (*left) (void);
	void (*right) (void);
	void (*start) (void);
};


/** The default window constructor.
 *
 * We use a gcc trick here to simulate object-oriented
 * behavior.  In a structure, if you have two named
 * field definitions in an initializer, only the last one
 * is used.  We start our structure initializers with
 * DEFAULT_WINDOW to give them default values, in case no
 * override is desired.  The code can declare different
 * initializers for any custom behavior.
 *
 * Generally, to simulate subclassing, the overriding
 * function should call the overridden first.
 *
 * All functions are null by default, except for
 * the ESCAPE button, which always pops the current
 * window and returns to the previous one.
 */
#define DEFAULT_WINDOW \
	.init = null_function, \
	.exit = null_function, \
	.draw = null_function, \
	.escape = window_pop, \
	.enter = null_function, \
	.up = null_function, \
	.down = null_function, \
	.left = null_function, \
	.right = null_function, \
	.start = null_function, \
	.thread = NULL


/* The browser window constructor */

#define INHERIT_FROM_BROWSER \
	DEFAULT_WINDOW, \
	.init = browser_init, \
	.draw = browser_draw, \
	.up = browser_up, \
	.down = browser_down \

extern __test__ void browser_init (void);
extern __test__ void browser_draw (void);
extern __test__ void browser_up (void);
extern __test__ void browser_down (void);

struct menu;


/** The window object.  These are RAM objects that store the
 * state of a particular instance of a window.
 *
 * The w_class union is used to hold custom data needed for
 * different classes of windows.  The common code only knows
 * about w_priv; the init function should always use the
 * priv data (passed in by the creator of the window) to
 * populate the w_class union as needed.
 */
struct window
{
	struct window_ops *ops;
	union  {
		struct window_menu {
			struct menu *self;
			struct menu *parent;
			U8 selected;
		} menu;
		struct window_scroller {
			void *funcs;
			U8 offset;
			U8 size;
		} scroller;
		void *priv;
	} w_class;
};


extern struct window *win_top;

/** menu_selection is a shorthand macro for obtaining
 * the current "selection" in a menu window. */
#define menu_selection (win_top->w_class.menu.selected)


/** window_call_op() is a macro for invoking one of the
 * window_ops.  This is done so that tracing can be added
 * here, rather than throughout the code, if needed. */
#define window_call_op(win, op)	win->ops->op ()

#endif /* _WINDOW_H */
