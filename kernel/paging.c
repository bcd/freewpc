
/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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

__fastram__ uint8_t page_loaded;


#if 0
void _far_call_handler (U8 unused1, U8 unused2)
{
	struct far_call_address
	{
		volatile void (*address) ();
		U8 page;
	};
	register void (*call_address) ();
	struct far_call_address * volatile *call_info;
	U8 saved_page;

	asm __volatile__ ("pshs\tb,x" ::: "b", "x");
	call_info = (struct far_call_address * volatile *)(&unused2 - 2);
	dbprintf ("%p %02X\n", (call_info[-1])->address, (call_info[-1])->page);
	dbprintf ("%p %02X\n", (call_info[0])->address, (call_info[0])->page);
	dbprintf ("%p %02X\n", (call_info[1])->address, (call_info[1])->page);
	dbprintf ("%p %02X\n", (call_info[2])->address, (call_info[2])->page);
	dbprintf ("%p %02X\n", (call_info[3])->address, (call_info[3])->page);

	saved_page = wpc_get_rom_page ();
	call_address = (*call_info)->address;
	asm __volatile__ ("puls\tb,x" ::: "b", "x");
	call_address ();
	wpc_set_rom_page (saved_page);
	(*call_info)++;
}
#endif

uint8_t paged_read_byte (uint8_t *byte_ptr, uint8_t page)
{
	return 0;
}


uint16_t paged_read_word (uint16_t *word_ptr, uint8_t page)
{
	return 0;
}

