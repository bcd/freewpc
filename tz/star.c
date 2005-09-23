
#include <freewpc.h>

void star_task (void)
{
	uint16_t args = task_get_arg ();
	uint8_t x = args >> 8;
	uint8_t y = args & 0xFF;
	uint8_t *dmd = dmd_low_bytes + (y << 4) + x;
	uint8_t i;

	for (i=8; i>0; i--)
	{
		dmd[-DMD_BYTE_WIDTH] = 0x10;
		task_sleep (TIME_33MS);

		dmd[0] = 0x3C;
		task_sleep (TIME_33MS);

		dmd[+DMD_BYTE_WIDTH] = 0x8;
		task_sleep (TIME_33MS);

		dmd[-DMD_BYTE_WIDTH] = 0x8;
		task_sleep (TIME_33MS);

		dmd[0] = 0x18;
		task_sleep (TIME_33MS);

		dmd[+DMD_BYTE_WIDTH] = 0x10;
		task_sleep (TIME_33MS);
	}
	task_exit ();
}


#define star_create(x,y) \
do \
{ \
	task_t *tp = task_create_gid (GID_STARFIELD, star_task); \
	task_set_arg (tp, (x << 8) | y); \
} while (0)

void starfield_task (void)
{
	star_create (1, 5);
	task_sleep_sec (1);
	star_create (14, 24);
	task_sleep_sec (1);
	star_create (14, 8);
	task_sleep_sec (1);
	star_create (1, 27);
	task_exit ();
}

void starfield_start (void)
{
	task_create_gid (GID_STARFIELD, starfield_task);
}

void starfield_stop (void)
{
	task_kill_gid (GID_STARFIELD);
}
