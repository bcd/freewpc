#ifndef _TEST_H
#define _TEST_H

struct test;


typedef struct test_class
{
	void (*escape_handler) (struct test *t);
	void (*enter_handler) (struct test *t);
	void (*up_handler) (struct test *t);
	void (*down_handler) (struct test *t);
} test_class_t;

typedef struct test
{
	const char *banner;
	const struct test *up;
	uint8_t start_index;
	uint8_t child_count;
	union {
		struct {
			const struct test *first_child;
		} menu;

		struct {
			void (*enter_proc) (void) __taskentry__;
			void (*value_format) (U8 *value);
			U8 *value;
		} item;
	} u;
} test_t;


#define TEST_MENU_COUNT(m)	(sizeof (m) / sizeof (struct test))

#define TEST_ITEM(enter_proc)	\
	.u = { .item = { enter_proc, NULL, NULL } }

#define RO_VALUE_ITEM(format, valptr) \
	0, 0, .u = { .item = { NULL, format, valptr } }

#define TEST_MENU(m)	\
	TEST_MENU_COUNT(m), .u = { .menu = { m } }

#endif /* _TEST_H */

