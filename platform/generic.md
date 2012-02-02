#--------------------------------------------------------------------------
# FreeWPC generic machine description
# (C) Copyright 2006-2012 by Brian Dominy <brian@oddchange.com>
#
# See tools/genmachine for more information about the format of this file.
#
# This file contains generic definitions that are common to all platforms.
# That means only things which are not hardware specific.
#--------------------------------------------------------------------------

[deffs]

NULL: c_decl(deff_exit), PRI_NULL
Amode: page(COMMON_PAGE), c_decl(system_amode_deff), runner, PRI_AMODE
Scores: runner, page(EFFECT_PAGE), PRI_SCORES
Scores Important: page(EFFECT_PAGE), PRI_SCORES_IMPORTANT, D_RESTARTABLE
Score Goal: page(EFFECT_PAGE), PRI_SCORE_GOAL, D_TIMEOUT
Credits: page(EFFECT_PAGE), PRI_CREDITS
Tilt Warning: page(EFFECT_PAGE), PRI_TILT_WARNING
Tilt: runner, page(EFFECT_PAGE), PRI_TILT
Game Over: page(EFFECT_PAGE), PRI_GAME_OVER
Volume Change: page(EFFECT_PAGE), runner, PRI_VOLUME_CHANGE_DISPLAY, D_PAUSE
Slam Tilt: page(EFFECT_PAGE), runner, PRI_SLAMTILT
Status Report: page(COMMON_PAGE), runner, PRI_STATUS
Nonfatal Error: page(EFFECT_PAGE), PRI_DEBUGGER
HSEntry: page(COMMON_PAGE), runner, PRI_HSENTRY
HSCredits: page(COMMON_PAGE), runner, PRI_HSENTRY
Match: page(COMMON_PAGE), runner, PRI_MATCH
Buyin Offer: page(EFFECT_PAGE), PRI_MATCH
Locating Balls: page(EFFECT_PAGE), PRI_BALL_SEARCH
Player Tournament Ready: page(EFFECT_PAGE), PRI_STATUS
System Reset: PRI_RESET, page(COMMON_PAGE)
# TODO : priority below?
Coin Door Buttons: page(EFFECT_PAGE), PRI_JACKPOT
Plunge Ball: page(EFFECT_PAGE), PRI_SCORE_GOAL
Coin Door Power: page(EFFECT_PAGE), PRI_JACKPOT
Ball Save: page(EFFECT_PAGE), c_decl(ball_save_deff), PRI_BALLSAVE
Enter Initials: page(COMMON_PAGE), PRI_HSENTRY
Enter PIN: page(COMMON_PAGE), PRI_PIN_ENTRY

[leffs]
NULL: PRI_NULL
Amode: runner, PRI_AMODE, c_decl(system_amode_leff), page(COMMON_PAGE)
Tilt Warning: PRI_TILT_WARNING, GI(ALL), c_decl(tilt_warning_leff), page(COMMON_PAGE)
Tilt: runner, PRI_TILT, LAMPS(ALL), GI(ALL), c_decl(no_lights_leff), page(COMMON_PAGE)
Ball Save: shared, PRI_LEFF3, LAMPS(BALL_SAVE), page(COMMON_PAGE)

[timers]
Ignore Tilt:

[globalflags]
Ball At Plunger:
Coin Door Opened:

