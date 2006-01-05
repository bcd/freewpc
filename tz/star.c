
#include <freewpc.h>

void star_task (void)
{
	U8 *dmd = (U8 *)task_get_arg ();
	int i;

	for (i=3; i>0; i--)
	{
		dmd[-DMD_BYTE_WIDTH] = 0x10;
		dmd[-DMD_BYTE_WIDTH + DMD_PAGE_SIZE] = 0x10;
		task_sleep (TIME_33MS);

		dmd[0] = 0x3C;
		dmd[DMD_PAGE_SIZE] = 0x3C;
		task_sleep (TIME_66MS);

		dmd[+DMD_BYTE_WIDTH] = 0x8;
		dmd[+DMD_BYTE_WIDTH+DMD_PAGE_SIZE] = 0x8;
		task_sleep (TIME_33MS);

		dmd[-DMD_BYTE_WIDTH] = 0x8;
		dmd[-DMD_BYTE_WIDTH+DMD_PAGE_SIZE] = 0x8;
		task_sleep (TIME_66MS);

		dmd[0] = 0x18;
		dmd[0+DMD_PAGE_SIZE] = 0x18;
		task_sleep (TIME_33MS);

		dmd[+DMD_BYTE_WIDTH] = 0x10;
		dmd[+DMD_BYTE_WIDTH+DMD_PAGE_SIZE] = 0x10;
		task_sleep (TIME_100MS);
	}
	task_exit ();
}


#define star_create(x,y) \
do \
{ \
	task_t *tp = task_create_child (star_task); \
	task_set_arg (tp, (U16)(dmd_low_buffer + ((U16)y << 4) + x)); \
} while (0)

void starfield_task (void)
{
	star_create (1, 3);
	task_sleep_sec (1);
	star_create (14, 28);
	task_sleep_sec (1);
	star_create (14, 3);
	task_sleep_sec (1);
	star_create (1, 28);
	task_sleep_sec (2);
	task_exit ();
}

void starfield_start (void)
{
	task_create_child (starfield_task);
}

