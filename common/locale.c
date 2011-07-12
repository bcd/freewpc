/*
 * Copyright 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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

/**
 * \file
 * \brief Handle locale-specific things, such as the formatting of dates/times
 * and the names of the pricing modes.  This module reads the DIP switches
 * or jumpers on the CPU board to determine how to act.
 */

#include <freewpc.h>
#include <preset.h>

/** The last setting of the DIP switches seen in init time */
__nvram__ U8 locale_code;

/** A checksum for the DIP switches to prove that it is valid */
__nvram__ U8 locale_code_csum;

static const char *month_names[] = {
	"JAN.", "FEB.", "MAR.", "APR.", "MAY", "JUN.",
	"JUL.", "AUG.", "SEP.", "OCT.", "NOV.", "DEC.",
};


const char *locale_names[] = {
	"ERROR", "FRANCE", "GERMANY", "FRANCE 20F",
	"INVALID", "INVALID", "INVALID", "GERMANY 2",
	"INVALID", "FRANCE 3", "EXPORT", "FRANCE 4",
	"UNITED KINGDOM", "EUROPE", "SPAIN", "USA/CANADA",
};

/* TODO - option to render in short form for audit display */

void locale_render_date (U8 month, U8 day, U16 year)
{
	switch (system_config.date_style)
	{
		case DATE_STYLE_US:
		default:
			sprintf ("%s %d, %ld", month_names[month-1], day, year);
			break;

		case DATE_STYLE_EURO:
			sprintf ("%d %s %ld", day, month_names[month-1], year);
			break;
	}
}


void render_build_date (void)
{
	locale_render_date (BUILD_MONTH, BUILD_DAY, BUILD_YEAR);
}


void locale_render (U8 locale)
{
	sprintf (locale_names[locale]);
}


CALLSET_ENTRY (locale, init)
{
	U8 current_locale;

	/* Read the DIP switch settings */
	current_locale = pinio_read_locale ();

	/* If the DIP switch setting changed, or the previous
	value was corrupted somehow, then perform a factory reset
	according to the current setting. */
	if (current_locale != locale_code
		|| ~locale_code != locale_code_csum)
	{
#ifdef CONFIG_TEST
		/* Install locale-specific adjustments */
		preset_install_country_code (current_locale);
#endif
		/* Save the current locale so that install does not
		need to be performed on the next reboot, unless
		the DIP switches are changed. */
		pinio_nvram_unlock ();
		locale_code = current_locale;
		locale_code_csum = ~current_locale;
		pinio_nvram_lock ();
	}
}

