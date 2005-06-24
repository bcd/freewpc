#ifndef _SYS_SOUND_H
#define _SYS_SOUND_H

typedef uint8_t music_code_t, sound_code_t;

void music_off (void);
void music_set (music_code_t code);
void music_change (music_code_t code);
void sound_init (void);
void sound_send (sound_code_t code);

#endif /* _SYS_SOUND_H */
