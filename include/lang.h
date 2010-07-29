/*
 * Copyright 2010 by Brian Dominy <brian@oddchange.com>
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

#ifndef _LANG_H
#define _LANG_H

/* Enumerate all possible supported languages.
	Each is given a unique integer ID.  In any particular build, a subset
	(nonzero) of these languages may be compiled in.
 */
#define STR_EN_OFF 0
#define STR_FR_OFF 1
#define STR_GR_OFF 2


/* Configuration (TODO : move to .config)
	Each define CONFIG_LANG_xxx can be enabled or disabled, to say
	if that language is included at compile-time.

	CONFIG_LANG_DEFAULT should also be defined to the default
	language (one of the STR_xx_OFF defines).
 */
#define CONFIG_LANG_EN
#define CONFIG_LANG_FR
#define CONFIG_LANG_GR
#define CONFIG_LANG_DEFAULT STR_EN_OFF


/* Compute how many languages have been enabled at compile time.
   For each translatable string, we will store it as an array of
	this many real strings. */
#define NUM_STR_LANGS 3


/* Define a type 'string_array' which is an array of strings, one
	for each possible translation. */
typedef const char *string_array[NUM_STR_LANGS];


/* Define some helper macros for declaring string translations.
	The general syntax is:
		STR_BEGIN(XYZ)
			STR_EN("xyz")
			STR_FR("xyz_fr")
			STR_GR("xyz_gr")
		STR_END

	STR_BEGIN and STR_END bracket a list of translations.
	STR_BEGIN also gives the list a label for a name.

	Within the list, use STR_xx to give the translation
	for each of the languages.
	All other translations are optional except for the default.
	If a translation is not given, the default language will be used.
 */
#define STR_BEGIN(label) string_array str_ ## label = {
#define STR_END };

#ifdef CONFIG_LANG_EN
#define STR_EN(str) [STR_EN_OFF] = str,
#else
#define STR_EN(str) STR_EN(str)
#endif

#ifdef CONFIG_LANG_FR
#define STR_FR(str) [STR_FR_OFF] = str,
#else
#define STR_FR(str) STR_EN(str)
#endif

#ifdef CONFIG_LANG_GR
#define STR_GR(str) [STR_GR_OFF] = str,
#else
#define STR_GR(str) STR_EN(str)
#endif


/* Define a macro STR_OFF which refers to the current language to
   be used for string references. */
#if (NUM_STR_LANGS > 1)
	/* If more than one language is enabled at compile-time,
	then we must use a runtime variable to track which one
	is currently enabled. */
	extern U8 str_off;
	#define STR_OFF str_off
#else
	/* If only one language is enabled, then runtime checks
	can be skipped. */
	#if defined CONFIG_LANG_EN
		#define STR_OFF STR_EN_OFF
	#elif defined CONFIG_LANG_FR
		#define STR_OFF STR_FR_OFF
	#elif defined CONFIG_LANG_GR
		#define STR_OFF STR_GR_OFF
	#endif
#endif


/* STR (label) is used whenever you need a reference to
   a particular string.  The correct translation is returned.
	Use the same label name that appeared in the translation
	table's STR_BEGIN() call. */
#define STR(label) str_ ## label [STR_OFF]

#endif /* _LANG_H */
