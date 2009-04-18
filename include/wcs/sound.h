/*
 * Copyright 2006, 2008, 2009 by Brian Dominy <brian@oddchange.com>
 *
 * This file is part of FreeWPC.
 *
 * FreeWPC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * FreeWPC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with FreeWPC; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _MACH_SOUND_H
#define _MACH_SOUND_H

#define MUS_MULTIBALL				MUSIC_SND(0x1)
#define MUS_LOCK_LIT					MUSIC_SND(0x2)
#define MUS_MAIN_LONG_ENTRY		MUSIC_SND(0x3)
#define MUS_MULTIBALL_LIT			MUSIC_SND(0x4)
#define MUS_MAIN_PLUNGER			MUSIC_SND(0x5)
#define MUS_MULTIBALL_LIT_PLUNGER	MUSIC_SND(0x6)
#define MUS_MULTIBALL_PLUNGER		MUSIC_SND(0x7)
#define MUS_EXTRA_BALL				MUSIC_SND(0xB)
#define MUS_JACKPOT_LIT				MUSIC_SND(0xC)
#define MUS_MAIN						MUSIC_SND(0x15)
#define MUS_MAIN_DRUM_ENTRY		MUSIC_SND(0x16)
#define MUS_CHAMPION					MUSIC_SND(0x17)
#define MUS_STRIKER_AWARD			MUSIC_SND(0x19)
#define MUS_STRIKER_LIT				MUSIC_SND(0x1A)
#define MUS_ULTRA_AWARD				MUSIC_SND(0x1B)
#define MUS_STRIKER_EB_AWARD		MUSIC_SND(0x1D)
#define MUS_ULTRA_COMPLETE			MUSIC_SND(0x1F)
#define MUS_BONUS_BONG				MUSIC_SND(0x21)
#define MUS_TEA_PARTY				MUSIC_SND(0x23)
#define MUS_TEA_PARTY_WON			MUSIC_SND(0x24)
#define MUS_BONUS1					MUSIC_SND(0x27)
#define MUS_BONUS2					MUSIC_SND(0x28)
#define MUS_GOAL_IS_LIT				MUSIC_SND(0x36)
#define MUS_STRIKER_IS_LIT       MUSIC_SND(0x37)
#define MUS_MULTIBALL_STARTED		MUSIC_SND(0x38)
#define MUS_MULTIBALL_RESTARTED	MUSIC_SND(0x39)
#define MUS_FREE_KICK_AWARD		MUSIC_SND(0x3A)
#define MUS_SUPER_FREE_KICK_AWARD	MUSIC_SND(0x3B)
#define MUS_JACKPOT_RELIT        MUSIC_SND(0x3C)
#define MUS_TICKET_BOUGHT        MUSIC_SND(0x3D)
#define MUS_DRUMROLL             MUSIC_SND(0x3E)
#define MUS_SPECIAL_AWARD        MUSIC_SND(0x40)
#define MUS_SKILL_SHOT_CITY		MUSIC_SND(0x42)
#define MUS_FANFARE              MUSIC_SND(0x43)
#define MUS_SUPER_FREE_KICK_RUNNING	MUSIC_SND(0x44)
#define MUS_RESTART_RUNNING		MUSIC_SND(0x46)
#define MUS_STRIKER_EB_RUNNING	MUSIC_SND(0x48)
#define MUS_BIG_GOAL_RUNNING		MUSIC_SND(0x4A)
#define MUS_HIT_GOALIE_RUNNING	MUSIC_SND(0x4C)
#define MUS_WHERES_STRIKER_RUNNING	MUSIC_SND(0x4E)
#define SND_BUZZER					MUSIC_SND(0x64)
#define SND_LOW_KICK             MUSIC_SND(0x65)
#define SND_AIRPLANE_FLYOVER     MUSIC_SND(0x6C)
#define SND_WHISTLE              MUSIC_SND(0x6E)
#define SND_JET_BUMPER           MUSIC_SND(0x6F)
#define SND_FIREWORK_EXPLODE     MUSIC_SND(0x71)
#define SND_FIREWORK_LAUNCH1     MUSIC_SND(0x72)
#define SND_MAGNA_GOALIE         MUSIC_SND(0x73)
#define SND_UHHH                 MUSIC_SND(0x77)
#define SND_FREE_KICK_LIT        MUSIC_SND(0x78)
#define SND_SLINGSHOT            MUSIC_SND(0x7C)
#define SND_UNLIT_STRIKER        MUSIC_SND(0x7E)
#define SND_SPINNER              MUSIC_SND(0x80)
#define SND_DING1                MUSIC_SND(0x86)
#define SND_DING2                MUSIC_SND(0x87)
#define SND_DING3                MUSIC_SND(0x88)
#define SND_DING4                MUSIC_SND(0x89)
#define SND_INLANE               MUSIC_SND(0x8C)
#define SND_DRIBBLE              MUSIC_SND(0x8D)
#define SND_KICK                 MUSIC_SND(0x8E)
#define SND_LOCK_MAGNET          MUSIC_SND(0x8F)
#define SND_TOP_LANE_LIT         MUSIC_SND(0x92)
#define SND_TOP_LANE_UNLIT       MUSIC_SND(0x93)
#define SND_ULTRA_RAMP           MUSIC_SND(0x95)
#define SND_DING                 MUSIC_SND(0x96)
#define SND_FIREWORK_LAUNCH2     MUSIC_SND(0x97)
/* In the real game, TV kickout sound is a sequence of
 * SND_PLUNGE twice followed by SND_TV_KICKOUT. */
#define SND_TV_KICKOUT           MUSIC_SND(0x98)
#define SND_MOO                  MUSIC_SND(0x9A)
#define SND_EB_LANDING           MUSIC_SND(0x9B)
#define SND_CROWD_LOOP           MUSIC_SND(0x9C)
#define SND_CROWD_NOISE          MUSIC_SND(0x9D)
#define SND_CROWD_CHEER          MUSIC_SND(0xA0)
#define SND_CROWD_HAPPY          MUSIC_SND(0xA1)
#define SND_CROWD_AWW            MUSIC_SND(0xA2)
#define SND_CROWD_BOO            MUSIC_SND(0xA3)
#define SND_WAIT_BELL            MUSIC_SND(0xA4)
#define SND_PING                 MUSIC_SND(0xA5)
#define SND_CLICK                MUSIC_SND(0xA6)
#define SND_TV_STATIC            MUSIC_SND(0xA7)
#define SND_ASTHMA_ATTACK        MUSIC_SND(0xA8)
#define SND_GULP                 MUSIC_SND(0xAB)
#define SND_RETURN_TO_PLAY       MUSIC_SND(0xAC)
#define SND_ROCKET_LIGHT         MUSIC_SND(0xAD)
#define SND_ROCKET_LAUNCH        MUSIC_SND(0xAE)
#define SND_CHALKBOARD           MUSIC_SND(0xAF)
#define SND_UNLIT_GOAL           MUSIC_SND(0xB0)
#define SND_POW                  MUSIC_SND(0xB1)
#define SND_BIRDIE               MUSIC_SND(0xB2)
#define SND_PLUNGE               MUSIC_SND(0xBB)
#define SND_COUNT5               MUSIC_SND(0xBC)
#define SND_COUNT4               MUSIC_SND(0xBD)
#define SND_COUNT3               MUSIC_SND(0xBE)
#define SND_COUNT2               MUSIC_SND(0xBF)
#define SND_COUNT1               MUSIC_SND(0xC0)
#define SND_OUTLANE              MUSIC_SND(0xCC)

#define SPCH_SKILL                       SPEECH_SND(0x2C)
#define SPCH_SPEED                       SPEECH_SND(0x2D)
#define SPCH_SPIRIT                      SPEECH_SND(0x2E)
#define SPCH_STAMINA                     SPEECH_SND(0x2F)
#define SPCH_STRENGTH                    SPEECH_SND(0x30)
#define SPCH_ULTRA_GOALIE                SPEECH_SND(0x31)
#define SPCH_ULTRA_JETS                  SPEECH_SND(0x32)
#define SPCH_GOALLL                      SPEECH_SND(0x33)
#define SPCH_ULTRA_RAMPS                 SPEECH_SND(0x34)
#define SPCH_ULTRA_SPINNER               SPEECH_SND(0x35)
#define SPCH_NEXT_STOP_CHICAGO           SPEECH_SND(0x36)
#define SPCH_NEXT_STOP_DALLAS            SPEECH_SND(0x37)
#define SPCH_NEXT_STOP_BOSTON            SPEECH_SND(0x38)
#define SPCH_NEXT_STOP_NEW_YORK          SPEECH_SND(0x39)
#define SPCH_NEXT_STOP_NEW_JERSEY        SPEECH_SND(0x3A)
#define SPCH_NEXT_STOP_ORLANDO           SPEECH_SND(0x3B)
#define SPCH_NEXT_STOP_WASHINGTON_DC     SPEECH_SND(0x3C)
#define SPCH_NEXT_STOP_SAN_FRANCISCO     SPEECH_SND(0x3D)
#define SPCH_NEXT_STOP_DETROIT           SPEECH_SND(0x3E)
#define SPCH_LANDING_IN_CHICAGO          SPEECH_SND(0x3F)
#define SPCH_LANDING_IN_DALLAS           SPEECH_SND(0x40)
#define SPCH_LANDING_IN_BOSTON           SPEECH_SND(0x41)
#define SPCH_LANDING_IN_NEW_YORK         SPEECH_SND(0x42)
#define SPCH_LANDING_IN_NEW_JERSEY       SPEECH_SND(0x43)
#define SPCH_LANDING_IN_ORLANDO          SPEECH_SND(0x44)
#define SPCH_LANDING_IN_WASHINGTON_DC    SPEECH_SND(0x45)
#define SPCH_LANDING_IN_SAN_FRANCISCO    SPEECH_SND(0x46)
#define SPCH_LANDING_IN_DETROIT          SPEECH_SND(0x47)
#define SPCH_WELCOME_TO_DETROIT          SPEECH_SND(0x50)
#define SPCH_ENJOY_STAY_DETROIT          SPEECH_SND(0x59)
#define SPCH_FLIGHT_501_DETROIT          SPEECH_SND(0x62)
#define SPCH_PREPARE_LAND_DETROIT        SPEECH_SND(0x6B)
#define SPCH_ULTRA_EVERYTHING            SPEECH_SND(0x6C)
#define SPCH_RAIDEN                      SPEECH_SND(0x6D)
#define SPCH_NICE_CAR                    SPEECH_SND(0x6E)
#define SPCH_RUDY_PALS                   SPEECH_SND(0x6F)
#define SPCH_STAY_COOL_DADDY_O           SPEECH_SND(0x70)
#define SPCH_ADMIRAL_BIAGI_OUT           SPEECH_SND(0x71)
#define SPCH_SCREAM                      SPEECH_SND(0x72)

#define SPCH_SHOOT_THE_GOAL       0x07DCUL
#define SPCH_ANOTHER_GOAL         (0x07E7UL + (n))

#define SPCH_YELLOW_CARD          0x07FEUL
#define SPCH_RED_CARD             0x07FFUL

#define SPCH_OUT_OF_BOUNDS        0x0802UL
#define SPCH_CORNER_KICK(n)       (0x0803UL + (n)) /* 3 choices */
#define SPCH_INSTANT_REPLAY(n)    (0x0807UL + (n)) /* 2 choices */
#define SPCH_IN_OVERTIME          0x0819UL
#define SPCH_FEW_SECONDS_REMAIN   0x081BUL
#define SPCH_VICTORY_LAP(n)       (0x081CUL + (n)) /* 2 choices */
#define SPCH_REPLAY               0x08A3UL
#define SPCH_SHOOT_AGAIN          0x08A5UL




#endif /* _MACH_SOUND_H */
