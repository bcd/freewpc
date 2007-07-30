
#include <freewpc.h>
#include <window.h>
#include <test.h>

U8 tempo;

void music_mix_init (void)
{
	tempo = TIME_2S+TIME_500MS+TIME_66MS;
}

void music_mix_play (void)
{
#if 0
	music_set (MUS_SPIRAL_ROUND);
	for (;;)
	{
		task_sleep (tempo);
		sound_send (SND_GREED_ROUND_BOOM);
		task_sleep (tempo);
		sound_send (SND_FIST_BOOM1);
	}
#endif
}

void music_mix_draw (void)
{
	browser_draw ();
	sprintf ("TEMPO %d", tempo);
	font_render_string_left (&font_var5, 1, 16, sprintf_buffer);
}

void music_mix_left (void)
{
	tempo--;
	task_recreate_gid (GID_MIX_PLAY, music_mix_play);
}

void music_mix_right (void)
{
	tempo++;
	task_recreate_gid (GID_MIX_PLAY, music_mix_play);
}

void music_mix_enter (void)
{
	task_recreate_gid (GID_MIX_PLAY, music_mix_play);
}

void music_mix_exit (void)
{
	task_kill_gid (GID_MIX_PLAY);
	music_off ();
}

struct window_ops music_mix_window = {
	INHERIT_FROM_BROWSER,
	.init = music_mix_init,
	.enter = music_mix_enter,
	.exit = music_mix_exit,
	.left = music_mix_left,
	.right = music_mix_right,
	.draw = music_mix_draw,
};

struct menu music_mix_menu = {
	.name = "MUSIC MIX",
	.flags = M_ITEM,
	.var = { .subwindow = { &music_mix_window, NULL } },
};

