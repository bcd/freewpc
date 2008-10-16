
#include <freewpc.h>

void dmd_fill_color_line (U16 *ptr, U8 color)
{
	ptr[0] = ptr[1] = (color & 0x01) ? 0xFFFF: 0;
	ptr[256] = ptr[257] = (color & 0x02) ? 0xFFFF: 0;
}

void dmd_fill_color_pattern (U8 color)
{
	U16 *p = (U16 *)dmd_low_buffer + color * 2;
	U8 n;
	for (n=0; n < 32; n++)
	{
		dmd_fill_color_line (p, color);
		p += 8;
	}
}

void dmd_show_color_pattern (void)
{
	U8 color;

	dmd_alloc_low_high ();
	for (color = 0; color < 3; color++)
		dmd_fill_color_pattern (color);
	dmd_show2 ();
}
