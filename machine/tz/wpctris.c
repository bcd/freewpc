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

enum {
	POS_FREE,
	POS_FILLED,
};

enum key_pressed {
	KEY_NONE,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_ROTATE,
	KEY_DROP,
};

U8 board[BOARD_WIDTH][BOARD_HEIGHT];
U8 game_ticks;

struct live_piece_states live_piece_state ={
	U8 x;
	U8 y;
	U8 type;
	U8 rotation;
};

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
static const U8 pieces_init_pos  [7 /* type */ ][4 /* rotation */ ][2 /* initial position */] =
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

static U8 get_block_type (U8 piece, U8 rotation, U8 x, U8 y)
{
	return pieces [piece][rotation][x][y];
}

static U8 get_init_xpos (U8 piece, U8 rotation)
{
	return pieces_init_pos [piece][rotation][0]; 
}

static U8 get_init_ypos (U8 piece, U8 rotation)
{
	return pieces_init_pos [piece][rotation][1]; 
}

static void board_init (void)
{
	U8 i;
	U8 j;
	for (i = 0; i < BOARD_WIDTH;i++)
	{
		for (j = 0; j < BOARD_HEIGHT; j++)
			board[i][j] = POS_FREE;
	}
}

static void board_store_piece (U8 x, U8 y, U8 piece, U8 rotation)
{
	U8 i1, i2, j1, j2;
	for (i1 = x, i2 = 0; i1 < x + PIECE_BLOCKS; i1++, i2++)
	{
		for (j1 = y, j2 = 0; j1 < y + PIECE_BLOCKS; j1++, j2++)
		{
			if (get_block_type (piece, rotation, j2, i2) != 0)  
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

static board_delete_possible_lines (void)
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

static bool board_movement_is_possible (U8 x, U8 y, U8 piece, U8 rotation)
{
	U8 i1, i2, j1, j2;
	for (i1 = x, i2 = 0; i1 < x + PIECE_BLOCKS; i1++, i2++)
	{
		for (j1 = y, j2 = 0; j1 < y + PIECE_BLOCKS; j1++, j2++)
		{
			if (i1 < 0 || i1 > BOARD_WIDTH - 1
					||  j1 > BOARD_HEIGHT - 1)
			{
				if (get_block_type (piece, rotation, j2, i2) != 0)
					return FALSE;
			}  
			if (j1 >= 0)  
            		{
				if ((get_block_type (piece, rotation, j2, i2) != 0)
						&& (!board_block_is_free (i1, j1)))
					return FALSE;
			}
		}
	}  
	return true;  
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
	game_ticks = 0;
	live_piece_state->piece = random_scaled (7);
	live_piece_state->rotation = random_scaled (4);
	live_piece_state->x = (BOARD_WIDTH / 2) + get_init_xpos (live_piece_state->piece, live_piece_state->rotation);
	live_piece_state->y = get_init_ypos (live_piece_state->piece, live_piece_state->rotation);
}

static void move_piece_down (void)
{
	if (++game_ticks > 10)
	{
		if (board_movement_is_possible (live_piece_state->x, live_piece_state->y + 1, live_piece_state->piece, live_piece_state->rotation))
		{
			live_piece_state->y++;
		}
		else
		{
			board_store_piece (live_piece_state->x, live_piece_state->y, live_piece_state->piece, live_piece_state->rotation);
			board_delete_possible_lines ();
			if (game_over_check ())
			{
				game_over = TRUE;
				break;
			}
			create_new_piece ();
		}
	}
}


static void move_piece (void)
{
	switch (key_pressed)
	{
		case KEY_LEFT:
			if (board_movement_is_possible (live_piece_state->x - 1, live_piece_state->y, live_piece_state->piece, live_piece_state->rotation))
				live_piece_state->x--;
			break;
		case KEY_RIGHT:
			if (board_movement_is_possible (live_piece_state->x + 1, live_piece_state->y, live_piece_state->piece, live_piece_state->rotation))
				live_piece_state->x++;
			break;
		case KEY_ROTATE:
			if (board_movement_is_possible (live_piece_state->x, live_piece_state->y, live_piece_state->piece, (live_piece_state->rotation + 1) % 4))
				live_piece_state->rotation = (live_piece_state->rotation + 1) % 4;
			break;
		case KEY_DROP:
			while (board_movement_is_possible (live_piece_state->x, live_piece_state->y, live_piece_state->piece, live_piece_state->rotation))
				live_piece_state->y++;
			board_store_piece (live_piece_state->x, live_piece_state->y - 1, live_piece_state->piece, live_piece_state->rotation);
			board_delete_possible_lines ();
			if (game_over_check ())
			{
				game_over = TRUE;
				break;
			}
			create_new_piece ();
			break;
	}
}

void wpctris_game_loop (void)
{
	draw_board ();
	draw_piece ();
	read_input ();
	move_piece ();
	move_piece_down ();
}

