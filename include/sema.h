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

#ifndef _SEMA_H
#define _SEMA_H

typedef U8 sema_t;


extern inline void sem_init (sema_t *sem, U8 count)
{
	*sem = count;
}

extern inline bool sem_try_wait (sema_t *sem)
{
	if (*sem != 0)
	{
		(*sem)--;
		return TRUE;
	}
	else
		return FALSE;
}

extern inline bool sem_wait (sema_t *sem)
{
	while (sem_try_wait (sem) == FALSE)
		task_sleep (TIME_100MS);
}

extern inline bool sem_wait_any (sema_t *sems[])
{
	for (;;)
	{
		sema_t **sem = sems[0];
		while (*sem != NULL)
		{
			if (sem_try_wait (*sem) == TRUE)
				return;
			sem++;
		}
		task_sleep (TIME_100MS);
	}
}

extern inline void sem_signal (sema_t *sem)
{
	(*sem)++;
}

extern inline void sem_set (sema_t *sem)
{
	(*sem) = 1;
}


#endif /* _SEMA_H */
