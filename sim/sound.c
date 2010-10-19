
#include <freewpc.h>
#include <simulation.h>
#include <hwsim/sound-ext.h>

static void sound_ext_reset (void)
{
#ifdef CONFIG_UI
	ui_write_sound_reset ();
#endif
}

void sound_ext_command (U16 cmd)
{
#ifdef CONFIG_UI
	ui_write_sound_command (cmd);
#endif
}

static void sound_ext_write_data (U8 val)
{
	static int write_count = 0;
	static U16 cmd = 0;

	cmd <<= 8;
	cmd |= val;
	write_count++;

	if (write_count == 2)
	{
		sound_ext_command (cmd);
		cmd = 0;
		write_count = 0;
	}
}


U8 sound_ext_read (void *board, unsigned int regno)
{
	return 0;
}


void sound_ext_write (void *board, unsigned int regno, U8 val)
{
	switch (regno)
	{
		case SOUND_ADDR_DATA:
			sound_ext_write_data (val);
			break;
		case SOUND_ADDR_RESET_STATUS:
			sound_ext_reset ();
			break;
	}
}

