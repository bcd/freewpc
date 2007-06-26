/*
 * Copyright 2007 by Brian Dominy <brian@oddchange.com>
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

/* Design:
 * We use the block allocator provided by task.c to provide us with chunks
 * of memory of size 'sizeof (task_t)', which is currently 80 bytes.
 * 1 byte of this is reserved for task 'state', here it means that the
 * dispatcher should skip those blocks that are being used for dynamic memory.
 *
 * The raw allocator then reserves the first 4 bytes of any such chunk
 * for its own housekeeping.  See malloc_chunk_header_t.  The chunk header
 * contains next/prev pointers for maintaining a linked list of chunks.
 *
 * The remaining bytes of the chunk are subdivided into smaller blocks
 * that are actually handed out to callers of malloc().  These are called
 * userblocks.  The number of userblocks per chunk depends on the size.
 * Chunks are dedicated to a certain size userblock once created; this
 * is implied by which freelist they are on:
 * - 2 userblock/chunk @ 32 bytes each
 * - 4 userblocks/chunk @ 16 bytes each
 * - 8 userblocks/chunk @ 8 bytes each
 *
 * Each userblock has a 1-byte header that is used during free to
 * figure out which chunk the block is part of.
 */

#define MAX_USERBLOCK 32

//#define MALLOC_TEST

enum chunk_type
{
	CHUNK_TYPE_LEN8 = 0,
	CHUNK_TYPE_LEN16,
	CHUNK_TYPE_LEN32,
	NUM_CHUNK_TYPES
};


typedef struct _user_header
{
	/* Not used.  Keep the reserved bits at the top, as GCC
	puts earlier fields into the high order bits, which is the
	least efficient place for them. */
	U8 reserved : 3;

	/* type says which size chunk this is.  This is written
	during chunk allocation and remains constant thereafter. */
	U8 type : 2;

	/* blocknum says which block within the chunk this is.
	This is written during chunk allocation and remains
	constant thereafter. */
	U8 blocknum : 3;
} user_header_t;


typedef struct _malloc_chunk
{
	struct _malloc_chunk *next;
	struct _malloc_chunk *prev;

	/* A bitmask that says which userblocks in the chunk
	are available */
	U8 available;
	union {
		struct {
			struct userblock8 {
				user_header_t flags;
				U8 data[8];
			} blocks[8];
		} len8;
		struct {
			struct userblock16 {
				user_header_t flags;
				U8 data[16];
			} blocks[4];
		} len16;
		struct {
			struct userblock32 {
				user_header_t flags;
				U8 data[32];
			} blocks[2];
		} len32;
	} u;
} malloc_chunk_t;


/** An array of free lists of chunks, indexed by chunk type */
malloc_chunk_t *chunk_lists[NUM_CHUNK_TYPES];


static const U8 set_bit_mask[8] = { 
	0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 
};

static const U8 clear_bit_mask[8] = { 
	0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F
};

/* first_one_table[N] = bit position of the first '1' digit
 * in N, where N is 0..15.  Values are 0..3. */
static const U8 first_one_table[16] = {
	/* 0000b */ 0xFF, /* invalid - no ones are set */
	/* 0001b */ 0,
	/* 0010b */ 1,
	/* 0011b */ 0,
	/* 0100b */ 2,
	/* 0101b */ 0,
	/* 0110b */ 1,
	/* 0111b */ 0,
	/* 1000b */ 3,
	/* 1001b */ 0,
	/* 1010b */ 1,
	/* 1011b */ 0,
	/* 1100b */ 2,
	/* 1101b */ 0,
	/* 1110b */ 1,
	/* 1111b */ 0,
};


static inline enum chunk_type get_chunk_type_for_size (U8 size)
{
	/* Note, we favor smaller allocations by checking
	the sizes in increasing order. */
	if (size <= 8)
		return CHUNK_TYPE_LEN8;
	else if (size <= 16)
		return CHUNK_TYPE_LEN16;
	else if (size <= 32)
		return CHUNK_TYPE_LEN32;
	else
	{
		dbprintf ("attempt to malloc too much\n");
		fatal (ERR_MALLOC);
	}
}


static inline malloc_chunk_t **get_chunks_for_type (enum chunk_type type)
{
	return &chunk_lists[type];
}


/** Given a bitmask in 'bits', find the first bit position that is
nonzero.  It is assumed that 'bits' is nonzero. */
U8 find_first_one (U8 bits)
{
	if (bits & 0x0F)
	{
		return first_one_table[bits & 0x0F];
	}
	else
	{
		bits >>= 4;
		return first_one_table[bits] + 4;
	}
}


#if 0
/** Dump a userblock's status */
void userblock_dump (enum chunk_type type, U8 blocknum, user_header_t *flags)
{
	if (blocknum != flags->blocknum)
	{
		dbprintf ("?B%d/%d", blocknum, flags->blocknum);
	}
	else if (type != flags->type)
	{
		dbprintf ("?T%d/%d", type, flags->type);
	}
	else if (flags->allocated == 0)
	{
		dbprintf (".");
	}
	else
	{
		dbprintf ("X");
	}
}
#endif


/* Dump the structure of a task block that is used for malloc(). */
void malloc_chunk_dump (task_t *task)
{
	malloc_chunk_t *chunk = (malloc_chunk_t *)(((char *)task) + 1);
	enum chunk_type type = chunk->u.len8.blocks[0].flags.type;
	U8 block, blocks;

	dbprintf ("nx=%p  pv=%p  ", chunk->next, chunk->prev);

	if (chunk == chunk_lists[type])
		dbprintf ("HEAD   ");
	else
		dbprintf ("       ");

	switch (type)
	{
		case CHUNK_TYPE_LEN8:
			dbprintf ("MEM(8)  ");
			blocks = 8;
			break;
		case CHUNK_TYPE_LEN16:
			dbprintf ("MEM(16) ");
			blocks = 4;
			break;
		case CHUNK_TYPE_LEN32:
			dbprintf ("MEM(32) ");
			blocks = 2;
			break;
	}

	for (block = 0; block < blocks; block++)
	{
		if (chunk->available & (1 << blocks))
		{
			dbprintf (".");
		}
		else
		{
			dbprintf ("X");
		}
	}

	dbprintf ("  %02X\n", chunk->available);
}


/** Allocate and initialize a new chunk of memory.  This is used
internally. */
malloc_chunk_t *chunk_allocate (enum chunk_type type)
{
	extern task_t *block_allocate (void);
	task_t *task = block_allocate ();
	malloc_chunk_t *chunk;
	U8 block;

	dbprintf ("allocating chunk for type %d\n", type);

	if (!task)
	{
		dbprintf ("block_allocate failed\n");
		fatal (ERR_MALLOC);
	}

	task->state |= TASK_MALLOC;
	chunk = (malloc_chunk_t *)(((char *)task) + 1);

	/* Initialize each of the user blocks in the chunk */
	switch (type)
	{
		case CHUNK_TYPE_LEN8:
			chunk->available = 0xFF;
			for (block = 0; block < 8; block++)
			{
				struct userblock8 *ub = &chunk->u.len8.blocks[block];
				ub->flags.reserved = 0;
				ub->flags.type = CHUNK_TYPE_LEN8;
				ub->flags.blocknum = block;
			}
			break;
		case CHUNK_TYPE_LEN16:
			chunk->available = 0x0F;
			for (block = 0; block < 4; block++)
			{
				struct userblock16 *ub = &chunk->u.len16.blocks[block];
				ub->flags.reserved = 0;
				ub->flags.type = CHUNK_TYPE_LEN16;
				ub->flags.blocknum = block;
			}
			break;
		case CHUNK_TYPE_LEN32:
			chunk->available = 0x03;
			for (block = 0; block < 2; block++)
			{
				struct userblock32 *ub = &chunk->u.len32.blocks[block];
				ub->flags.reserved = 0;
				ub->flags.type = CHUNK_TYPE_LEN32;
				ub->flags.blocknum = block;
			}
			break;

		default:
			dbprintf ("invalid type %d in chunk alloc\n", type);
			fatal (ERR_MALLOC);
	}

	return chunk;
}


/** Allocate a block of dynamic memory. */
void *malloc (U8 size)
{
	malloc_chunk_t *chunk, *first, **chunkptr;
	U8 blocknum;
	enum chunk_type type;
	void *ptr;

	/* Find the first free chunk that can satisfy a request of
	this size. */
	type = get_chunk_type_for_size (size);
	chunkptr = get_chunks_for_type (type);

	if (chunkptr)
	{
		chunk = *chunkptr;
	}
	else
	{
		/* No chunks of this size are free now.  We need to create
		a new chunk.  Make it the head of its free list. */
		chunk = *chunkptr = chunk_allocate (type);
		chunk->next = chunk->prev = chunk;
	}

find_free:
	/* OK, we have at least 1 chunk on the free list, and 'chunk'
	points to its head.  Find the first free userblock within the
	chunk.  If the entire block is used up, then move to the next
	block on the same list.  Note that the lists are doubly-linked,
	and cyclic, so end-of-list is detected as the pointer wrapping
	back to the front. */
	first = chunk;
	do {
		if (chunk->available)
		{
			/* There's at least 1 free block here... take the first one */
			blocknum = find_first_one (chunk->available);
			chunk->available &= clear_bit_mask[blocknum];

			switch (type)
			{
				case CHUNK_TYPE_LEN8:
				{
					struct userblock8 *ub = &chunk->u.len8.blocks[blocknum];
					ptr = ub->data;
					break;
				}
				case CHUNK_TYPE_LEN16:
				{
					struct userblock16 *ub = &chunk->u.len16.blocks[blocknum];
					ptr = ub->data;
					break;
				}
				case CHUNK_TYPE_LEN32:
				{
					struct userblock32 *ub = &chunk->u.len32.blocks[blocknum];
					ptr = ub->data;
					break;
				}
				default:
					dbprintf ("invalid type %d in find_free\n", type);
					fatal (ERR_MALLOC);
			}
			return ptr;
		}
		else
		{
			chunk = chunk->next;
		}
	} while (chunk != first);

	/* There are no free blocks on any of the already allocated
	chunks.  Need to allocate a new chunk then, and add it to
	the head of the list. */
	chunk = *chunkptr = chunk_allocate (type);
	chunk->next = first;
	chunk->prev = first->prev;
	first->prev->next = chunk;
	first->prev = chunk;
	goto find_free;
}


/** Free a block of dynamically allocated memory. */
void free (void *ptr)
{
	enum chunk_type type;
	user_header_t *flags;
	U8 blocknum;
	malloc_chunk_t *chunk;

	/* Get the chunk type and block number */
	flags = (user_header_t *)(ptr - 1);
	type = flags->type;
	blocknum = flags->blocknum;

	/* Get a pointer to the first userblock for this chunk */
	switch (type)
	{
		case CHUNK_TYPE_LEN8:
			ptr = flags - sizeof (struct userblock8) * blocknum;
			break;

		case CHUNK_TYPE_LEN16:
			ptr = flags - sizeof (struct userblock16) * blocknum;
			break;

		case CHUNK_TYPE_LEN32:
			ptr = flags - sizeof (struct userblock32) * blocknum;
			break;
	}

	/* Back up to the beginning of the chunk */
	chunk = (malloc_chunk_t *)(ptr - 5);

	/* Mark the block as available again */
	chunk->available |= set_bit_mask[blocknum];

	/* TODO : if all userblocks in this chunk are free, then
	return the entire chunk back to the block allocator. */
}


/** An indication that a minimum of 'count' buffers, each of
length 'size', is required by the caller.  This is used
as an early indicator of memory requirements in order to
speed up the allocations. */
void prealloc (U8 size, U8 count)
{
}


#ifdef MALLOC_TEST

#define MAX_POINTERS 32

U8 *ptrs[MAX_POINTERS];

void malloc_test_thread (void)
{
	U8 *p;
	U8 n, sz;

	task_set_flags (TASK_PROTECTED);
	dbprintf ("malloc() test running.\n");

	for (n = 0; n < MAX_POINTERS; n++)
		ptrs[n] = NULL;

	for (;;)
	{
		/* Try to allocate something */
		n = random_scaled (MAX_POINTERS);
		if (ptrs[n] == NULL)
		{
			sz = random_scaled (MAX_USERBLOCK);
			p = ptrs[n] = malloc (sz);
			memset (p, sz, sz);	
		}

		/* Try to free something */
		n = random_scaled (MAX_POINTERS);
		if (ptrs[n] != NULL)
		{
			U8 off;
			p = ptrs[n];
			sz = p[0];
			for (off = 1; off < sz; off++)
			{
				if (p[off] != sz)
				{
					dbprintf ("bad data in %p, len %d, offset %d\n", p, sz, off);
					fatal (ERR_MALLOC);
				}
			}
			free (ptrs[n]);
			ptrs[n] = NULL;
		}

		/* Wait a bit before the next iteration. */
		task_sleep (TIME_16MS);
	}
}

#endif /* MALLOC_TEST */


/** Initialize the malloc subsystem */
CALLSET_ENTRY (malloc, init)
{
	enum chunk_type type;

	/* Make sure malloc_chunk fits into a task_t */
	if (sizeof (malloc_chunk_t) > sizeof (task_t) - 1)
	{
		dbprintf ("chunk is too large\n");
		fatal (ERR_MALLOC);
	}

#ifdef MALLOC_TEST
	task_create_anon (malloc_test_thread);
#endif
} 

