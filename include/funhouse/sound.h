
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
#define SND_SKILL2           0x9F
#define SND_SPECIAL_LIT      0xA0
#define SND_FRENZY_LIT       0xA1
#define SND_MILLION_AWARD    0xA9
#define SND_3WAY_COMBO       0xAA
#define SND_RAMP_MADE        0xAC
#define SND_4WAY_COMBO       0xAD
#define SND_LOCK_MB_KICK     0xB0
#define SND_RAMP_ENTER2      0xB6
#define SND_KICKOUT          0xBA
#define SND_STEPS_TOP_AWARD  0xBC
#define SND_KICKOUT_COMING   0xBF
#define SND_GONG2            0xC0
//#define SND_RAMP_MADE        0xC2
#define SND_BOOM1            0xC6
#define SND_BOOM2            0xC7
#define SND_JACKPOT          0xC8
#define SND_MULTIBALL_START  0xC9
#define SND_RESTART_RING     0xCB
#define SND_QUICK_MB_AWARD   0xCC
#define SND_REPLAY           0xCE
#define SND_TARGET1          0xD3
#define SND_TARGET2          0xD4
#define SND_BONUS_BLIP1      0xD8
#define SND_BONUS_BLIP2      0xD9
#define SND_BONUS_FRENZY     0xE0
#define SND_TARGET_COMPLETE  0xE6
#define SND_CHOMP            0xDF

#define SPCH_CHUCKLE         0x101
#define SPCH_HEY             0x102
#define SPCH_YOU_CANT_HIDE   0x103
#define SPCH_I_SEE_YOU_NOW   0x104
#define SPCH_COME_BACK_HERE  0x105
#define SPCH_GETTING_LATE    0x107
#define SPCH_BIG_DEAL        0x108
#define SPCH_YOU_BIG         0x109
#define SPCH_BLEH            0x10E
#define SPCH_NO_WAY          0x110
#define SPCH_STOP_RUN_AROUND 0x113
#define SPCH_STOP_IT         0x114
#define SPCH_OWW             0x118
#define SPCH_THATS_NOT_FUNNY 0x119
#define SPCH_CLOSE_THE_DOOR  0x11A
#define SPCH_FUNHOUSE        0x11C
#define SPCH_COME_ON_IN      0x126
#define SPCH_FUNHOUSE_CLOSED 0x12A
#define SPCH_WAY_TO_GO       0x12E
#define SPCH_YUM_YUM         0x130
#define SPCH_SHOOT_AGAIN     0x136
#define SPCH_FRENZY          0x139
#define SPCH_WHAT_WAS_THAT   0x13C
#define SPCH_NICE_GOING      0x13E
#define SPCH_HOWDY           0x13F
#define SPCH_YOURE_UP        0x141
#define SPCH_FEEL_LIKE_1M    0x14B
#define SPCH_MAYBE_NEXT_TIME 0x14E

#endif /* _MACH_SOUND_H */
