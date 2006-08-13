
#include <freewpc.h>

struct malloc_header
{
	struct malloc_header *prev;
	struct malloc_header *next;
	U8 size;
	U8 pad[3];
	U8 data[0];
};


static struct malloc_header *free_list;


/* Note: you can only allocate up to 256 bytes at a time! */
void *
malloc (U8 len)
{
	/* Round the length to the next multiple of 16. */
	len = (len + 15) & ~15;

	/* Search for a block of at least the required size. */
	void *p = free_list;

	while (p != NULL)
	{
		if (p->size >= len)
		{
			/* Found a suitable block. */

			/* Is it too big?  It may need fragmenting. */
			res = p->size - len;
			if (res > sizeof (struct malloc_header) + 16)
			{
				/* Yes, split it into two blocks */
			}

			/* Remove p from the free list */
			return (p);
		}
		else
			p = p->next;
	}

	/* No blocks found. */
	/* Try to coalesce two adjacent blocks into one */
	return (NULL);
}


void
malloc_init (void)
{
	/* Initialize the free list with a single block
	 * encompassing the entire malloc area. */
	free_list = (struct malloc_header *)MALLOC_BASE;
	free_list->prev = free_list->next = 0;
	free_list->size = MALLOC_SIZE - sizeof (struct malloc_header);
}


