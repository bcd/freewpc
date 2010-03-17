#--------------------------------------------------------------------------
# System description for FreeWPC
# (C) Copyright 2006-2010 by Brian Dominy <brian@oddchange.com>
#
# See tools/genmachine for more information about the format of this file.
#--------------------------------------------------------------------------


#--------------------------------------------------------------------------
# This file contains definitions that are common to every WPC generation.
# You should *NOT* include this file from your machine description;
# rather include the file for the generation applicable to your machine.
# It will include this for you.
#--------------------------------------------------------------------------

[lamps]
11: First Lamp
88: Last Lamp

[switches]
D1: Left Coin, cabinet
D2: Center Coin, cabinet
D3: Right Coin, cabinet
D4: Fourth Coin, cabinet
D5: Escape, service, intest, cabinet, button
D6: Down, service, intest, cabinet, button
D7: Up, service, intest, cabinet, button
D8: Enter, service, intest, cabinet, button
22: Coin Door Closed, cabinet, edge, intest
24: Always Closed, virtual, c_decl(sw_unused)

[drives]
H1: High Power Sol. 1, notinstalled
H2: High Power Sol. 2, notinstalled
H3: High Power Sol. 3, notinstalled
H4: High Power Sol. 4, notinstalled
H5: High Power Sol. 5, notinstalled
H6: High Power Sol. 6, notinstalled
H7: High Power Sol. 7, notinstalled
H8: High Power Sol. 8, notinstalled

L1: Low Power Sol. 1, notinstalled
L2: Low Power Sol. 2, notinstalled
L3: Low Power Sol. 3, notinstalled
L4: Low Power Sol. 4, notinstalled
L5: Low Power Sol. 5, notinstalled
L6: Low Power Sol. 6, notinstalled
L7: Low Power Sol. 7, notinstalled
L8: Low Power Sol. 8, notinstalled

G1: Flasher 1, notinstalled
G2: Flasher 2, notinstalled
G3: Flasher 3, notinstalled
G4: Flasher 4, notinstalled
G5: Flasher 5, notinstalled
G6: Flasher 6, notinstalled
G7: Flasher 7, notinstalled
G8: Flasher 8, notinstalled

A1: General Sol. 1, notinstalled
A2: General Sol. 2, notinstalled
A3: General Sol. 3, notinstalled
A4: General Sol. 4, notinstalled

[gi]
0: G.I. String 1
1: G.I. String 2
2: G.I. String 3
3: G.I. String 4
4: G.I. String 5


[lamplists]
ALL: 11..88
Ball Save: MACHINE_BALL_SAVE_LAMP

[deffs]
NULL: c_decl(deff_exit), PRI_NULL
Amode: page(COMMON_PAGE), c_decl(system_amode_deff), runner, PRI_AMODE
Scores: runner, page(EFFECT_PAGE), PRI_SCORES
Scores Important: page(EFFECT_PAGE), PRI_SCORES_IMPORTANT, D_RESTARTABLE
Score Goal: page(EFFECT_PAGE), PRI_SCORE_GOAL, D_TIMEOUT
Credits: page(COMMON_PAGE), PRI_CREDITS
Tilt Warning: page(COMMON_PAGE), PRI_TILT_WARNING
Tilt: runner, page(COMMON_PAGE), PRI_TILT
Game Over: page(EFFECT_PAGE), PRI_GAME_OVER
Volume Change: page(EFFECT_PAGE), runner, PRI_VOLUME_CHANGE_DISPLAY, D_PAUSE
Slam Tilt: page(COMMON_PAGE), runner, PRI_SLAMTILT
Status Report: page(COMMON_PAGE), runner, PRI_STATUS
Nonfatal Error: page(EFFECT_PAGE), PRI_DEBUGGER
HSEntry: page(COMMON_PAGE), runner, PRI_HSENTRY
HSCredits: page(COMMON_PAGE), runner, PRI_HSENTRY
Match: page(COMMON_PAGE), runner, PRI_MATCH
Buyin Offer: page(COMMON_PAGE), PRI_MATCH
Locating Balls: page(EFFECT_PAGE), PRI_BALL_SEARCH
Player Tournament Ready: page(COMMON_PAGE), PRI_STATUS
System Reset: PRI_RESET, page(COMMON_PAGE)
# TODO : priority below?
Coin Door Buttons: page(COMMON_PAGE), PRI_JACKPOT
Plunge Ball: page(EFFECT_PAGE), PRI_SCORE_GOAL
Coin Door Power: page(COMMON_PAGE), PRI_JACKPOT
Ball Save: page(EFFECT_PAGE), c_decl(ball_save_deff), PRI_BALLSAVE
Enter Initials: page(COMMON_PAGE), PRI_HSENTRY
Enter PIN: page(COMMON_PAGE), PRI_PIN_ENTRY

[leffs]
NULL: PRI_NULL
Amode: runner, PRI_AMODE, c_decl(system_amode_leff), page(COMMON_PAGE)
Tilt Warning: PRI_TILT_WARNING, GI(ALL), c_decl(tilt_warning_leff), page(COMMON_PAGE)
Tilt: runner, PRI_TILT, LAMPS(ALL), GI(ALL), c_decl(no_lights_leff), page(COMMON_PAGE)
Ball Save: shared, PRI_LEFF3, LAMPS(BALL_SAVE), page(COMMON_PAGE)

[fonts]
mono5:
num5x7:
lucida9:
tinynum:
var5:
fixed6:
cu17:
fixed10:
term6:
times8:
bitmap8:

[timers]
Ignore Tilt:

[globalflags]
Ball At Plunger:
Coin Door Opened:

