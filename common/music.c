
#include <freewpc.h>

audio_track_t music_tracks[MAX_TRACKS];

U8 music_track_current;


void music_update (void)
{
	U8 i;
	U8 best_prio = 0;

	if (music_track_current == 0xFF)
	{
		for (i=0; i < MAX_TRACKS; i++)
		{
			if (music_tracks[i].prio > best_prio)
				music_track_current = i;
		}
	}

	dbprintf ("Current track = (%02X, %d)\n", 
		music_tracks[music_track_current].code,
		music_tracks[music_track_current].prio);

	if (music_track_current == 0xFF)
		music_off ();
	else
		music_set (music_tracks[music_track_current].code);
}


U8 music_find (audio_track_t track)
{
	U8 i;
	for (i=0; i < MAX_TRACKS; i++)
		if ((music_tracks[i].code == track.code)
			&& (music_tracks[i].prio == track.prio))
			return i;
	return 0xFF;
}


void music_start (audio_track_t track)
{
	U8 i;
	audio_track_t zero_track = { 0, };

	dbprintf ("music_start (%02X, %d)\n", track.code, track.prio);

	i = music_find (track);
	if (i != 0xFF)
	{
		dbprintf ("warning: started track that is already queued\n");
		return;
	}

	i = music_find (zero_track);
	if (i != 0xFF)
	{
		music_tracks[i] = track;

		if ((music_track_current == 0xFF)
			|| (music_tracks[music_track_current].prio < track.prio))
		{
			music_track_current = i;
			music_update ();
		}
	}
	else
	{
		dbprintf ("error: no free music tracks\n");
	}
}

void music_stop (audio_track_t track)
{
	U8 i;

	dbprintf ("music_stop (%02X, %d)\n", track.code, track.prio);
	
	i = music_find (track);
	if (i != 0xFF)
	{
		music_tracks[i].code = 0;
		music_tracks[i].prio = 0;

		if (i == music_track_current)
		{
			music_track_current = 0xFF;
			music_update ();
		}
	}
}

CALLSET_ENTRY (music, init)
{
	memset (music_tracks, 0, sizeof (music_tracks));
	music_track_current = 0xFF;
}

void music_stop_all (void)
{
	music_init ();
	music_update ();
}

