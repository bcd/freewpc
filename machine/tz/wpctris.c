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

/* CALLSET_SECTION (wpctris, __machine4__) */
#include <freewpc.h>

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define PIECE_BLOCKS 5 //piece matrix size
#define MAX_KEYS 6

enum {
	POS_FREE,
	POS_FILLED,
};

typedef enum {
	KEY_NONE,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_ROTATE,
	KEY_DROP,
} key_pressed_t;

enum {
	BM_LEFT,
	BM_RIGHT,
	BM_DOWN,
	BM_ROTATE,
};

key_pressed_t key_pressed[MAX_KEYS];
U8 key_counter;

U8 board[BOARD_WIDTH][BOARD_HEIGHT];
U8 game_ticks;
bool game_over;

struct live_piece_ops
{
	U8 x;
	U8 y;
	U8 type;
	U8 rotation;
} live_piece_state;

struct bm_poss_ops {
	U8 x;
	U8 y;
	U8 rotation;
} bm_poss_state;

struct gbt_ops
{
	U8 type;
	U8 rotation;
	U8 x;
	U8 y;
} gbt_state;


static const U8 pieces [7 /* type */ ][4 /* rotation */ ][5 /* horizontal blocks */ ][5 /* vertical blocks */ ] =
{
// Square
  {
   {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 2, 1, 0},
    {0, 0, 1, 1, 0},
    {0, 0, 0, 0, 0}
    },
   {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 2, 1, 0},
    {0, 0, 1, 1, 0},
    {0, 0, 0, 0, 0}
    },
   {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 2, 1, 0},
    {0, 0, 1, 1, 0},
    {0, 0, 0, 0, 0}
    },
   {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 2, 1, 0},
    {0, 0, 1, 1, 0},
    {0, 0, 0, 0, 0}
    }
   },

// I
  {
   {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 1, 2, 1, 1},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}
    },
   {
    {0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 2, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0}
    },
   {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {1, 1, 2, 1, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}
    },
   {
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 2, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0}
    }
   }
  ,
// L
  {
   {
    {0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 2, 0, 0},
    {0, 0, 1, 1, 0},
    {0, 0, 0, 0, 0}
    },
   {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 1, 2, 1, 0},
    {0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0}
    },
   {
    {0, 0, 0, 0, 0},
    {0, 1, 1, 0, 0},
    {0, 0, 2, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0}
    },
   {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0},
    {0, 1, 2, 1, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}
    }
   },
// L mirrored
  {
   {
    {0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 2, 0, 0},
    {0, 1, 1, 0, 0},
    {0, 0, 0, 0, 0}
    },
   {
    {0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0},
    {0, 1, 2, 1, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}
    },
   {
    {0, 0, 0, 0, 0},
    {0, 0, 1, 1, 0},
    {0, 0, 2, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0}
    },
   {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 1, 2, 1, 0},
    {0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0}
    }
   },
// N
  {
   {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0},
    {0, 0, 2, 1, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0}
    },
   {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 1, 2, 0, 0},
    {0, 0, 1, 1, 0},
    {0, 0, 0, 0, 0}
    },
   {
    {0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 1, 2, 0, 0},
    {0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0}
    },

   {
    {0, 0, 0, 0, 0},
    {0, 1, 1, 0, 0},
    {0, 0, 2, 1, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}
    }
   },
// N mirrored
  {
   {
    {0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 2, 1, 0},
    {0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0}
    },
   {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 2, 1, 0},
    {0, 1, 1, 0, 0},
    {0, 0, 0, 0, 0}
    },
   {
    {0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0},
    {0, 1, 2, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0}
    },
   {
    {0, 0, 0, 0, 0},
    {0, 0, 1, 1, 0},
    {0, 1, 2, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}
    }
   },
// T
  {
   {
    {0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 2, 1, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0}
    },
   {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 1, 2, 1, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0}
    },
   {
    {0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 1, 2, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0}
    },
   {
    {0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 1, 2, 1, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}
    }
   }
};

// Displacement of the piece to the position where it is first drawn in the board when it is created
static const S8 pieces_init_pos  [7 /* type */ ][4 /* rotation */ ][2 /* initial position */] =
{
/* Square */
  {
	{-2, -3},
    {-2, -3},
    {-2, -3},
    {-2, -3}
   },
/* I */
  {
	{-2, -2},
    {-2, -3},
    {-2, -2},
    {-2, -3}
   },
/* L */
  {
	{-2, -3},
    {-2, -3},
    {-2, -3},
    {-2, -2}
   },
/* L mirrored */
  {
	{-2, -3},
    {-2, -2},
    {-2, -3},
    {-2, -3}
   },
/* N */
  {
	{-2, -3},
    {-2, -3},
    {-2, -3},
    {-2, -2}
   },
/* N mirrored */
  {
	{-2, -3},
    {-2, -3},
    {-2, -3},
    {-2, -2}
   },
/* T */
  {
	{-2, -3},
    {-2, -3},
    {-2, -3},
    {-2, -2}
   },
};

static void clear_inputs (void)
{
	for (key_counter = 0; key_counter < MAX_KEYS; key_counter++)
	{
		key_pressed[key_counter] = KEY_NONE;
	}
	key_counter = 0;
}

static void set_input (key_pressed_t key)
{
	if (++key_counter > MAX_KEYS)
		key_counter = MAX_KEYS;
	key_pressed[key_counter] = key;
}

CALLSET_ENTRY (wpctris, sw_left_button)
{
	if (game_over)
		return;
	if (switch_poll_logical (SW_RIGHT_BUTTON))
		set_input (KEY_ROTATE);
	else
		set_input (KEY_LEFT);
}

CALLSET_ENTRY (wpctris, sw_right_button)
{
	if (game_over)
		return;
	if (switch_poll_logical (SW_LEFT_BUTTON))
		set_input (KEY_ROTATE);
	else
		set_input (KEY_RIGHT);
}

static U8 get_block_type (void)
{
	struct gbt_ops *sgbt = &gbt_state;
	return pieces [sgbt->type][sgbt->rotation][sgbt->x][sgbt->y];
}

static S8 get_init_xpos (U8 piece, U8 rotation)
{
	return pieces_init_pos [piece][rotation][0]; 
}

static S8 get_init_ypos (U8 piece, U8 rotation)
{
	return pieces_init_pos [piece][rotation][1]; 
}

static void board_init (void)
{
	U8 i;
	U8 j;
	for (i = 0; i < BOARD_WIDTH; i++)
	{
		for (j = 0; j < BOARD_HEIGHT; j++)
			board[i][j] = POS_FREE;
	}
}

static void board_store_piece (U8 x, U8 y, U8 type, U8 rotation)
{
	struct gbt_ops *sgbt = &gbt_state;
	sgbt->type = type;
	sgbt->rotation = rotation;
	U8 i1, i2, j1, j2;
	for (i1 = x, i2 = 0; i1 < x + PIECE_BLOCKS; i1++, i2++)
	{
		for (j1 = y, j2 = 0; j1 < y + PIECE_BLOCKS; j1++, j2++)
		{
			sgbt->x = j2;
			sgbt->y = i2;
			if (get_block_type () != 0)  
				board[i1][j1] = POS_FILLED;
		}
	}
}

static void board_delete_line (U8 y)
{
	U8 i, j;
	for (j = y; j > 0; j--)
	{
		for (i = 0; i < BOARD_WIDTH; i++)
		{
			board[i][j] = board[i][j-1];
		}
	}
}

static void board_delete_possible_lines (void)
{
	U8 i, j;
	for (j = 0; j < BOARD_HEIGHT; j++)
	{
		i = 0;
		while (i < BOARD_WIDTH)
		{
			if (board[i][j] != POS_FILLED)
				break;
			i++;
		}

		if (i == BOARD_WIDTH)
			board_delete_line (j);
	}
}

static bool board_block_is_free (U8 x, U8 y)
{
	if (board[x][y] == POS_FREE)
		return TRUE;
	else
		return FALSE;
}


static inline bool board_movement_is_possible (void)
{
	// Checks collision with pieces already stored in the board or the board limits
	// This is just to check the 5x5 blocks of a piece with the appropriate area in the board
	U8 i1, i2, j1, j2;

	gbt_state.type = live_piece_state.type;
	for (i1 = bm_poss_state.x, i2 = 0; i1 < bm_poss_state.x + PIECE_BLOCKS; i1++, i2++)
	{
		for (j1 = bm_poss_state.y, j2 = 0; j1 < bm_poss_state.y + PIECE_BLOCKS; j1++, j2++)
		{
			gbt_state.x = j2;
			gbt_state.y = i2;
			// Check if the piece is outside the limits of the board
			if (i1 > BOARD_WIDTH - 1 || j1 > BOARD_HEIGHT - 1)
			{
	//			if (get_block_type () != POS_FREE)
					return FALSE;
			}
			// Check if the piece have collisioned with a block already stored in the map
	//		if (get_block_type () != POS_FREE &&
	//				!board_block_is_free (i1, j1))
	//				return FALSE;
		}
	}

	// No collision
	return TRUE;
}

static bool game_over_check (void)
{
	U8 i;
	for (i = 0; i < BOARD_WIDTH; i++)
	{
		if (board[i][0] == POS_FILLED)
			return TRUE;
	}
	return FALSE;
}

static void create_new_piece (void)
{
	clear_inputs ();
	game_ticks = 0;
	live_piece_state.type = random_scaled (7);
	live_piece_state.rotation = random_scaled (4);
	live_piece_state.x = (BOARD_WIDTH / 2) + get_init_xpos (live_piece_state.type, live_piece_state.rotation);
	live_piece_state.y = 3 + get_init_ypos (live_piece_state.type, live_piece_state.rotation);
}


static inline bool board_movement_left (void)
{
	bm_poss_state.x = live_piece_state.x - 1;
	bm_poss_state.y = live_piece_state.y;
	bm_poss_state.rotation = live_piece_state.rotation;
	if (board_movement_is_possible ())
		return TRUE;
	else
		return FALSE;
}

static inline bool board_movement_right (void)
{
	bm_poss_state.x = live_piece_state.x + 1;
	bm_poss_state.y = live_piece_state.y;
	bm_poss_state.rotation = live_piece_state.rotation;
	if (board_movement_is_possible ())
		return TRUE;
	else
		return FALSE;
}

static inline bool board_movement_down (void)
{
	bm_poss_state.x = live_piece_state.x;
	bm_poss_state.y = live_piece_state.y + 1;
	bm_poss_state.rotation = live_piece_state.rotation;
	if (board_movement_is_possible ())
		return TRUE;
	else
		return FALSE;
}

static inline bool board_movement_rotate (void)
{
	bm_poss_state.x = live_piece_state.x;
	bm_poss_state.y = live_piece_state.y;
	bm_poss_state.rotation = (live_piece_state.rotation + 1) % 4;
	if (board_movement_is_possible ())
		return TRUE;
	else
		return FALSE;
}

static void move_piece_down (void)
{
	if (++game_ticks > 10)
	{
		if (board_movement_down ())
		{
			game_ticks = 0;
			live_piece_state.y++;
		}
		else
		{
			board_store_piece (live_piece_state.x, live_piece_state.y, live_piece_state.type, live_piece_state.rotation);
			board_delete_possible_lines ();
			if (game_over_check ())
			{
				game_over = TRUE;
				return;
			}
			create_new_piece ();
		}
	}
}


static void move_piece (void)
{
	while (key_counter > 0)
	{

		switch (key_pressed[key_counter])
		{
			case KEY_LEFT:
				if (board_movement_left ())
					live_piece_state.x--;
				break;
			case KEY_RIGHT:
				if (board_movement_right ())
					live_piece_state.x++;
				break;
			case KEY_ROTATE:
				if (board_movement_rotate ())
					if (++live_piece_state.rotation > 3)
						live_piece_state.rotation = 0;
				break;
			case KEY_DROP:
				while (board_movement_down ())
					live_piece_state.y++;
				board_store_piece (live_piece_state.x, live_piece_state.y - 1, live_piece_state.type, live_piece_state.rotation);
				board_delete_possible_lines ();
				if (game_over_check ())
				{
					game_over = TRUE;
					break;
				}
				create_new_piece ();
				break;
			default:
				break;
		}
		key_counter--;
		//draw_and_show_scene
	}
}

void wpctris_start (void)
{
	game_over = FALSE;
	board_init ();
	create_new_piece ();
}

static const U8 one_pixel[] = {
	1,1,1,
};

static void pixel_on (U8 x, U8 y)
{
	bitmap_blit (one_pixel, x, y);
}

static void draw_board (void)
{
	//U8 x, y;

}

static void draw_live_piece (void)
{
	struct gbt_ops *sgbt = &gbt_state;
	U8 i, j;
	for (i = 0; i < PIECE_BLOCKS; i++)
	{
		for (j = 0; j < PIECE_BLOCKS; j++)
		{
			sgbt->type = live_piece_state.type;
			sgbt->rotation = live_piece_state.rotation;
			sgbt->x = i;
			sgbt->y = j;
			if (get_block_type () != 0)
				pixel_on (live_piece_state.x + i, live_piece_state.y + j);
		}
	}
}

static inline void wpctris_game_loop (void)
{
	while (!game_over)
	{
		dmd_alloc_pair_clean ();
	//	draw_board ();
		draw_live_piece ();
		move_piece ();
		move_piece_down ();
		dmd_show2 ();
		task_sleep (TIME_100MS);
	//	task_sleep_sec (5);
	}	
}

void wpctris_deff (void)
{
	wpctris_start ();
	wpctris_game_loop ();
	deff_exit ();
}


