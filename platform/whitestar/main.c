
#include <freewpc.h>
#undef int

/* TODO: Fix not to use this */
__fastram__ U8 wpc_rom_bank;

void exit (void)
{
	fatal (ERR_LIBC_ABORT);
}


/** Called by the standard library on some errors.
 * This maps to our fatal() function. */
void abort (void)
{
	fatal (ERR_LIBC_ABORT);
}


int main (void)
{
	freewpc_init ();
	return 0;
}

