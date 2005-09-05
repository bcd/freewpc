/**************************************************************************
 *
 * FreeWPC - an open source operating system for the Williams Pinball
 *           Controller (WPC) platform
 *
 **************************************************************************/

#ifndef _MACH_SWITCH_H
#define _MACH_SWITCH_H

#define SW_RIGHT_INLANE			MAKE_SW(1,1)
#define SW_LEFT_INLANE			MAKE_SW(1,2)
#define SW_START_BUTTON			MAKE_SW(1,3)
#define SW_TILT					MAKE_SW(1,4)
#define SW_RIGHT_TROUGH			MAKE_SW(1,5)
#define SW_CENTER_TROUGH		MAKE_SW(1,6)
#define SW_LEFT_TROUGH			MAKE_SW(1,7)
#define SW_OUTHOLE				MAKE_SW(1,8)

#define SW_SLAM_TILT				MAKE_SW(2,1)
#define SW_COIN_DOOR_CLOSED	MAKE_SW(2,2)
#define SW_BUYIN_BUTTON			MAKE_SW(2,3)
#define SW_FAR_LEFT_TROUGH		MAKE_SW(2,5)
#define SW_TROUGH_PROX			MAKE_SW(2,6)
#define SW_SHOOTER				MAKE_SW(2,7)
#define SW_ROCKET					MAKE_SW(2,8)

#define SW_SLOT					MAKE_SW(5,8)

#define SW_AUTOFIRE1				MAKE_SW(7,1)
#define SW_AUTOFIRE2				MAKE_SW(7,2)

#define SW_LOCK_CENTER			MAKE_SW(8,4)
#define SW_LOCK_UPPER			MAKE_SW(8,5)
#define SW_LOCK_LOWER			MAKE_SW(8,8)

#endif /* _MACH_SWITCH_H */
