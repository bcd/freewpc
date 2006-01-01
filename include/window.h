#ifndef _WINDOW_H
#define _WINDOW_H

/* The window operations structure (window class). */
struct window_ops
{
	/** Constructor/destructor functions */
	void (*init) (void);
	void (*exit) (void);

	/** suspend/resume are called whenever the
	 * current window is swapped out due to a new
	 * window being loaded, or being swapped back
	 * in when the new window exits. */
	void (*suspend) (void);
	void (*resume) (void);

	/* The draw function, called just about every
	 * time something changes.  Draw functions are
	 * always called after the key handlers below. */
	void (*draw) (void);

	/** Key handlers **/
	void (*escape) (void);
	void (*enter) (void);
	void (*up) (void);
	void (*down) (void);
	void (*left) (void);
	void (*right) (void);
	void (*start) (void);

	/** Thread function.  This should be set to NULL
	 * if the window doesn't need a thread. */
	void (*thread) (void);
};


/* The default window constructor.
 *
 * We use a gcc trick here to simulate object-oriented
 * behavior.  In a structue, if you have two named
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
	.suspend = null_function, \
	.resume = null_function, \
	.draw = null_function, \
	.escape = window_pop, \
	.enter = null_function, \
	.up = null_function, \
	.down = null_function, \
	.left = null_function, \
	.right = null_function, \
	.start = null_function, \
	.thread = NULL


struct menu;


/* The window object.  These are RAM objects that store the
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


/** menu_selection is a shorthand macro for obtaining
 * the current "selection" in a menu window. */
#define menu_selection (win_top->w_class.menu.selected)


/** window_call_op() is a macro for invoking one of the
 * window_ops.  This is done so that tracing can be added
 * here, rather than throughout the code. */
#if 0
#define window_call_op(win, op) \
do { \
	if (win->ops->op) \
	{ \
		(*win->ops->op) (); \
	} \
} while (0)
#else
#define window_call_op(win, op)	win->ops->op ()
#endif


#endif /* _WINDOW_H */
