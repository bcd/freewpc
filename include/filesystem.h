/*
 * Copyright 2011 by Brian Dominy <brian@oddchange.com>
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

#define __dirtab__ __attribute__((section ("dirtab")))

#define MAX_FILE_INFO 32

typedef U16 size_t;

/* A list of 'filenames', which are just values that denote the contents of
   a particular region of NVRAM.  It is extremely important that filename
	IDs do not change over time!  Always add values to the end, and don't
	delete them unless you are careful. */
enum file_type
{
	FT_NONE = 0,
	FT_ADJUST,
	FT_AUDIT,
	FT_TIMESTAMP,
	FT_VOLUME,
	FT_COIN,
	FT_LOCALE,
	FT_LICENSE,
	FT_HIGHSCORE,
	FT_REPLAY,
	FT_VERSION,
	FT_ROTEST,
};

struct file_info
{
	enum file_type type;
	U8 attr : 4;
	U8 version : 4;
	void *data;
	size_t len;
	U16 csum;
};


void file_init (void);
void file_reset (void);

