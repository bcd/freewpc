
#include "imglib.h"

#define MAX_FRAMES 3
unsigned int frame_ring_offset = 0;
struct buffer *frame_ring[MAX_FRAMES] = { NULL, };

struct buffer *sum_buffer;

struct buffer *prev_sum_buffer = NULL;


/*
 * Return the next frame of display data.
 */
struct buffer *get_frame (void)
{
	FILE *fp;
	struct buffer *buf;

	fp = fopen ("../../../eon/dmd", "r");
	if (!fp)
	{
		fprintf (stderr, "could not open image\n");
		return NULL;
	}

	buf = frame_alloc ();
	buffer_read (buf, fp);
	fclose (fp);

	buf = buffer_replace (buf, buffer_splitbits (buf));
	buf->width = 128;
	buf->height = 32;
	return buf;
}


/**
 * Given a new bitmap BUF, update the display.
 */
void update_view (struct buffer *buf)
{
	unsigned int pos;

	sum_buffer = frame_alloc ();

	/* Subtract oldest buffer contents */
	if (frame_ring[frame_ring_offset])
	{
		for (pos=0; pos < sum_buffer->len; pos++)
			sum_buffer->data[pos] -= frame_ring[frame_ring_offset]->data[pos];
		buffer_free (frame_ring[frame_ring_offset]);
	}

	/* Add new contents */
	for (pos=0; pos < sum_buffer->len; pos++)
		sum_buffer->data[pos] += buf->data[pos];
	frame_ring[frame_ring_offset] = buf;

	/* Show the new buffer if it's different */
	if (!prev_sum_buffer
		|| memcmp (prev_sum_buffer->data, sum_buffer->data, 128*32))
		bitmap_write_ascii (sum_buffer, stdout);

	if (prev_sum_buffer)
		buffer_free (prev_sum_buffer);
	prev_sum_buffer = sum_buffer;
}


int main (int argc, char *argv[])
{
	struct buffer *buf;

	for (;;)
	{
		usleep (10 * 1000UL);
		buf = get_frame ();
		if (buf)
			update_view (buf);
	}
}

