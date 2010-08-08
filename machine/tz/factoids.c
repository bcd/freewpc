/*
 * Copyright 2006-2010 by Ewan Meadows <sonny_jim@hotmail.com>
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

/* CALLSET_SECTION (factoids, __machine3__) */

#include <freewpc.h>
#define NUM_FACTOIDS 19

const struct {
	const char *line1;
	const char *line2;
	const char *line3;
	const char *line4;
} tz_factoids[] = {
	{ "THE TALKING TINA DOLL", "IS FROM THE EPISODE", "THE LIVING DOLL", "FEATURING TELLY SAVALAS" },
	{ "THE SKULL AND BUG", "ON THE BACKGLASS", "ARE FROM THE EPISODE", "QUEEN OF THE NILE" },
	{ "THE MANNEQUIN MODEL", "ON THE LEFT SLINGSHOT", "IS FROM THE OPENING SEQUENCE", "OF THE TV SERIES" },
	{ "THE FLIGHT JACKET", "ON THE BACKGLASS", "IS FROM THE EPISODE", "KING NINE WILL NOT RETURN" },
	{ "THE SLOT MACHINE", "ON THE PLAYFIELD", "IS FROM THE EPISODE", "THE FEVER" },
	{ "THE PLAYER PIANO", "ON THE PLAYFIELD", "IS FROM THE EPISODE", "A PIANO IN THE HOUSE" },
	{ "THE CAMERA", "ON THE PLAYFIELD", "IS FROM THE EPISODE", "A MOST UNUSUAL CAMERA" },
	{ "WILLIAMS MANUFACTURED", "15,235 UNITS OF TWILIGHT ZONE", "COMPARED TO 20,270 UNITS", "FOR THE ADDAMS FAMILY" },
	{ "DEVELOPMENT ON FREEWPC STARTED", "IN 2005 AND THE FIRST HARDWARE TEST", "WAS IN A WCS", "IN OCTOBER 2008" },
	{ "FREEWPC WAS ORIGINALLY", "100% ASSEMBLY LANGUAGE", "IT IS NOW 90% C", "6% PERL AND 4% ASM"},
	{ "GOTTLIEBS HUMPTY DUMPTY", "MANUFACTURED IN 1947 WAS", "THE FIRST MACHINE TO HAVE", "PLAYER CONTROLLER FLIPPERS" },
	{ "WILLIAMS SARATOGA", "MANUFACTURED IN 1948 WAS", "THE FIRST MACHINE TO HAVE", "THE STANDARD FLIPPER LAYOUT" },
	{ "BALLYS BALLS-A-POPPIN", "MANUFACTURED IN 1956 WAS", "THE FIRST MACHINE TO HAVE", "MULTIBALL" },
	{ "WILLIAMS GORGAR", "MANUFACTURED IN 1979 WAS", "THE FIRST MACHINE TO HAVE", "SYNTHETIC SPEECH" },
	{ "DE CHECKPOINT", "MANUFACTURED IN 1991 WAS", "THE FIRST MACHINE TO HAVE", "A DOT MATRIX DISPLAY" },
	{ "BALLYS NIP-IT, AS PLAYED BY", "THE FONZ IN HAPPY DAYS", "WHICH IS SET IN THE 50S,", "WASNT MADE UNTIL 1972" },
	{ "TOM HANKS ASKED FOR HIS", "IMAGE TO BE REMOVED FROM", "THE BACKGLASS ART OF", "APOLLO 13" },
	{ "DOLPHINS SLEEP", "WITH ONE EYE OPEN", "", "" },
	{ "TWELVE OR MORE COWS", "ARE KNOWN AS", "A FLINK" },
};

static void factoid_msg (U8 factoid)
{
	font_render_string_center (&font_var5, 64, 5, tz_factoids[factoid].line1);
	font_render_string_center (&font_var5, 64, 12, tz_factoids[factoid].line2);
	font_render_string_center (&font_var5, 64, 19, tz_factoids[factoid].line3);
	font_render_string_center (&font_var5, 64, 26, tz_factoids[factoid].line4);
}

void show_random_factoid (void)
{
	dmd_map_overlay ();
	dmd_clean_page_high ();
	dmd_clean_page_low ();
	font_render_string_center (&font_fixed6, 64, 10, "RANDOM FACTOID");
	show_text_on_stars ();
	
	dmd_map_overlay ();
	dmd_clean_page_high ();
	dmd_clean_page_low ();
	factoid_msg(random_scaled(NUM_FACTOIDS - 1));
	show_text_on_stars ();
	dmd_clean_page_high ();
	dmd_clean_page_low ();
	show_text_on_stars ();
}
