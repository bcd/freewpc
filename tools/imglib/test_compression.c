
#include "imglib.h"


void compression_test (int passes)
{
	struct buffer *enc, *dec;
	struct buffer *buf;
	unsigned int x, y;
	int n;
	unsigned long pass;

	for (pass = 0; pass < passes; pass++)
	{
		/* Allocate a test frame */
		buf = frame_alloc ();

		/* Do 200 random pixel writes into the frame. */
		for (n = 0; n < 200; n++)
		{
			buf->color = rand () % 2;
			bitmap_draw_pixel (buf, rand () % 128, rand () % 32);
		}

		/* Convert into joined format */
		buf = buffer_replace (buf, buffer_joinbits (buf));

		/* Test that compression and decompression are true
		inverses. */
		enc = buffer_compress (buf, NULL);
		dec = buffer_decompress (enc);
		if (buffer_compare (buf, dec))
		{
			printf ("error on pass %ld:\n", pass);
			buffer_write_c (buf, stdout);
			buffer_write_c (dec, stdout);
			exit (1);
		}

		buffer_free (enc);
		buffer_free (dec);
		buffer_free (buf);
	}
}


int main (int argc, char *argv[])
{
	int passes = 10000;
	if (argc > 1)
		passes = strtoul (argv[1], NULL, 10);
	compression_test (passes);
	printf ("%d passes ran successfully.\n", passes);
	exit (0);
}

