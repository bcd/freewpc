/*
 * Copyright 2008 by Brian Dominy <brian@oddchange.com>
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

#ifndef _PRESET_H
#define _PRESET_H


struct preset_component
{
	struct adjustment *info;
	U8 *nvram;
	U8 value;
};


struct preset
{
	char *name;
	struct preset_component *comps;
};


__test2__ U8 preset_count (void);
__test2__ void preset_install (struct preset_component *comps);
__test2__ void preset_install_from_test (void);
__test2__ void preset_install_country_code (U8 code);
__test2__ void preset_render_name (U8 index);
__test2__ bool preset_installed_p (U8 index);
__test2__ void preset_show_components (void);


#endif /* _PRESET_H */
