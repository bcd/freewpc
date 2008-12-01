
#include <freewpc.h>
#include <simulation.h>


void wpc_sound_reset (void)
{
#ifdef CONFIG_UI
	ui_write_sound_reset ();
#endif
}

void wpc_sound_command (U16 cmd)
{
#ifdef CONFIG_UI
	ui_write_sound_command (cmd);
#endif
}

void wpc_sound_write (U8 val)
{
	static int write_count = 0;
	static U16 cmd = 0;

	cmd <<= 8;
	cmd |= val;
	write_count++;

	if (write_count == 2)
	{
		wpc_sound_command (cmd);
		cmd = 0;
		write_count = 0;
	}
}

