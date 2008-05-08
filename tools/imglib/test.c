
#include "imglib.h"


void compression_test (void)
{
	struct buffer *enc, *dec;
	struct buffer *buf;
	unsigned int x, y;
	int n;
	unsigned long pass;

	for (pass = 0; pass < 100000; pass++)
	{
		buf = frame_alloc ();

		for (n = 0; n < 200; n++)
		{
			buf->color = rand () % 2;
			bitmap_draw_pixel (buf, rand () % 128, rand () % 32);
		}

		buf = buffer_replace (buf, buffer_joinbits (buf));
		enc = buffer_compress (buf);
		dec = buffer_decompress (enc);

		if (buffer_compare (buf, dec))
		{
			printf ("error on pass %ld:\n", pass);
			buffer_write_c (buf, stdout);
			buffer_write_c (dec, stdout);
			return;
		}

		buffer_free (enc);
		buffer_free (dec);
		buffer_free (buf);
	}
}


int main (int argc, char *argv[])
{
	compression_test ();
	exit (0);
}

