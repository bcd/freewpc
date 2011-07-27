/*
 * Copyright 2007-2011 by Brian Dominy <brian@oddchange.com>
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
 * \file
 * \brief Routines for accessing the security PIC device.
 */

/** For low-level debugging ; normally this is not needed */
//#define pic_debug dbprintf
#define pic_debug(rest...)


/** For running this natively on an Intel x86 */
#ifdef CONFIG_LITTLE_ENDIAN
#define POS_CONVERT(pos) (3-(pos))
#else
#define POS_CONVERT(pos) pos
#endif
#define PICMAP(byte, reg, pos) [byte] = (U8 *)&pic_serial_encoded.reg + POS_CONVERT (pos)


/** The internal structure of the PIC is maintained in
4 32-bit registers and 2 8-bit registers. */
struct pic_serial_encoded
{
	U32 reg1;
	U32 reg2;
	U32 reg3;
	U32 reg4;
	U8 check10;
	U8 check2;
} pic_serial_encoded;


/** A mapping between the PIC's internal structure and the
external byte registers that are used to read it.
The internals are "scrambled" to make it more difficult
to reverse engineer.  Each PICMAP entry says (1) which
external serial register is being mapped, (2) which
internal register partially appears here, and (3) which
part of the register it is, with 0 meaning the most significant
byte.  NULL entries mean that the external PIC register is
not directly associated with the internal structure at all. */
U8 *pic_serial_map[] =
{
	PICMAP(3, reg3, 1),
	PICMAP(14, reg3, 2),
	PICMAP(6, reg3, 3),

	PICMAP(1, reg1, 1),
	PICMAP(11, reg1, 2),
	PICMAP(9, reg1, 3),

	PICMAP(7, reg2, 0),
	PICMAP(12, reg2, 1),
	PICMAP(0, reg2, 2),
	PICMAP(8, reg2, 3),

	PICMAP(2, check2, 0),
	PICMAP(10, check10, 0),

	PICMAP(15, reg4, 2),
	PICMAP(4, reg4, 3),

	[5] = NULL,
	[13] = NULL,
};


/** A table that maps the digits of the PIC's
internal 32-bit registers to their location within
the serial number.  Each of the 4 registers can contribute
up to 5 digits.  The Nth row in the table says that this
entry applies to the Nth register (reg1 through reg4).
Within the entry are 5 offsets; if the ith offset is X[i],
then the X[i]th digit of the serial number is described
by the ith digit in the internal register.  A -1
means that this digit does not contribute to the serial
number at all. */
struct pic_strip_info
{
	S8 place[5];
} pic_strip_info[] = {
	{{ -1, -1, 1, 7, 4 }},
	{{ 2, 15, 0, 8, 6 }},
	{{ -1, 16, 3, 5, 14 }},
	{{ 13, 12, 11, 10, 9 }},
};


/** A lookup table to aid in division by powers of ten. */
static const U16 place_value[] = {
	10000U,
	1000U,
	100U,
	10U,
	1U
};

/** The decoded PIC serial number.  The 17-digit serial number
 * is stored here in textual format, i.e. each byte is between
 * '0' and '9'.  The last byte is kept as a '\0' to allow
 * the whole thing to be printed. */
U8 pic_serial_number[18];

/** True if the PIC could not be read or some error occurred during
 * reading, so that the PIC data should not be trusted. */
bool pic_invalid;

/** True if the PIC unlock code has been calculated.  Until this is
 * true, the periodic refresh is disabled. */
bool pic_unlock_ready;

/** The unlock code that is periodically sent to the PIC to enable
 * switch scanning */
U8 pic_unlock_code[3];


/** Decode a 32-bit internal PIC register. */
void pic_decode32 (U32 *reg, const U32 offset, const U16 divisor, const bool negate)
{
	pic_debug ("reg.start = %w\n", *reg);
	pic_debug ("offset = %w\n", offset);
	pic_debug ("divisor = %ld\n", divisor);
	if (negate)
	{
		*reg = offset -*reg;
		pic_debug ("reg.after_negate = %w\n", *reg);
	}
	else
	{
		*reg -= offset;
		pic_debug ("reg.after_subtract = %w\n", *reg);
	}

	/* Division/modulo operations under the 6809 compiler are weakly
	 * implemented, so bypass them and use a special hand-crafted version
	 * that really works. */
#ifdef CONFIG_NATIVE
	if (*reg % divisor)
		dbprintf ("error: there is a remainder\n");
	*reg /= divisor;
#else
	/* This is done differently only because native 32-bit division on
	the 6809 does not work. */
	struct divrem32 divrem;
	U32 divisor32 = divisor;
	divide32 (reg, &divisor32, &divrem);
	*reg = divrem.quotient;
#endif
	pic_debug ("reg.after_divide = %w\n", *reg);
}


/** Strip the individual digits of a decoded PIC register,
 * and store them separately into the decoded serial number
 * array according to the mapping defined in INFO. */
void pic_strip_digits (U32 reg, struct pic_strip_info *info)
{
	U8 place;
	U8 digit;

	for (place = 0 ; place < 5 ; place++)
	{
		digit = 0;
		pic_debug ("pic_strip_digits %w\n", reg);
		while (reg >= place_value[place])
		{
			reg -= place_value[place];
			digit++;
		}
		pinio_watchdog_reset ();

		if (info->place[place] != -1)
		{
			pic_debug ("Digit %d is %d (place was %ld)\n", info->place[place], digit, place_value[place]);
			pic_serial_number[info->place[place]] = digit + '0';
		}
	}
}


/** Return the game number as stored in the PIC as a decimal value. */
U16 pic_read_game_number (void)
{
	U16 game_number;

	game_number = 100UL * (pic_serial_number[0] - '0')
		+ 10UL * (pic_serial_number[1] - '0')
		+ (pic_serial_number[2] - '0');

	return game_number;
}


/** Compute the 3 byte switch matrix unlock code.
 * This is a function of the last 3 bytes of the serial number,
 * plus the game number (the first 3 bytes). */
void pic_compute_unlock_code (void)
{
	U32 reg;

	dbprintf ("Computing unlock code...");

	reg = pic_read_game_number ();

	reg = ((reg >> 8) * (0x100UL * pic_serial_number[14] + pic_serial_number[16])) +
		((reg & 0xFF) * (0x100UL * pic_serial_number[15] + pic_serial_number[14]));

	pic_unlock_code[0] = (reg >> 16) & 0xFF;
	pic_unlock_code[1] = (reg >> 8) & 0xFF;
	pic_unlock_code[2] = reg & 0xFF;
	pic_unlock_ready = TRUE;

	dbprintf ("done\n");
}



__attribute__((noinline))
void pic_wait (void)
{
	pinio_watchdog_reset ();
	null_function ();
	null_function ();
	null_function ();
}


/**
 * Initialize the PIC security device.
 */
void pic_init (void)
{
	U8 i;
	U8 val;
	U8 *ereg;

	dbprintf ("Initialize the PIC...\n");
	pic_invalid = TRUE;
	pic_unlock_ready = FALSE;

#ifdef CONFIG_NO_PIC
	return;
#endif

	/* Reset the PIC. */
	wpc_write_pic (WPC_PIC_RESET);
	pic_wait ();

	/* Zero the serial number registers */
	memset (&pic_serial_number, '0', sizeof (pic_serial_number));

	/* Zero the encoding registers.  Because most of these are
	 * 32-bit numbers, but sometimes only 16 or 24 bits will be
	 * set below, we ensure the uppermost bytes are zero here. */
	memset (&pic_serial_encoded, 0, sizeof (pic_serial_encoded));

	/* Read each of the PIC serial registers, and store the
	 * value returned into the appropriate encoding register,
	 * using the serial map to guide us. */
	for (i=0; i < 16; i++)
	{
		/* Read a single PIC register, and store it in the
		 * correct slot.  Note that some delay is required
		 * before the result can be obtained.  Also interrupts
		 * must be disabled to prevent other accesses to the
		 * PIC device during this time (i.e. switch polling). */
		wpc_write_pic (WPC_PIC_SERIAL (i));
		pic_wait ();
		val = wpc_read_pic ();
		ereg = pic_serial_map[i];
		if (ereg)
			*ereg = val;
	}

	/* Each encoding register holds one or more PIC digits,
	 * multiplied by some magic value, offset by some magic value...
	 * The next step is to strip out the magic values, leaving only
	 * the digits.
	 *
	 * If any division yields a remainder, the data is invalid.
	 *
	 * The check registers are not modified in any way, but serve
	 * as an adjustment on reg2 and reg3.
	 */

	/* reg1 = digits x 1BCD + 1F3F0
	 * reg2 = digits x 107F + 71E259
	 * reg3 = digits x 245 + 3D74
	 * reg4 = 99999 - digits
	 */
	pic_decode32 (&pic_serial_encoded.reg1, 0x1F3F0ULL, 0x1BCD, FALSE);
	pic_serial_encoded.reg1 -= (5 * pic_serial_encoded.check10);
	pic_strip_digits (pic_serial_encoded.reg1, &pic_strip_info[0]);
	
	pic_decode32 (&pic_serial_encoded.reg2, 0x71E259ULL, 0x107F, FALSE);
	pic_serial_encoded.reg2 -= (2 * pic_serial_encoded.check10 + pic_serial_encoded.check2);
	pic_strip_digits (pic_serial_encoded.reg2, &pic_strip_info[1]);

	pic_decode32 (&pic_serial_encoded.reg3, 0x3D74ULL, 0x245, FALSE);
	pic_serial_encoded.reg3 -= pic_serial_encoded.check2;
	pic_strip_digits (pic_serial_encoded.reg3, &pic_strip_info[2]);
	
	pic_decode32 (&pic_serial_encoded.reg4, 99999ULL, 1, TRUE);
	pic_strip_digits (pic_serial_encoded.reg4, &pic_strip_info[3]);
}


CALLSET_ENTRY (pic, init_complete)
{
	/* Once the serial number is fully decoded, verify that
	 * it matches the game number.  If it did not match, then
	 * the switch matrix will not work. */
	U16 pic_game_number = pic_read_game_number ();
	if (pic_game_number == MACHINE_NUMBER)
	{
		pic_invalid = FALSE;
		pic_compute_unlock_code ();
	}
}


/** Render the serial number read from the PIC for display.
 * Note that the last 3 digits of the serial number are
 * never displayed normally, as this breaks the security
 * feature. */
void pic_render_serial_number (void)
{
	sprintf ("%3s %6s %5s", pic_serial_number, pic_serial_number+3,
		pic_serial_number+9);
}

