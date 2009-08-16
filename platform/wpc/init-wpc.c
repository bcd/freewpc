/*
 * Copyright 2009 by Brian Dominy <brian@oddchange.com>
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


/**
 * Initialize the WPC platform.
 */
void platform_init (void)
{
	/* Reset the sound board... the earlier the better */
	pinio_reset_sound ();

	/* Initializing the RAM page */
	pinio_set_bank (PINIO_BANK_RAM, 0);

#ifdef __m6809__
	/* Install the null pointer catcher, by programming
	 * some SWI instructions at zero. */
	*(U8 *)0 = 0x3F;
	*(U8 *)1 = 0x3F;
#endif /* __m6809__ */

	/* Set up protected RAM */
	wpc_set_ram_protect (RAM_UNLOCKED);
	wpc_set_ram_protect_size (PROT_BASE_0x1800);
	wpc_set_ram_protect (RAM_LOCKED);

	/* Initialize the ROM page register
	 * page of ROM adjacent to the system area is mapped.
	 * This is the default location for machine-specific files. */
	pinio_set_bank (PINIO_BANK_ROM, MACHINE_PAGE);

	/* Initialize other critical WPC output registers relating
	 * to hardware */
	writeb (WPC_SOL_FLASH2_OUTPUT, 0);
	writeb (WPC_SOL_HIGHPOWER_OUTPUT, 0);
	writeb (WPC_SOL_FLASH1_OUTPUT, 0);
	writeb (WPC_SOL_LOWPOWER_OUTPUT, 0);
	writeb (WPC_LAMP_ROW_OUTPUT, 0);
	writeb (WPC_GI_TRIAC, 0);
}

