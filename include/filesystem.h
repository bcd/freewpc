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

#ifdef __m6809__
#define __dirtab__ __attribute__((section ("dirtab")))
#else
#define __dirtab__ __nvram__
#endif

#define MAX_FILE_INFO 32


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
	FT_DATE,
	FT_FLEX1,
	FT_FLEX2,
	FT_FLEX3,
};


/** Describes an area of protected memory that should be checksummed.
The data area and the checksum itself need not be contiguous.
A reset function is provided which says what to do to the block
if checksum validation fails. */
struct area_csum
{
	/** The name of the file that is described here */
	enum file_type type;

	/** A pointer to the beginning of the block */
	/* __nvram__ */ U8 *area;

	/** The length of the protected block, in bytes */
	U8 length;

	/** A pointer to the variable that actually holds the checksum */
	/* __nvram__ */ U8 *csum;

	/** A version identifier for the structure */
	U8 version;

	/** A function that will reset the block to factory defaults.
	    This must reside within the same page as the caller to the
		 csum module. */
	void (*reset) (void);
};



struct file_info
{
	enum file_type type;
	U8 attr : 4;
	U8 version : 4;
	void *data;
	size_t len;
	U8 csum;
	U8 reserved;
};


struct file_info *file_find (enum file_type type);
void file_init (void);
void file_reset (void);
void file_register (const struct area_csum *csi);

void csum_area_update (const struct area_csum *csi);
void csum_area_reset (const struct area_csum *csi);
void csum_area_check (const struct area_csum *csi);
