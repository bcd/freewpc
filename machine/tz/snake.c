
/*
 * Copyright 2011 by Ewan Meadows <sonny_jim@hotmail.com>
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

/* CALLSET_SECTION (snake, __machine3__) */

#include <freewpc.h>
#include <search.h>

#define GRID_WIDTH 32 // 128 / 4
#define GRID_HEIGHT 8 // 32 / 4
#define MAX_BENDS 64
#define MAX_FOOD 7
#define INITIAL_LENGTH 4
#define MAX_LENGTH 64
#define NUM_SPEED_LEVELS 8
#define GAME_LENGTH 10 //seconds 

typedef enum {
	NONE=0,
	LEFT,
	UP,
	RIGHT,
	DOWN,
} snake_dir_t;

snake_dir_t button_sequence[MAX_BENDS];
U8 food_collected;
U8 counter;
U8 tail_counter;
U8 button_counter;
/* stored as [x],[y] coords */
U8 food_locations_x[MAX_FOOD];
U8 food_locations_y[MAX_FOOD];

struct Snake_Data {
U8 length;
U8 head_x; // Stores Head Coordinate
U8 head_y; // Stores Head Coordinate
snake_dir_t head_dir; // Stores Head Direction
U8 tail_x; // Stores Tail Coordinate
U8 tail_y; // Stores Tail Coordinate
snake_dir_t tail_dir; // Stores Tail Direction
snake_dir_t bend_x[MAX_BENDS];// Stores Bend x,y
snake_dir_t bend_y[MAX_BENDS];// Stores Bend x,y
snake_dir_t bend_dir[MAX_BENDS];// Stores Bend direction
bool dead;
bool moved;
U8 sleep; //Current wait setting
U8 timer; //How many seconds of play left
score_t score;
} Snake;

 const U8 snake_head_bitmaps[] = {
	/*
	3,3,3,7,3, 
	3,3,7,7,2, 
	3,3,6,7,6, 
	3,3,2,7,7, 
	*/
	// Up
	4,4,14,15,15,14,
	// Right
	4,4,6,15,15,15,
	// Down
	4,4,7,15,15,7,
	// Left
	4,4,15,15,15,6,

};

 const U8 snake_body_bitmap[] = {
	4,4,0,6,6,0,
};

 const U8 snake_tail_bitmaps[] = {
	4,4,8,15,15,8,
	4,4,6,6,6,15,
	4,4,1,15,15,1,
	4,4,15,6,6,6,
};

 const U8 food_bitmap[] = {
	4,4,9,6,6,9
};

static inline void increase_time (void)
{
	if (food_collected < 10)
		bounded_increment (Snake.timer, GAME_LENGTH);	
	else if (food_collected < 40)
		bounded_increment (Snake.timer, GAME_LENGTH / 2);
	else
		bounded_increment (Snake.timer, 2);	
}

static inline void increase_speed (void)
{
	if (food_collected < 40)
		bounded_decrement (Snake.sleep, 2);
	else	
		bounded_decrement (Snake.sleep, 1);
}

static inline void increase_length (void)
{
	/* Fnar fnar */
	if (Snake.length >= MAX_LENGTH)
		return;
	Snake.length++;
	switch (Snake.tail_dir)
	{
		case LEFT:
			Snake.tail_x++;
			break;
		case RIGHT:
			Snake.tail_x--;
			break;
		case UP:
			Snake.tail_y++;
			break;
		case DOWN:
			Snake.tail_y--;
			break;
		case NONE:
			break;
	}
}

static inline void increase_score (void)
{
	if (food_collected < 10)
		score_add (Snake.score, score_table[SC_1M]);
	else if (food_collected < 40)
		score_add (Snake.score, score_table[SC_250K]);
	else
	{
		#ifdef MACHINE_TZ
		sound_send (SND_KACHING);
		#endif
		score_add (Snake.score, score_table[SC_5M]);
	}
}

static void pick_food (U8 food_no)
{
	U8 old_food_x = food_locations_x[food_no];
	U8 old_food_y = food_locations_y[food_no];
	/* Don't pick the same place twice */
	while (old_food_x == food_locations_x[food_no]
		&& old_food_y == food_locations_y[food_no])
	{
		food_locations_x[food_no] = 3 + (3 * food_no);
		food_locations_y[food_no] = 2 + random_scaled(6);
	}
}

static void food_init (void)
{
	U8 i;
	for (i=0;i < MAX_FOOD; i++)
	{	
		pick_food (i);
	}
}

static void draw_food (void)
{
	U8 i;
	for (i=0;i < MAX_FOOD; i++)
	{	
		bitmap_blit (food_bitmap, food_locations_x[i] * 4, food_locations_y[i] * 4);     
	}
}

static void flash_nyom (void)
{
	U8 i;
	for (i=0; i < 4; i++)
	{
		if (i % 2 != 0)
			sound_send (0x52);
		font_render_string_center (&font_var5, Snake.head_x * 4, Snake.head_y * 4, "NYOM");
		dmd_show_low ();
		task_sleep (TIME_33MS);
		dmd_invert_page (dmd_low_buffer);
	}

}

static void check_food (void)
{
	U8 i;
	for (i=0;i < MAX_FOOD; i++)
	{
		if (((Snake.head_x >= food_locations_x[i]) && (Snake.head_x <= food_locations_x[i]))
			&& 
			((Snake.head_y >= food_locations_y[i]) && (Snake.head_y <= food_locations_y[i])))
		{
			bounded_increment (food_collected, 50);
			if (food_collected == 50)
				Snake.dead = TRUE;
			/* Pick a new location */
			pick_food (i);
			increase_length ();
			increase_speed ();
			increase_time ();
			increase_score ();
			flash_nyom ();
		}
	}
}


static void draw_snake_head (void)
{
	bitmap_blit (snake_head_bitmaps + (Snake.head_dir - 1) * 6, Snake.head_x * 4, Snake.head_y * 4 );     
}

static void draw_snake_body (void)
{
	U8 body_x = Snake.tail_x;
	U8 body_y = Snake.tail_y;
	U8 body_counter = tail_counter;
	snake_dir_t body_dir = Snake.tail_dir;
	U8 i;
	for (i = 1; i <= Snake.length - 1; i++)
	{
		switch (body_dir)
		{
			case LEFT:
				body_x--;
				break;
			case RIGHT:
				body_x++;
				break;
			case UP:
				body_y--;
				break;
			case DOWN:
				body_y++;
				break;
			case NONE:
				break;
		}
		if (feature_config.snake_wrap == YES)
		{
			if (body_x == 0 && body_dir == LEFT)
			body_x = GRID_WIDTH;
			else if (body_x >= GRID_WIDTH && body_dir == RIGHT)
			body_x = 0;
	
			if (body_y == 0 && body_dir == UP)
			body_y = GRID_HEIGHT;
			else if (body_y >= GRID_HEIGHT && body_dir == DOWN)
			body_y = 0;
		}
		
		if (body_x == Snake.bend_x[body_counter]
			&& body_y == Snake.bend_y[body_counter])
		{
			body_dir = Snake.bend_dir[body_counter];
			if (body_counter++ >= MAX_BENDS)
				body_counter = 0;
		}
		/* Check head hasn't hit the body */
		if (body_x == Snake.head_x && body_y == Snake.head_y)
			Snake.dead = TRUE;
		bitmap_blit (snake_body_bitmap, body_x * 4, body_y * 4);     
	}

}

static void draw_snake_tail (void)
{
	bitmap_blit (snake_tail_bitmaps + (Snake.tail_dir - 1) * 6, Snake.tail_x * 4, Snake.tail_y * 4 );     
}

static inline void input_read (void)
{
   	if ( tail_counter > MAX_BENDS) 
		tail_counter = 0;
	if ( counter > MAX_BENDS) 
		counter = 0;
	
	if (button_sequence[counter] != NONE)
	{
		/* Rotate left -> Down -> Right -> Up */
		if (button_sequence[counter] == LEFT)
		{
			if (Snake.head_dir == LEFT)
				Snake.head_dir = DOWN;
			else
				Snake.head_dir--;
		}
		if (button_sequence[counter] == RIGHT)
		{
			if (Snake.head_dir == DOWN)
				Snake.head_dir = LEFT;
			else
				Snake.head_dir++;
		}
		/* Store bend in array */
		Snake.bend_x[counter] = Snake.head_x;
		Snake.bend_y[counter] = Snake.head_y;
		Snake.bend_dir[counter] = Snake.head_dir;
		counter++;
	}
}

static inline void check_tail (void)
{
	/* Code to change the direction at respective time */
	if (Snake.tail_x == Snake.bend_x[tail_counter]
		&& Snake.tail_y == Snake.bend_y[tail_counter])
	{
		Snake.tail_dir = Snake.bend_dir[tail_counter];
		tail_counter++;
	}
}

static void wrap_snake_boundary (void)
{
	if (Snake.head_x == 0 && Snake.head_dir == LEFT)
		Snake.head_x = GRID_WIDTH;
	else if (Snake.head_x >= GRID_WIDTH && Snake.head_dir == RIGHT)
		Snake.head_x = 0;

	if (Snake.head_y == 0 && Snake.head_dir == UP)
		Snake.head_y = GRID_HEIGHT;
	else if (Snake.head_y >= GRID_HEIGHT && Snake.head_dir == DOWN)
		Snake.head_y = 0;
	
	if (Snake.tail_x == 0 && Snake.tail_dir == LEFT)
		Snake.tail_x = GRID_WIDTH;
	else if (Snake.tail_x >= GRID_WIDTH && Snake.tail_dir == RIGHT)
		Snake.tail_x = 0;

	if (Snake.tail_y == 0 && Snake.tail_dir == UP)
		Snake.tail_y = GRID_HEIGHT;
	else if (Snake.tail_y >= GRID_HEIGHT && Snake.tail_dir == DOWN)
		Snake.tail_y = 0;
}

static inline void check_snake_boundary (void)
{
	if (Snake.head_x == 0 || Snake.head_x > GRID_WIDTH
		|| Snake.head_y == 0 || Snake.head_y > GRID_HEIGHT)
		Snake.dead = TRUE;
}


static inline void boundary_check (void)
{
	if (feature_config.snake_wrap == YES)
			wrap_snake_boundary ();
		else
		{
			dmd_draw_border (dmd_low_buffer);
			check_snake_boundary ();
		}
}


static inline void move_snake (void)
{
	switch (Snake.head_dir)
	{
		case LEFT:
			Snake.head_x--;
			break;
		case RIGHT:
			Snake.head_x++;
			break;
		case UP:
			Snake.head_y--;
			break;
		case DOWN:
			Snake.head_y++;
			break;
		case NONE:
			break;
	}
	
	switch (Snake.tail_dir)
	{
		case LEFT:
			Snake.tail_x--;
			break;
		case RIGHT:
			Snake.tail_x++;
			break;
		case UP:
			Snake.tail_y--;
			break;
		case DOWN:
			Snake.tail_y++;
			break;
		case NONE:
			break;
	}
	boundary_check ();
	Snake.moved = TRUE;
}

static void snake_timer_task (void)
{	
	for (Snake.timer = GAME_LENGTH; Snake.timer > 0; Snake.timer--)
	{
		task_sleep_sec (1);
	}
	Snake.dead = TRUE;
	task_exit ();
}

static inline void initialise_game (void)
{
	/* Init */
	U8 i;
	task_recreate_gid (GID_SNAKE_TIMER, snake_timer_task);
	Snake.timer = GAME_LENGTH;
	Snake.dead = FALSE;
	Snake.length = INITIAL_LENGTH;
	Snake.head_x = GRID_WIDTH / 2;
	Snake.head_y = GRID_HEIGHT / 2;
	Snake.head_dir = RIGHT;
	Snake.tail_x = Snake.head_x - Snake.length;
	Snake.tail_y = Snake.head_y;
	Snake.tail_dir = RIGHT;
	Snake.moved = FALSE;
	Snake.sleep = NUM_SPEED_LEVELS;
	score_zero (Snake.score);
	counter = 0;
	button_counter = 0;
	tail_counter = 0;
	/* Reset bends */
	for (i = 0; i < MAX_BENDS; i++)
	{
		Snake.bend_x[i] = 0;
		Snake.bend_y[i] = 0;
		Snake.bend_dir[i] = 0;
		button_sequence[i] = NONE;
	}
	food_collected = 0;
	food_init ();
}

static void snake_sleep (void)
{
	U8 i;
	for (i = 0; i < Snake.sleep; i++)
	{
		task_sleep (TIME_33MS);
	}
}

static inline  void draw_timer (void)
{
	sprintf ("%d", Snake.timer);
	font_render_string_right (&font_var5, 126, 2, sprintf_buffer);
	font_render_string_right (&font_var5, 118, 2, "TIME:");
	sprintf ("%d", food_collected);
	font_render_string_right (&font_fixed10, 126, 16, sprintf_buffer);
}

static void draw_snake (void)
{
	draw_snake_head ();
	draw_snake_tail ();
	draw_snake_body ();
}

void snake_ready_deff (void)
{
	U16 fno;
	dmd_alloc_pair_clean ();
	U8 i;
	for (i = 0;i < 4;i++)
	{
		for (fno = IMG_SNAKE_START; fno <= IMG_SNAKE_END; fno += 2)
		{
			dmd_map_overlay ();
			dmd_clean_page_low ();
			font_render_string_right (&font_var5, 128, 2, "VIDEO MODE READY");
			if (fno % 5 == 0 || fno >= IMG_SNAKE_END)
			{
				sound_send (0x54);
				font_render_string_right (&font_var5, 128, 16, "SHOOT LOCK");
			}
			dmd_text_outline ();
			dmd_alloc_pair ();
			frame_draw (fno);
			dmd_overlay_outline ();
			dmd_show2 ();
			task_sleep (TIME_33MS);
		}
	}
	task_sleep (TIME_700MS);
	deff_exit ();
}

void snake_deff (void)
{
	initialise_game ();
	//pause system timer
	U16 fno;
	/* Draw */
	sound_send (SND_THINGS_ARE_NOT_WHAT);
	dmd_alloc_pair_clean ();
	U8 i;
	for (i = 0;i < 15;i++)
	{
		for (fno = IMG_SNAKE_START; fno <= IMG_SNAKE_END; fno += 2)
		{
			dmd_map_overlay ();
			dmd_clean_page_low ();
			font_render_string_right (&font_var5, 128, 2, "VIDEO MODE");
			font_render_string_right (&font_var5, 128, 10, "EAT X");
			font_render_string_right (&font_var5, 128, 20, "AVOID TAIL");
			dmd_text_outline ();
			dmd_alloc_pair ();
			frame_draw (fno);
			dmd_overlay_outline ();
			dmd_show2 ();
		}
	}
	dmd_alloc_pair_clean ();
	
	while (Snake.dead == FALSE)
	{	
		dmd_alloc_low_clean ();
		check_food ();
		move_snake ();
		check_tail ();
		draw_food ();
		draw_snake ();
		draw_timer ();
		input_read ();
		ball_search_timer_reset ();

		//boundary_check ();
		//check_tail ();
		if (Snake.timer > 3)
			sound_send (0x54);
		else if (Snake.timer > 1)
			sound_send (0x58);
		else
			sound_send (0x59);
		dmd_show_low ();
		snake_sleep ();
	}
	task_kill_gid (GID_SNAKE_TIMER);
	#ifdef MACHINE_TZ
	sound_send (SND_EXPLOSION_3);
	dmd_alloc_pair_clean ();
	for (fno = IMG_EXPLODE_START; fno <= IMG_EXPLODE_END; fno += 2)
	{
		frame_draw (fno);
		dmd_show2 ();
		task_sleep (TIME_66MS);
	}
	#endif
	task_sleep (TIME_700MS);
	
	music_update ();
	dmd_alloc_pair_clean ();
	psprintf ("1 NYOM NYOM", "%d NYOM NYOMS", food_collected);
	font_render_string_center (&font_fixed6, 64, 7, sprintf_buffer);
	sprintf_score (Snake.score);
	font_render_string_center (&font_fixed6, 64, 18, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}

CALLSET_ENTRY (snake, init)
{
	Snake.dead = TRUE;
}

CALLSET_ENTRY (snake, start_player)
{
	flag_off (FLAG_SNAKE_READY);
}

CALLSET_ENTRY (snake, sw_left_button)
{
	if (Snake.dead == FALSE)
	{
		sound_send (0x51);
		button_sequence[button_counter] = LEFT;
		if (button_counter++ >= MAX_BENDS)
			button_counter = 0;
	}
}

CALLSET_ENTRY (snake, sw_right_button)
{
	if (Snake.dead == FALSE)
	{
		sound_send (0x51);
		button_sequence[button_counter] = RIGHT;
		if (button_counter++ >= MAX_BENDS)
			button_counter = 0;
	}
}

CALLSET_ENTRY (snake, music_refresh)
{
	if (deff_get_active () != DEFF_SNAKE || Snake.dead == TRUE)
		return;
	
	if (food_collected < 10)
		music_request (MUS_CLOCK_CHAOS1, PRI_GAME_VMODE);
	else if (food_collected < 20)
		music_request (MUS_CLOCK_CHAOS2, PRI_GAME_VMODE);
	if (food_collected < 30)
		music_request (MUS_CLOCK_CHAOS3, PRI_GAME_VMODE);
	if (food_collected < 40)
		music_request (MUS_CLOCK_CHAOS4, PRI_GAME_VMODE);
	else
		music_request (MUS_CLOCK_CHAOS5, PRI_GAME_VMODE);
		
}

CALLSET_ENTRY (snake, snake_start)
{
	flipper_disable ();
	music_request (MUS_CLOCK_CHAOS1, PRI_GAME_VMODE);
	leff_start (LEFF_BONUS);
	deff_start_sync (DEFF_SNAKE);
	leff_stop (LEFF_BONUS);
	callset_invoke (snake_end);
}

CALLSET_ENTRY (snake, snake_end)
{
	flag_off (FLAG_SNAKE_READY);
	score_add (current_score, Snake.score);
	flipper_enable ();
	if (!can_lock_ball ())
		deff_start_sync (DEFF_BALL_FROM_LOCK);
}
