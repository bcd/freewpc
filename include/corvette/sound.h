/*
 * Copyright 2010 by Dominic Clifton <me@dominicclifton.name>
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

//
// Music and Sound Effects
//

#define MUS_MAIN_PLUNGER			MUSIC_SND(0x1)
#define MUS_MAIN					MUSIC_SND(0x2)
#define MUS_MAIN_02					MUSIC_SND(0x3)
#define MUS_ZR1_LOCK				MUSIC_SND(0x5) // Behind "Flip to rev!"
#define MUS_MULTIBALL				MUSIC_SND(0x6)
#define MUS_RACE					MUSIC_SND(0x7)
#define MUS_MULTIBALL_REMATCH		MUSIC_SND(0x8) // ALSO PIT-IN Hurry Hup

#define MUS_DRAG_RACE				MUSIC_SND(0xB) // Behind "Sunday Sunday..."
#define MUS_UNKNOWN_02				MUSIC_SND(0xC) // Seems same as MUS_MULTIBALL_REMATCH
#define MUS_CATCH_ME				MUSIC_SND(0xD)
#define MUS_RACE_MULTIBALL			MUSIC_SND(0xE)99
#define MUS_END_GAME				MUSIC_SND(0xF) // Music played after all players have finished their game
#define MUS_UNKNOWN_04				MUSIC_SND(0x10) // Score entry
#define MUS_ROUTE_66				MUSIC_SND(0x11) // Short
#define MUS_UNKNOWN_05				MUSIC_SND(0x12) // Short guitar + drum intro
#define MUS_MULTIBALL_PLUNGER		MUSIC_SND(0x13) // Short drum solo

#define SND_CHING_01						MUSIC_SND(0x68)
#define SND_PNEUMATIC_TORQUE_WRENCH_01		MUSIC_SND(0x6D) // Short
#define SND_PNEUMATIC_TORQUE_WRENCH_02		MUSIC_SND(0x6E) // Longer
#define SND_DLLLLING						MUSIC_SND(0x71)
#define SND_ENGINE_REV_01					MUSIC_SND(0x72)
#define SND_ENGINE_REV_02					MUSIC_SND(0x73)
#define SND_TIRE_SCREECH_01					MUSIC_SND(0x77) // Burnout
#define SND_TIRE_SCREECH_02					MUSIC_SND(0x79) // Brake

#define SND_GUITAR_01						MUSIC_SND(0x7B)
#define SND_GUITAR_02						MUSIC_SND(0x7C)
#define SND_GUITAR_03						MUSIC_SND(0x7D)
#define SND_AWARD_01						MUSIC_SND(0x7E) // Engine Rev + Music
#define SND_DITTY_01						MUSIC_SND(0x7F) // Screaming Guitar + Piano low
#define SND_DITTY_02						MUSIC_SND(0x80) // Screaming Guitar + Piano medium
#define SND_DITTY_03						MUSIC_SND(0x81) // Screaming Guitar + Piano high

#define SND_EXPLOSION_01					MUSIC_SND(0x82)
//#define SND_RACE_STARTER_01					MUSIC_SND(0x83) // Dupliate of 0x8B
#define SND_SPINNER							MUSIC_SND(0x84)

#define SND_DLLLLUNG_01						MUSIC_SND(0x86) // Low (L. Standup)
#define SND_DLLLLUNG_02						MUSIC_SND(0x87) // Medium (L. Standup)
#define SND_DLLLLUNG_03						MUSIC_SND(0x88) // High (L.Standup)

#define SND_TRAFFIC_HORN_01					MUSIC_SND(0x89)
#define SND_CAR_CRASH_01					MUSIC_SND(0x8A) // Tire screech + collision
#define SND_RACE_STARTER_01					MUSIC_SND(0x8B) // Low (Get ready)
#define SND_RACE_STARTER_02					MUSIC_SND(0x8C) // High + Long (Go)

#define SND_ENGINE_REV_03					MUSIC_SND(0x8D) // Raspy V8 Rev - Kickback
#define SND_DLDLDLING						MUSIC_SND(0x8E) // Guitar + cymbal
#define SND_ENGINE_START_REV_IDLE			MUSIC_SND(0x8F) // See 0xFE

// Used during original rom's drag race
#define SND_GEAR_CHANGE_01					MUSIC_SND(0x90) // Drag race 1st gear
#define SND_GEAR_CHANGE_02					MUSIC_SND(0x91) // Drag race 2nd gear
#define SND_GEAR_CHANGE_03					MUSIC_SND(0x92) // Drag race 3rd gear
#define SND_GEAR_CHANGE_04					MUSIC_SND(0x93)
#define SND_ENGINE_REV_04					MUSIC_SND(0x94) // Quick Rev
#define SND_ENGINE_REV_05					MUSIC_SND(0x95) // Double Rev

// Used during original rom's ZR-1 Multiball
#define SND_ENGINE_REV_THEN_IDLE_01			MUSIC_SND(0x96)
#define SND_ENGINE_REV_THEN_IDLE_02			MUSIC_SND(0x97)
#define SND_ENGINE_REV_THEN_IDLE_03			MUSIC_SND(0x98)
#define SND_ENGINE_IDLE_01					MUSIC_SND(0x99)
#define SND_BURNOUT_01						MUSIC_SND(0x9A) // Rev + Tire screech then engine fade into distance

#define SND_RAUCOUS_FADE					MUSIC_SND(0x9B) // Original rom uses during attract mode
#define SND_RAUCOUS_QUICK					MUSIC_SND(0x9C)

#define SND_TRAFFIC_ZOOM_01					MUSIC_SND(0x9D)
#define SND_JACKPOT_01						MUSIC_SND(0x9E) // Used during Race Multiball
#define SND_TIRE_SCREECH_03					MUSIC_SND(0x9F) // Brake
#define SND_TIRE_SCREECH_04					MUSIC_SND(0xA0) // Brake
#define SND_TRAFFIC_ZOOM_02					MUSIC_SND(0xA1)
#define SND_TRAFFIC_ZOOM_03					MUSIC_SND(0xA2)
#define SND_TRAFFIC_ZOOM_04					MUSIC_SND(0xA3)
#define SND_TRAFFIC_ZOOM_05					MUSIC_SND(0xA4)

#define SND_DITTY_04						MUSIC_SND(0xA6)
#define SND_DITTY_05						MUSIC_SND(0xA7)
#define SND_BEEP_BEEP						MUSIC_SND(0xA8) // ZR-1 Lock Release
#define SND_DITTY_06						MUSIC_SND(0xA9)
#define SND_BEEP							MUSIC_SND(0xAA) // Right Return Lane
#define SND_ENGINE_REV_06					MUSIC_SND(0xAB) // Weird Rev
#define SND_BONUS_AWARD_01					MUSIC_SND(0xAE) // First
#define SND_BONUS_AWARD_02					MUSIC_SND(0xAF) // Repeat n times
#define SND_BONUS_AWARD_03					MUSIC_SND(0xB0) // Last (balls remaining)
#define SND_BONUS_AWARD_04					MUSIC_SND(0xB1) // Last (no balls remaining)
#define SND_CYMBAL_CRASH_01					MUSIC_SND(0xB3)
#define SND_OUT_LANE						MUSIC_SND(0xB4)
//#define SND_RACE_STARTER_03				MUSIC_SND(0xB5) // Duplicate of SND_RACE_STARTER_01 0x8B
//#define SND_RACE_STARTER_04				MUSIC_SND(0xB6) // Duplicate of SND_RACE_STARTER_02 0x8C
#define SND_HIGH_LIFT_CAMS_AWARD			MUSIC_SND(0xB7)
#define SND_CAR_AWARDED						MUSIC_SND(0xB8) // TODO I think.
#define SND_ROUTE_66						MUSIC_SND(0xB9)
#define SND_JACKPOT_02						MUSIC_SND(0xBA) // Jackpot - Similar to 0x9E, Used during ZR-1 Multiball
#define SND_BIG_BLOCK_AWARD					MUSIC_SND(0xBB)
#define SND_BURNOUT_02						MUSIC_SND(0xBC) // Rev + Tire screech then engine fade into distance
#define SND_EXTRA_BALL_AWARD				MUSIC_SND(0xBD)
#define SND_EXTRA_BALL_AWARD_SHORT			MUSIC_SND(0xBE) // Last part of 0xBD
#define SND_REPLAY_AWARD					MUSIC_SND(0xBF)
#define SND_REPLAY_AWARD_SHORT				MUSIC_SND(0xC0) // Last part of 0xBF
#define SND_JACKPOT_03						MUSIC_SND(0xC1) // Explosion Sound
#define SND_AWARD_02						MUSIC_SND(0xC2) // Super Jackpot ?
#define SND_AWARD_03						MUSIC_SND(0xC3)
#define SND_AWARD_04						MUSIC_SND(0xC4) // ZR-1 Ball Lock
#define SND_AWARD_05						MUSIC_SND(0xC5) // Garage door noise
#define SND_AWARD_06						MUSIC_SND(0xC6)
#define SND_KISS							MUSIC_SND(0xC7) // Catch me - Caught!
#define SND_ENGINE_NOISE_01					MUSIC_SND(0xC8) // Looping
#define SND_ENGINE_NOISE_02					MUSIC_SND(0xC9) // Looping
#define SND_ENGINE_NOISE_03					MUSIC_SND(0xCA) // Looping
#define SND_ENGINE_NOISE_04					MUSIC_SND(0xCB) // Looping
#define SND_ENGINE_NOISE_05					MUSIC_SND(0xCC) // Looping
#define SND_ENGINE_NOISE_06					MUSIC_SND(0xCD) // Looping
#define SND_ENGINE_NOISE_07					MUSIC_SND(0xCE) // Looping
#define SND_ENGINE_NOISE_08					MUSIC_SND(0xCF) // Looping
#define SND_ENGINE_NOISE_09					MUSIC_SND(0xD1) // Looping
#define SND_RACE_01							MUSIC_SND(0xD2) // Long Rev
#define SND_RACE_02							MUSIC_SND(0xD3) // Long Rev
#define SND_RACE_03							MUSIC_SND(0xD4) // Long Rev
#define SND_RACE_04							MUSIC_SND(0xD5) // Long Rev
#define SND_RACE_05							MUSIC_SND(0xD6) // Long Rev
#define SND_RACE_06							MUSIC_SND(0xD7) // Long Rev
#define SND_RACE_07							MUSIC_SND(0xD8) // Long Rev
#define SND_RACE_08							MUSIC_SND(0xD9) // Long Rev
#define SND_RACE_09							MUSIC_SND(0xDA) // Long Rev
#define SND_END_RACE_01						MUSIC_SND(0xDD)
#define SND_END_RACE_02						MUSIC_SND(0xDE)
#define SND_END_RACE_03						MUSIC_SND(0xDF)
#define SND_END_RACE_04						MUSIC_SND(0xE0)
#define SND_END_RACE_05						MUSIC_SND(0xE1)
#define SND_END_RACE_06						MUSIC_SND(0xE2)
#define SND_END_RACE_07						MUSIC_SND(0xE3)
#define SND_END_RACE_08						MUSIC_SND(0xE4)
#define SND_CROWD_CHEER_01					MUSIC_SND(0xE7)
#define SND_CROWD_CHEER_02					MUSIC_SND(0xE8)
#define SND_CROWD_CHEER_03					MUSIC_SND(0xE9)
#define SND_CROWD_CHEER_04					MUSIC_SND(0xEA)
#define SND_CROWD_CHEER_05					MUSIC_SND(0xEB)
#define SND_ROUTE_66_SIGN_POST_WOBBLE		MUSIC_SND(0xEC)
#define SND_DITTY_07						MUSIC_SND(0xED) // ZR-1 ball lock without garage door sound, fades

#define SND_WOOSH							MUSIC_SND(0xEE)
#define SND_SPARK_PLUG_01					MUSIC_SND(0xF0) // Short
#define SND_SPARK_PLUG_02					MUSIC_SND(0xF1) // Long
#define SND_CREDIT							MUSIC_SND(0xF2)
#define SND_AWARD_07						MUSIC_SND(0xF3)
#define SND_SPARK_PLUG_03					MUSIC_SND(0xF4) // Short
#define SND_SPARK_PLUG_04					MUSIC_SND(0xF5) // Long
#define SND_AWARD_08						MUSIC_SND(0xF6)
#define SND_AWARD_09						MUSIC_SND(0xF7)
#define SND_ROUTE_66_QUICK					MUSIC_SND(0xF8)
#define SND_EXPLOSION_03					MUSIC_SND(0xF9)
#define SND_BLASTER							MUSIC_SND(0xFA) // Kickback Enabled
#define SND_BOING							MUSIC_SND(0xFB) // Kickback Standup
#define SND_STARTER_MOTOR					MUSIC_SND(0xFC)
#define SND_BALL_LAUNCH_REV_01				MUSIC_SND(0xFD)
#define SND_BALL_LAUNCH_REV_02				MUSIC_SND(0xFE)
#define SND_BALL_LAUNCH_REV_03				MUSIC_SND(0xFE)

//
// Speech
//

#define SPCH_KISS							SPEECH_SND(0x02)
#define SPCH_BEEP_01						SPEECH_SND(0x03)
#define SPCH_BEEP_02						SPEECH_SND(0x04)

#define SPCH_JINGLE_01						SPEECH_SND(0x05)
#define SPCH_EFFECT_01						SPEECH_SND(0x06) // Sounds very similar to MUSIC_SND(0x9E)

#define SPCH_FR_BONJOUR						SPEECH_SND(0x2C)
#define SPCH_FR_JE_MAPPELLE_BRIGET			SPEECH_SND(0x2D)
#define SPCH_IT_CIAO						SPEECH_SND(0x2E)
#define SPCH_IT_SONO_GINA					SPEECH_SND(0x2F)
// ... 0x37 in french/italian catch me women's voices
// 0x38 N/A
// 0x39 - 0x54 german?dutch?/spanish catch me women's voices

#define SPCH_SUNDAY_SUNDAY_DRAG_RACE_INTRO	SPEECH_SND(0x58)
#define SPCH_GREETING_RACE_FANS_INTRO		SPEECH_SND(0x59)
#define SPCH_GM_PARTS_ENDURANCE				SPEECH_SND(0x6E) // Hamish
#define SPCH_PULLING_INTO_PITS				SPEECH_SND(0x6F) // Hamish
#define SPCH_AYE_BRET_THATS_FOR_SURE		SPEECH_SND(0x70) // Hamish
#define SPCH_SLOW_PIT_CREW					SPEECH_SND(0x71) // Hamish
#define SPCH_HES_BEEN_OUT_FOR_A_LONG_TIME	SPEECH_SND(0x72) // Hamish
#define SPCH_HES_SLIDING_AROUND				SPEECH_SND(0x73) // Hamish
#define SPCH_HES_GOT_TO_DECIDE_PIT_OR_RACE	SPEECH_SND(0x74) // Hamish
#define SPCH_PIT_CREW_WASHING_CAR			SPEECH_SND(0x75) // Hamish
#define SPCH_ENGINE_TROUBLE					SPEECH_SND(0x76) // Hamish
#define SPCH_BIT_LOOSE_IN_THE_TURNS			SPEECH_SND(0x77) // Hamish
#define SPCH_OCH_NAY_LEGS_ARE_BANDY			SPEECH_SND(0x78) // Hamish
#define SPCH_HELLO_THIS_IS_BRETT			SPEECH_SND(0x79) // Brett
#define SPCH_CORVETTE_5TH					SPEECH_SND(0x86) // Brett
#define SPCH_CONDITIONS_GRUELLING			SPEECH_SND(0x87) // Brett
#define SPCH_CORVETTE_TAKEN_LEAD			SPEECH_SND(0x8B) // Brett
#define SPCH_CORVETTE_FALLEN_BACK			SPEECH_SND(0x8C) // Brett
#define SPCH_PIT_CREW_WORKING_FURIOUSLY		SPEECH_SND(0x90) // Brett
#define SPCH_INCREDIBLE_PIT_STOP_HAMISH		SPEECH_SND(0x91) // Brett
#define SPCH_PIT_CREW_HAVE_TO_DO_BETTER		SPEECH_SND(0x92) // Brett
#define SPCH_SMOKE_FROM_CORVETTE_HAMISH		SPEECH_SND(0x95) // Brett
#define SPCH_HAVING_TROUBLE_TRAFFIC_HAMISH	SPEECH_SND(0x96) // Brett
#define SPCH_KICK_IT_UP_A_NOTCH_TO_WIN		SPEECH_SND(0x97) // Brett
#define SPCH_UNDER_THE_YELLOW				SPEECH_SND(0x98) // Brett
#define SPCH_BLACK_FLAGGED_OUT_OF_RACE		SPEECH_SND(0x99) // Brett
#define SPCH_YOU_WANNA_RACE					SPEECH_SND(0x9A)
#define SPCH_NOT_YOU_AGAIN					SPEECH_SND(0x9E)
#define SPCH_SO_YOURE_BACK_FOR_MORE			SPEECH_SND(0x9F)
#define SPCH_HOUAH							SPEECH_SND(0xA0)
#define SPCH_GET_THE_EXTRA_BALL				SPEECH_SND(0xA1)
#define SPCH_GET_THE_JACKPOT				SPEECH_SND(0xA2)
#define SPCH_TAKE_YOUR_BEST_SHOT			SPEECH_SND(0xA3)
#define SPCH_PUT_THE_HAMMER_DOWN			SPEECH_SND(0xA4)
#define SPCH_OOOOH_BABY						SPEECH_SND(0xA5)
#define SPCH_REV_IT_TO_THE_REDLINE			SPEECH_SND(0xA6)
#define SPCH_FLIP_TO_REV					SPEECH_SND(0xA7)
#define SPCH_YOU_WANT_THIS_BLUE_FLAME		SPEECH_SND(0xA8)
#define SPCH_YOU_WANT_THIS_GRAN_SPORT		SPEECH_SND(0xA9)
#define SPCH_YOU_WANT_THIS_FUELIE			SPEECH_SND(0xAA)
#define SPCH_YOU_WANT_THIS_67_STINGRAY		SPEECH_SND(0xAB)
#define SPCH_YOU_WANT_THIS_LT1				SPEECH_SND(0xAC)
#define SPCH_YOU_WANT_THIS_PACE_CAR			SPEECH_SND(0xAD)
#define SPCH_YOU_WANT_THIS_L83				SPEECH_SND(0xAE)
#define SPCH_YOU_WANT_THIS_CHALLENGE_CAR	SPEECH_SND(0xAF)
#define SPCH_YOU_WANT_THIS_ZR1				SPEECH_SND(0xB0)
#define SPCH_YOURE_GONNA_HAVE_TO_RACE		SPEECH_SND(0xB3) // for it

#define SPCH_YOU_NEED_BIG_BRAKES			SPEECH_SND(0xB5)
#define SPCH_YOU_NEED_A_FUELIE				SPEECH_SND(0xB6)
#define SPCH_YOU_NEED_SOME_NITROUS			SPEECH_SND(0xB7)
#define SPCH_YOU_NEED_Z07_SUSPENSION		SPEECH_SND(0xB8)
#define SPCH_YOU_NEED_A_BIG_BLOCK			SPEECH_SND(0xB9)
#define SPCH_YOU_NEED_SOME_HIGH_LIFT_CAMS	SPEECH_SND(0xBA)
#define SPCH_YOU_NEED_STICKY_TIRES			SPEECH_SND(0xBB)
#define SPCH_YOU_NEED_SUPERCHARGER			SPEECH_SND(0xBC)
#define SPCH_YOU_NEED_SIX_SPEED_TRANNY		SPEECH_SND(0xBD)
#define SPCH_YOU_NEED_SOME_TURBO_BOOST		SPEECH_SND(0xBE)
#define SPCH_YOU_NEED_A_TWIN_TURBO			SPEECH_SND(0xBF)

#define SPCH_GET_IT_IN_GEAR					SPEECH_SND(0xC0)
#define SPCH_BLEW_THAT_ENGINE				SPEECH_SND(0xC1)
#define SPCH_LETS_SEE_WHAT_YOU_GOT			SPEECH_SND(0xC2)
#define SPCH_JUST_TRY_AND_KEEP_UP			SPEECH_SND(0xC3)
#define SPCH_BLOW_YOUR_DOORS_OFF			SPEECH_SND(0xC4)
#define SPCH_SEE_YOU_ON_ROUTE_66			SPEECH_SND(0xC5)
#define SPCH_HEAD_FOR_THE_QUADRAJETS		SPEECH_SND(0xC6)

#define SPCH_YOU_AINT_THROUGH_YET			SPEECH_SND(0xC9)
#define SPCH_GET_BACK_HERE					SPEECH_SND(0xCA)
#define SPCH_DONT_MOVE						SPEECH_SND(0xCB)
#define SPCH_EAT_MY_DUST					SPEECH_SND(0xCC)
#define SPCH_BEST_TAKE_CARE_OF_BLUE_FLAME	SPEECH_SND(0xCE)
#define SPCH_BEST_TAKE_CARE_OF_GRAN_SPORT	SPEECH_SND(0xCF)
#define SPCH_BEST_TAKE_CARE_OF_FUELIE		SPEECH_SND(0xD0)
#define SPCH_BEST_TAKE_CARE_OF_67_STINGRAY	SPEECH_SND(0xD1)
#define SPCH_BEST_TAKE_CARE_OF_LT1			SPEECH_SND(0xD2)
#define SPCH_BEST_TAKE_CARE_OF_PACE_CAR		SPEECH_SND(0xD3)
#define SPCH_BEST_TAKE_CARE_OF_L83			SPEECH_SND(0xD4)
#define SPCH_BEST_TAKE_CARE_OF_CHALLENGE_CAR	SPEECH_SND(0xD5)
#define SPCH_BEST_TAKE_CARE_OF_ZR1			SPEECH_SND(0xD6)


#define SPCH_MATCH_JABBER					SPEECH_SND(0xEE)
#define SPCH_MATCH_MATCHED					SPEECH_SND(0xEF)
#define SPCH_MATCH_NO_MATCH					SPEECH_SND(0xF0)

#define SPCH_HI								SPEECH_SND(0xF5)
#define SPCH_IM_TAMMY						SPEECH_SND(0xF6)
#define SPCH_CATCH_ME_IF_YOU_CAN			SPEECH_SND(0xF7)
#define SPCH_NEED_DRIVING_LESSONS			SPEECH_SND(0xF8)
#define SPCH_NEED_TO_GET_OUT_MORE			SPEECH_SND(0xF9)
#define SPCH_NEED_TO_PLAY_MORE_PINBALL		SPEECH_SND(0xFA)
#define SPCH_OH_WELL_MAYBE_NEXT_TIME		SPEECH_SND(0xFB)
#define SPCH_MAYBE_NEXT_TIME				SPEECH_SND(0xFC)
#define SPCH_NEED_MORE_PRACTICE				SPEECH_SND(0xFD)
#define SPCH_YOU_ARE_GREASTED_LIGHTNING		SPEECH_SND(0xFE)
#define SPCH_YOU_ARE_SOME_KINDA_DRIVER		SPEECH_SND(0xFF)


//
// set 2
//
#define SET_02_SND(x)			((2UL << 8) + (x))

#define SPCH_YOU COULD WIN DAYTONA								SET_02_SND(0x00)  // Girl
#define SPCH_HAMISH_PLAYER_ONE									SET_02_SND(0x01) // Hamish
#define SPCH_HAMISH_PLAYER_TWO									SET_02_SND(0x02) // Hamish
#define SPCH_HAMISH_PLAYER_THREE								SET_02_SND(0x03) // Hamish
#define SPCH_HAMISH_PLAYER_FOUR									SET_02_SND(0x04) // Hamish
#define SPCH_HES_UP												SET_02_SND(0x05) // Hamish
#define SPCH_GET_THE_EXTRA_BALL_02								SET_02_SND(0x06) // Same as in set 1
#define SPCH_GET_THE_JACKPOT_02									SET_02_SND(0x07) // Same as in set 1
#define SPCH_EXTRA_BALL_02										SET_02_SND(0x08) // Same as in set 1
#define SPCH_JACKPOT_01											SET_02_SND(0x09)
#define SPCH_SUPER_JACKPOT										SET_02_SND(0x0A)
#define SPCH_GET_BACK_IN_THE_CAR_AND_DRIVE						SET_02_SND(0x0B) // Hamish

#define SPCH_SOME_BIG_BRAKES									SET_02_SND(0x19)
#define SPCH_A_FUELIE											SET_02_SND(0x1A)
#define SPCH_SOME_NITROUS										SET_02_SND(0x1B)
//...
#define SPCH_A_TWIN_TURBO										SET_02_SND(0x23)

#define SPCH_HES_OFF_TO_A_GOOD_START							SET_02_SND(0x27) // Hamish ...
#define SPCH_BIT_SLOW_OFF_STARTING_LINE							SET_02_SND(0x28)
#define SPCH_GOT_TO_MAKE_HIS_MOVE								SET_02_SND(0x2A)
#define SPCH_AYE_THAT_WAS_A_BONNY_PIECE_OF_DRIVING				SET_02_SND(0x2B)
#define SPCH_THAT_WAS_A_BEAUTIFUL_PIECE_OF_DRIVING				SET_02_SND(0x2C)
#define SPCH_HES_REALLY_TURNING_IT_ON							SET_02_SND(0x2D)
#define SPCH_COMING_TO_CORNERS_LIKE_ON_RAILS					SET_02_SND(0x2E)
#define SPCH_THAT_WAS_SPECTACULAR_DRIVING						SET_02_SND(0x2F)
#define SPCH_HAVE_YOU_EVER_SEEN_SUCH_A_DISPLAY					SET_02_SND(0x30)
#define SPCH_HE_COULD_SET_A_WORLD_SPEED_RECORD					SET_02_SND(0x31)
#define SPCH_HES_ON_A_WILD_RAGGED_PACE							SET_02_SND(0x32)
#define SPCH_HES_ABSLOUTELY_SHATTERS_THE_WORLD_SPEED_RECORD		SET_02_SND(0x33)
#define SPCH_DRIVING_OUT_OF_HIS_MIND							SET_02_SND(0x34)
#define SPCH_THIS_IS_WHAT_CHAMPIONSHIP_RACING_IS_ALL_ABOUT		SET_02_SND(0x35)
#define SPCH_HES_DONE_IT										SET_02_SND(0x36)
#define SPCH_HES_WON_THE_RACE									SET_02_SND(0x37) // ... Hamish

#define SPCH_PLAYER_ONE											SET_02_SND(0x3B)
#define SPCH_PLAYER_TWO											SET_02_SND(0x3C)
#define SPCH_PLAYER_THREE										SET_02_SND(0x3D)
#define SPCH_PLAYER_FOUR										SET_02_SND(0x3E)
#define SPCH_YOURE_UP											SET_02_SND(0x3F)
#define SPCH_ITS_TIME_TO_RACE									SET_02_SND(0x40)
#define SPCH_MEET_ME_AT_THE_STARTING_LINE						SET_02_SND(0x41)
#define SPCH_READY_WHEN_YOU_ARE									SET_02_SND(0x42)
#define SPCH_IF_YOU_WANNA_RACE_YOU_GOTTA_PIT					SET_02_SND(0x43)
#define SPCH_SHOOT_THE_PUT										SET_02_SND(0x44)
#define SPCH_SHOOT_THE_PIT_TO_START_A_CHALLENGE					SET_02_SND(0x45)
#define SPCH_JACKPOT_02											SET_02_SND(0x46)
#define SPCH_SUPER_JACKPOT_02									SET_02_SND(0x47)
#define SPCH_EXTRA_BALL											SET_02_SND(0x48)
#define SPCH_SPECIAL											SET_02_SND(0x49)
#define SPCH_TORQUE_JACKPOT										SET_02_SND(0x4A)
#define SPCH_HORSEPOWER_JACKPOT									SET_02_SND(0x4B)
#define SPCH_RAUCOUS											SET_02_SND(0x4C)
#define SPCH_HEAD_FOR_THE_LT5									SET_02_SND(0x4D)
#define SPCH_HEAD_FOR_THE_PITS									SET_02_SND(0x4E)
#define SPCH_HEAD_FOR_THE_TURBO									SET_02_SND(0x4F)
#define SPCH_HEAD_FOR_ROUTE_66									SET_02_SND(0x50)
#define SPCH_HEAD_FOR_THE_SPINNER								SET_02_SND(0x51)
#define SPCH_YOU_PULLED_A_FAST_ONE								SET_02_SND(0x52)
#define SPCH_YOU_WIN											SET_02_SND(0x53)
#define SPCH_GET_THE_SPECIAL									SET_02_SND(0x54)
#define SPCH_REPLAY												SET_02_SND(0x55)
#define SPCH_DRIVE_AGAIN										SET_02_SND(0x56)
#define SPCH_SHOOT_AGAIN										SET_02_SND(0x57)
#define SPCH_QUALIFY											SET_02_SND(0x61)

#define SPCH_FR_BONJOUR_02										SET_02_SND(0xBC)
#define SPCH_FR_JE_MAPPELLE_BRIGET_02							SET_02_SND(0xBD)
#define SPCH_FR_MONDEU											SET_02_SND(0xC7)
// ... more voices, repeated it seems from SET_01 above

#endif /* _MACH_SOUND_H */
