
#include "imglib.h"
#include "wpclib.h"

//#define FILE_METHOD
#define SOCKET_METHOD

#define MAX_FRAMES 3
unsigned int frame_ring_offset = 0;
struct buffer *frame_ring[MAX_FRAMES] = { NULL, };

struct buffer *sum_buffer;

struct buffer *prev_sum_buffer = NULL;

int cpu_sock;

#if 0
/*
 * Return the next frame of display data.
 */
struct buffer *get_frame (void)
{
	FILE *fp;
	struct buffer *buf;

#ifdef FILE_METHOD
	fp = fopen ("../../../eon/dmd", "r");
	if (!fp)
	{
		fprintf (stderr, "could not open image\n");
		return NULL;
	}

	buf = frame_alloc ();
	buffer_read (buf, fp);
	fclose (fp);
#endif

#ifdef SOCKET_METHOD
	buf = frame_alloc ();
	//try_receive ();
	//memcpy (buf->data, s, 512);
	buf->len = 512;
#endif

	buf = buffer_replace (buf, buffer_splitbits (buf));
	buf->width = 128;
	buf->height = 32;
	return buf;
}
#endif


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
		//buffer_free (frame_ring[frame_ring_offset]);
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

struct buffer *page_buffer[16] = { NULL, };

struct buffer *last_buf = NULL;


void process_dmd_page (struct wpc_message *msg)
{
	struct buffer *buf;
	int n;

	for (n = 0; n < NUM_DMD_PHASES; n++)
	{
		unsigned int page = msg->u.dmdpage.phases[n].page;
		unsigned char *data = msg->u.dmdpage.phases[n].data;

		//printf ("Page %d data.\n", page);

		if (page_buffer[page])
			buffer_free (page_buffer[page]);

		buf = frame_alloc ();
		memcpy (buf->data, data, 512);
		buf->len = 512;

		buf = buffer_replace (buf, buffer_splitbits (buf));
		buf->width = 128;
		buf->height = 32;

		page_buffer[page] = buf;
	}
}


void process_visible_pages (struct wpc_message *msg)
{
	int phase;
	struct buffer *buf, *composite;
	int pos;

	composite = frame_alloc ();
	for (phase = 0; phase < NUM_DMD_PHASES; phase++)
	{
		unsigned int page = msg->u.dmdvisible.phases[phase];
		printf ("Phase %d = %d\n", phase, page);

		buf = page_buffer[page];
		for (pos=0; pos < composite->len; pos++)
			composite->data[pos] += (buf ? buf->data[pos] : 0);
	}
	bitmap_write_ascii (composite, stdout);
	buffer_free (composite);
}


int try_receive (void)
{
	struct wpc_message aMsg;
	struct wpc_message *msg = &aMsg;
	int rc;

	rc = udp_socket_receive (cpu_sock, 9000, msg, sizeof (aMsg));
	if (rc < 0)
		return rc;

#if 0
		printf ("Code: %02X   Time: %09d   Len: %03d\n",
			msg->code, msg->timestamp, msg->len);
#endif

	switch (msg->code)
	{
		case CODE_DMD_PAGE:
			process_dmd_page (msg);
			break;

		case CODE_DMD_VISIBLE:
			process_visible_pages (msg);
			break;
		}
	return 0;
}


int main (int argc, char *argv[])
{
	struct buffer *buf;

	cpu_sock = udp_socket_create (9001);
	for (;;)
	{
		usleep (10 * 1000UL);
		try_receive ();
	}
#if 0
		buf = get_frame ();
		if (buf)
			update_view (buf);
#endif
}

