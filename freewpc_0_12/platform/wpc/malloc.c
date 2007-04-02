
#include <freewpc.h>

#define TASK_PRECIOUS_SIZE 8

#define MALLOC_BLOCK_SIZE (sizeof (task_t) - TASK_PRECIOUS_SIZE)

#define MALLOC_CHUNK_SIZE 16

#define MALLOC_CHUNKS_PER_BLOCK 2

#define MALLOC_BITMAP_INIT ((1 << MALLOC_CHUNKS_PER_BLOCK) - 1)

/*
 * malloc() is implemented by using the same memory as reserved for
 * the task control blocks.  The state is marked differently so that
 * the scheduler doesn't try to start a task from the block in use.
 *
 * Allocations for more than sizeof(task_t) cannot be honored.
 * A single task block can be used for multiple allocations though.
 * The task block is redefined as follows:
 *
 * Header (2 bytes)
 * Chunk 1 (16 bytes)
 * Chunk 2 (16 bytes)
 * ...
 *
 * The number of chunks is dependent on the task size, which is
 * subject to change and so no assumptions are made.  However no more
 * than 8 chunks can be supported.
 * The header is defined by the struct malloc_header.
 */


struct malloc_chunk
{
	/* Describes how this chunk is being used.
	This byte is only valid for the first chunk of a multi-chunk
	allocation, and identifies the chunk's offset within the
	containing block, as well as the number of contiguous chunks
	that form the allocation. */
	U8 flags;

	/* The user data */
	U8 user[0];
};


struct malloc_block
{
	/* TODO: An overlay of the task structure, for preserving the fields
	that the task scheduler looks at */

	/* An array of bits that identifies which chunks in this block
	are available.  When bit N is set, that means that chunk N is
	FREE.  This field is initialized when the block is allocated to
	say that all chunks are free. */
	U8 chunk_bitmap;

	U8 pad;

	struct malloc_chunk chunk[0];
};


/* Allocate a new block for malloc() memory, stolen from the task
subsystem */
struct malloc_block *malloc_block_alloc (void)
{
	task_t *t;
	struct malloc_block *b;

	t = task_allocate ();
	t->state |= TASK_MALLOC;

	b = (struct malloc_block *)t;
	b->chunk_bitmap = MALLOC_BITMAP_INIT;
	return b;
}


/* Find a free malloc block that has 'count' free chunks.
If an existing block exists and has free space, use that.
Otherwise, allocate a new block. */
struct malloc_chunk *malloc_chunk_alloc (U8 count)
{
	struct malloc_block *b;
	struct malloc_chunk *chunk;

	/* Allocate a new block */
	b = malloc_block_alloc ();

	/* Return a pointer to the chunk */
	chunk = &b->chunk[0];
	chunk->flags = 0;
	return chunk;
}


/* Free a malloc chunk, by returning it to the block that
contains it.  If the containing block has no oustanding
allocations, then free the whole block back to the task ss. */
void malloc_chunk_free (struct malloc_chunk *chunk)
{
}


/* FYI : current task_t size is 80 bytes */

void *malloc (U8 size)
{
	if (size > sizeof (task_t))
		return NULL;
}

void *calloc (U8 size)
{
	void *p = malloc (size);
	if (p)
	{
		memset (p, 0, size);
	}
}

void free (void *p)
{
	task_t *t = (task_t *)p;

	if (t->state == TASK_MALLOC+TASK_USED)
		t->state = TASK_FREE;
}

