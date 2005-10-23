#ifndef _SYS_SOUND_H
#define _SYS_SOUND_H

/* Predefined, fixed system sound values */
#define SND_DOWN					0x50
#define SND_UP						0x51
#define SND_ABORT					0x52
#define SND_CONFIRM				0x53
#define SND_ALERT					0x54
#define SND_ENTER					0x57
#define SND_EXIT					0x58
#define SND_SCROLL				0x59

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
