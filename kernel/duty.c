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


typedef struct duty_pulse
{
	U8 on_time;
	U8 off_time;
} duty_pulse_t;


typedef U8 duty_state_t;


typedef struct duty_entry
{
	const duty_pulse_t *pulse;
	const duty_ops_t *ops;
	U8 id;
	duty_state_t state;
	U8 cycle_limit;
} duty_entry_t;


typedef struct duty_ops
{
	void (*on) (U8 id);
	void (*off) (U8 id);
	void (*limit) (U8 id);
} duty_ops_t;


void duty_init (
	duty_entry_t *entry, 
	duty_ops_t *ops, 
	U8 id, 
	const duty_pulse_t *pulse )
{
	entry->pulse = pulse;
	entry->ops = ops;
	entry->id = id;
	entry->state = pulse->on_time;
	entry->cycle_limit = cycle_limit;
}

void duty_update (duty_entry_t *entry)
{
	if (entry->state < 0)
	{
		/* Output is currently off */
		entry->state++;
		if (entry->state == 0)
		{
			/* A full cycle has completed */
			if (entry->cycle_limit > 0)
			{
				entry->cycle_limit--;
				if (entry->cycle_limit == 0)
				{
					/* Keep off permanently */
					return;
				}
			}
			/* Output should be switched on */
			(*entry->ops->on) (entry->id);
			/* Reset state to limit on time */
			entry->state = pulse->on_time;
		}
	}
	else
	{
		/* Output is currently on */
		entry->state--;
		if (entry->state == 0)
		{
			/* Output should be switched off */
			(*entry->ops->off) (entry->id);
			/* Reset state to limit off time */
			entry->state = -pulse->off_time;
		}
	}
}


