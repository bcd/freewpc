
#include "imglib.h"

int main (int argc, char *argv[])
{
	struct buffer *buf = bitmap_alloc (32, 32);
	bitmap_write_ascii (buf, stdout);

	exit (0);
}

