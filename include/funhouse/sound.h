
#ifndef _MACH_SOUND_H
#define _MACH_SOUND_H

#define MUS_MAIN_PLUNGER     0x01
#define MUS_MAIN             0x02
#define MUS_STEPS            0x04
#define MUS_1130             0x05
#define MUS_1145             0x06
#define MUS_MIDNIGHT         0x07
#define MUS_MULTIBALL        0x08
#define MUS_JACKPOT          0x09
#define MUS_RESTART_PLUNGER  0x0C
#define MUS_RESTART_RUNNING  0x0D
#define MUS_FRENZY           0x0E
#define MUS_SUPERDOG         0x0F
#define MUS_MULTIBALL2       0x10
#define MUS_HIGH_SCORE       0x11

#define SND_EXTRA_BALL       0x80
#define SND_LOCK_LIT         0x81
#define SND_WIND             0x83
#define SND_WIND_TUNNEL      0x86
#define SND_GONG             0x88
#define SND_RAMP_ENTER       0x89
#define SND_JET(x)           (0x8A + (x))
#define SND_LEFT_PLUNGER     0x8E
#define SND_TILT             0x90
#define SND_STEP_SPELLED     0x91
#define SND_TILT_WARNING     0x93
#define SND_COIN             0x94
#define SND_SKILL_SHOT       0x9C
#define SND_STEPS_AWARD      0x9D
#define SND_MILLION_AWARD    0xA9
#define SND_3WAY_COMBO       0xAA
#define SND_RAMP_MADE        0xAC
#define SND_4WAY_COMBO       0xAD
#define SND_RAMP_ENTER2      0xB6
#define SND_STEPS_TOP_AWARD  0xBC
#define SND_KICKOUT          0xBF
#define SND_GONG2            0xC0
#define SND_JACKPOT          0xC8
#define SND_MULTIBALL_START  0xC9
#define SND_RESTART_RING     0xCB
#define SND_QUICK_MB_AWARD   0xCC
#define SND_REPLAY           0xCE
#define SND_CHOMP            0xDF

#define SPCH_HEY             0x102
#define SPCH_YOU_CANT_HIDE   0x103
#define SPCH_I_SEE_YOU_NOW   0x104
#define SPCH_COME_BACK_HERE  0x105
#define SPCH_GETTING_LATE    0x107
#define SPCH_BIG_DEAL        0x108
#define SPCH_YOU_BIG         0x109
#define SPCH_BLEH            0x10E

#endif /* _MACH_SOUND_H */
