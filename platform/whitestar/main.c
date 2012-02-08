
#include <freewpc.h>
#undef int

__fastram__ U8 wpc_rom_bank;

void platform_init (void)
{
}

int main (void)
{
	freewpc_init ();
	return 0;
}

