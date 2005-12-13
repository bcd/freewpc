#ifndef _SYS_SOUND_H
#define _SYS_SOUND_H

/* Predefined, fixed system sound values */
#if (MACHINE_DCS == 0)
#define SND_TEST_DOWN			0x50
#define SND_TEST_UP				0x51
#define SND_TEST_ABORT			0x52
#define SND_TEST_CONFIRM		0x53
#define SND_TEST_ALERT			0x54
#define SND_TEST_ENTER			0x57
#define SND_TEST_EXIT			0x58
#define SND_TEST_SCROLL			0x59
#else

#define MUSIC_SND(x)				((0UL << 8) + (x))
#define SPEECH_SND(x)			((1UL << 8) + (x))
#define TEST_SND(x)				((3UL << 8) + (x))

#define SND_TEST_UP				TEST_SND(212)
#define SND_TEST_ALERT			TEST_SND(216)
#define SND_TEST_BONG			TEST_SND(216)
#define SND_TEST_DOWN			TEST_SND(218)
#define SND_TEST_SCROLL			TEST_SND(218)
#define SND_TEST_ABORT			TEST_SND(218)
#define SND_TEST_CONFIRM		TEST_SND(218)
#define SND_TEST_ENTER			TEST_SND(219)
#define SND_TEST_ESCAPE			TEST_SND(219)
#define SND_TEST_EXIT			TEST_SND(220)
#endif

#define MIN_VOLUME				0
#define MAX_VOLUME				32
#define DEFAULT_VOLUME			8

typedef uint16_t music_code_t, sound_code_t;

void music_off (void);
void music_set (music_code_t code);
void music_change (music_code_t code);
void sound_rtt (void);
void sound_init (void);
void sound_send (sound_code_t code);
void sound_reset (void);
void volume_down (void);
void volume_up (void);
void volume_update (void);

#endif /* _SYS_SOUND_H */
