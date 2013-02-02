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

#include <freewpc.h>
#include <flex.h>


static void flex_recalc (__fardata__ const struct flex_config *fconf)
{
	U8 games_wanted;
	U8 games;
	U8 level;
	U8 min_level;
	U8 max_level;
	U8 *adj_percent;
	U8 frequency;
	U8 n;
	struct flex_data *fdata;

	far_read_access ();
	fdata = far_read (fconf, data);
	min_level = far_read (fconf, min_level);
	max_level = far_read (fconf, max_level);
	adj_percent = far_read (fconf, adj_percent);
	frequency = far_read (fconf, frequency);

	dbprintf ("Recalc flex\n");
	dbprintf ("percentage=%d\n", *adj_percent);
	games_wanted = (100 - *adj_percent * frequency) / 100UL;
	dbprintf ("wanted=%d of %d\n", games_wanted, frequency);

	/* Start by counting the number of games played where the player
	did not reach the min level for the adjustment. */
	games = 0;
	for (n=0; n < fdata->games; n++)
		if (fdata->history[n] <= min_level)
			games++;

	/* Now scan the numbers of games played at each of the levels,
	and stop as soon as the desired count is seen. */
	for (level = min_level+1; level <= max_level; level++)
	{
		for (n=0; n < fdata->games; n++)
			if (fdata->history[n] == level)
				games++;
		if (games >= games_wanted)
			break;
	}

	/* Set the flex value to level-1 */
	pinio_nvram_unlock ();
	fdata->level = level-1;
	fdata->games = 0;
	csum_area_update (far_read (fconf, csum)); // FIXME
	pinio_nvram_lock ();
}

void flex_end_game (__fardata__ const struct flex_config *fconf)
{
	U8 *adj_percent;

	far_read_access ();
	adj_percent = far_read (fconf, adj_percent);
	if (adj_percent && *adj_percent)
	{
		struct flex_data *fdata = far_read (fconf, data);
		U8 frequency = far_read (fconf, frequency);
		if (fdata->games >= frequency)
		{
			flex_recalc (fconf);
		}
	}
}

void flex_end_player (__fardata__ const struct flex_config *fconf, U8 value)
{
	struct flex_data *fdata;

	far_read_access ();
	fdata = far_read (fconf, data);
	if (fdata->games < MAX_FLEX_GAMES)
	{
		pinio_nvram_unlock ();
		fdata->history[fdata->games++] = value;
		csum_area_update (far_read (fconf, csum));
		pinio_nvram_lock ();
		dbprintf ("Logging flex #%d %d, level is %d\n",
			fdata->games, value, fdata->level);
	}
}

void flex_reset (__fardata__ const struct flex_config *fconf)
{
	struct flex_data *fdata;
	U8 *adj_level;

	far_read_access ();
	fdata = far_read (fconf, data);
	dbprintf ("fdata=%p\n", fdata);
	adj_level = far_read (fconf, adj_level);
	dbprintf ("adj_level=%p\n", adj_level);
	if (adj_level)
		fdata->level = *(adj_level);
	else
		fdata->level = far_read (fconf, min_level);
	fdata->games = 0;
	dbprintf ("Flex reset to %d\n", fdata->level);
}

