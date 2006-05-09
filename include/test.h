
#ifndef _TEST_H
#define _TEST_H

struct window;

/** win_top always points to the current window, or NULL if
 * no window is open. */
struct window *win_top;

extern U8 in_test;

void window_push_first (void);
void window_pop_first (void);
void window_start_thread (void);
void window_stop_thread (void);
void window_push (struct window_ops *ops, void *priv);
void window_pop_quiet (void);
void window_pop (void);
void window_init (void);



#define M_MENU 0x1
#define M_ITEM 0x2
#define M_LETTER_PREFIX 0x4

struct menu
{
	const char *name;
	int flags;
	union {
		struct menu **submenus;
		struct {
			struct window_ops *ops;
			void *priv;
		} subwindow;
	} var;
};

extern struct menu main_menu;
extern struct window_ops menu_window;


#define ADJ_BROWSING 0
#define ADJ_EDITING 1
#define ADJ_CONFIRMING 2

struct adjustment_value
{
	U8 min;
	U8 max;
	U8 step;
	void (*render) (U8 val);
};

struct adjustment
{
	const char *name;
	struct adjustment_value *values;
	U8 factory_default;
	U8 *nvram;
};

void adj_reset (struct adjustment *adjs);
void adj_verify (struct adjustment *adjs);
void adj_reset_all (void);
void adj_verify_all (void);
	

struct audit
{
	const char *name;
	void (*render) (audit_t);
	audit_t *nvram;
};


void menu_init (void);
void menu_draw (void);
void menu_enter (void);
void menu_up (void);
void menu_down (void);
void menu_start (void);


void test_init (void);
void test_up_button (void);
void test_down_button (void);
void test_left_flipper_button (void);
void test_right_flipper_button (void);
void test_enter_button (void);
void test_escape_button (void);
void test_start_button (void);

#endif /* _TEST_H */
