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

#include <freewpc.h>
#include <simulation.h>

#define HASH_SIZE 101
#define MAX_CONF_STACK 64

struct conf_item
{
	const char *name;
	unsigned int hash;
	int *valp;
	struct conf_item *chain;
};


struct conf_item *conf_table[HASH_SIZE] = { NULL, };


int conf_stack[MAX_CONF_STACK];
int *conf_stack_ptr = conf_stack;

static unsigned int conf_hash (const char *name)
{
	unsigned int hash = 0;
	while (*name)
	{
		hash = hash * 17 + *name;
		name++;
	}
	return hash;
}

static struct conf_item *conf_find (const char *name)
{
	unsigned int hash = conf_hash (name);
	struct conf_item *cf = conf_table[hash % HASH_SIZE];
	while (cf != NULL)
	{
		if (cf->hash == hash)
			return cf;
		cf = cf->chain;
	}
	return NULL;
}

void conf_add (const char *name, int *valp)
{
	struct conf_item *cf = malloc (sizeof (struct conf_item));
	cf->name = name;
	cf->hash = conf_hash (name);
	cf->valp = valp;
	cf->chain = conf_table[cf->hash % HASH_SIZE];
	conf_table[cf->hash % HASH_SIZE] = cf;
}

int conf_read (const char *name)
{
	struct conf_item *cf = conf_find (name);
	if (cf)
		return *(cf->valp);
	return 0;
}

void conf_write (const char *name, int val)
{
	struct conf_item *cf = conf_find (name);
	if (cf)
		*(cf->valp) = val;
	else
		simlog (SLC_DEBUG, "No such conf item '%s'\n", name);
}


void conf_push (int val)
{
	conf_stack_ptr++;
	*conf_stack_ptr = val;
}


int conf_pop (unsigned int count)
{
	conf_stack_ptr -= count;
}


int conf_read_stack (int offset)
{
	return conf_stack_ptr[-offset];
}

