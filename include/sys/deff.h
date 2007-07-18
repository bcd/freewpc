/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
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

#ifndef _SYS_DEFF_H
#define _SYS_DEFF_H


/** Type for a display effect ID */
typedef U8 deffnum_t;

/** Type for a display effect function */
typedef void (*deff_function_t) (void);


/** The flags for a normal display effect with no special requirements */
#define D_NORMAL	0x0

/** Set for a deff that is long-lived and runs until explicitly stopped */
#define D_RUNNING 0x1

/** Set for a deff that will only run for a short time once it gets the
display, but would like to wait if the display is currently busy.
After a certain period of time, waiting deffs are cancelled. */
#define D_WAITING 0x2

/** Set for a deff that can run with only 16 rows -- half of the display.
The deff function should call deff_get_current_size() to find out which
rows are available. */
#define D_HALFSIZE 0x4


/** Type for a display effect definition */
typedef struct
{
	U8 flags;
	U8 prio;
	deff_function_t fn;
	U8 page;
} deff_t;


extern const deff_t deff_table[];

extern void (*deff_component_table[4]) (void);

bool deff_is_running (deffnum_t dn);
deffnum_t deff_get_active (void);
void deff_start (deffnum_t dn);
void deff_stop (deffnum_t dn);
void deff_restart (deffnum_t dn);
void deff_start_highest_priority (void);
__noreturn__ void deff_exit (void);
__noreturn__ void deff_delay_and_exit (task_ticks_t ticks);
void deff_swap_low_high (S8 count, task_ticks_t delay);
void deff_init (void);
void deff_stop_all (void);


extern inline void deff_init_components (void)
{
	deff_component_table[0] = null_function;
	deff_component_table[1] = null_function;
	deff_component_table[2] = null_function;
	deff_component_table[3] = null_function;
}


extern inline void deff_set_component (U8 cnum, void (*function) (void))
{
	deff_component_table[cnum] = function;
}


extern inline void deff_call_components (void)
{
	(*deff_component_table[0]) ();
	(*deff_component_table[1]) ();
	(*deff_component_table[2]) ();
	(*deff_component_table[3]) ();
}


/* TODO - this function should be deprecated */
extern inline void deff_wait_for_other (deffnum_t dn)
{
	if (deff_is_running (dn))
	{
		U8 timeout = TIME_3S / TIME_100MS;
		while (timeout && deff_is_running (dn))
		{
			timeout--;
			task_sleep (TIME_100MS);
		}
	}
}


#ifndef MACHINE_CUSTOM_AMODE
void default_amode_deff (void);
#endif

#ifndef DEFF_AMODE
#define DEFF_AMODE 1
#endif

#endif /* _SYS_DEFF_H */

