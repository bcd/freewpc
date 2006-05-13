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

/*
 * This file implements basic queueing functions.
 *
 * All functions are inline.
 *
 * head_off is the index to the next element that can be
 * taken out of the queue.
 *
 * tail_off is the index of the next free position to which
 * a new element can be added.
 *
 * If head_off == tail_off, the queue is empty.
 *
 * Queue full conditions are not checked...
 */

typedef struct
{
	U8 head_off;
	U8 tail_off;
	U8 elems[0];
} queue_t;


extern inline void queue_init (queue_t *q)
{
	q->head_off = q->tail_off = 0;
}


extern inline void queue_insert (queue_t *q, U8 qlen, U8 v)
{
	q->elems[q->tail_off] = v;
	if (++q->tail_off == qlen)
		q->tail_off = 0;
}


extern inline U8 queue_remove (queue_t *q, U8 qlen)
{
	U8 v = q->elems[q->head_off];
	if (qlen == 8)
	{
		q->head_off = (q->head_off + 1) & 7;
	}
	else if (++q->head_off == qlen)
		q->head_off = 0;
	return (v);
}


extern inline bool queue_empty (queue_t *q)
{
	return (q->head_off == q->tail_off);
}


