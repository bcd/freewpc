
#include <freewpc.h>

#define VMODE_RATE TIME_50MS
#define VMODE_PLAYER_WIDTH 5
#define MAX_PLAYER_BULLETS 3
#define MAX_ENEMY_BULLETS 5

#define MINX 0
#define MINY 0
#define MAXX 127
#define MAXY 31

typedef U16 object_properties_t;

#define PROP_WIDTH(props)                (props & 0xFF)
#define PROP_HEIGHT(props)               ((props & 0xFF00UL) >> 8)
#define MKPROPS(width,height)            (((U16)height << 8) | width)

#define OBJ_FREE(coord,props)            do { coord.xy = 0xFFFF; } while (0)
#define OBJ_EXISTS_P(coord,props)        (coord.xy != 0xFFFF)
#define OBJ_SET(coord,props,_x,_y)       do { coord.x = _x; coord.y = _y; } while (0)

#define OBJ_MOVE_LEFT(coord,props,n)     if (coord.x > MINX+n-1) coord.x -= n;
#define OBJ_MOVE_RIGHT(coord,props,n)    if (coord.x <= MAXX-n+1-PROP_WIDTH(props)) coord.x += n;
#define OBJ_MOVE_UP(coord,props,n)       if (coord.y > MINY+n-1) coord.y -= n;
#define OBJ_MOVE_DOWN(coord,props,n)     if (coord.y <= MAXY-n+1-PROP_HEIGHT(props)) coord.y -= n;

#define OBJ_MOVE_HORIZ(coord,props,n) \
	do { if (n < 0) OBJ_MOVE_LEFT(coord,props,-n); else OBJ_MOVE_RIGHT(coord,props,n); } while (0)

#define OBJ_MOVE_VERT(coord,props,n) \
	do { if (n < 0) OBJ_MOVE_UP(coord,props,-n); else OBJ_MOVE_DOWN(coord,props,n); } while (0)

#define OBJ_MOVE(coord,props,x,y) \
	do { OBJ_MOVE_HORIZ(coord,props,x); OBJ_MOVE_VERT(coord,props,y); } while (0)

#define PLAYER_PROPS                    MKPROPS(5,5)

bool vmode_active;
union dmd_coordinate vmode_player_coord;
union dmd_coordinate vmode_enemy_coord[8];
U8 vmode_jump_state;
union dmd_coordinate vmode_player_bullet_coord[MAX_PLAYER_BULLETS];
union dmd_coordinate vmode_enemy_bullet_coord[MAX_ENEMY_BULLETS];
U8 vmode_slider_pos;


static const U8 bytepos[] = { 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80 };

void pixel_on (union dmd_coordinate coord)
{
	register U8 *dmd;
	
	dmd = dmd_low_buffer;
	dmd += DMD_BYTE_WIDTH * coord.y;
	dmd += coord.x / 8;
	*dmd |= bytepos[coord.x % 8];
}


CALLSET_ENTRY (vmode, sw_left_button)
{
	if (vmode_active)
	{
		while (switch_poll_logical (SW_L_L_FLIPPER_BUTTON))
		{
			OBJ_MOVE_LEFT (vmode_player_coord, PLAYER_PROPS, 1);
			task_sleep (TIME_16MS);
		}
	}
}


CALLSET_ENTRY (vmode, sw_right_button)
{
	if (vmode_active)
	{
		while (switch_poll_logical (SW_L_R_FLIPPER_BUTTON))
		{
			OBJ_MOVE_RIGHT (vmode_player_coord, PLAYER_PROPS, 1);
			task_sleep (TIME_16MS);
		}
	}
}

CALLSET_ENTRY (vmode, sw_start_button)
{
	if (vmode_active)
	{
		/* Allocate a bullet */
		sound_send (SND_SLINGSHOT);
		task_sleep (VMODE_RATE * 2);
	}
}


void vmode_redraw (void)
{
	dmd_alloc_low_clean ();
	bitmap_draw (vmode_player_coord, BM_X5);
	dmd_show_low ();
}


void vmode_update (void)
{
	/* Update player bullet trajectories (straight up) */
	/* Update enemy bullet trajectories (straight down) */
	/* Update enemy positions */
	/* Update slider position */
}


void video_mode_deff (void)
{
#if 0
	while (vmode_active)
	{
		vmode_redraw ();
		vmode_update ();
		task_sleep (VMODE_RATE);
	}
#endif
	deff_exit ();
}


void vmode_start (void)
{
	vmode_active = TRUE;
	OBJ_SET (vmode_player_coord, PLAYER_PROPS, 63, 26);
	vmode_jump_state = 0;
	deff_start (DEFF_VIDEO_MODE);
}

void vmode_stop (void)
{
	deff_stop (DEFF_VIDEO_MODE);
}

