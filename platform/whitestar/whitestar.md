#--------------------------------------------------------------------------
# System description for Whitestar
# (C) Copyright 2007 by Brian Dominy <brian@oddchange.com>
#
# See tools/genmachine for more information about the format of this file.
#--------------------------------------------------------------------------

[lamps]
11: First Lamp
88: Last Lamp

[switches]
D1: Left Flipper, button
D2: Left Flipper EOS
D3: Right Flipper, button
D4: Right Flipper EOS
D5: Upper Flipper, button
D6: Red Button, button
D7: Green Button, button
D8: Black Button, button

11: Left Post Save, button
12: 4th Coin, cabinet
13: 6th Coin, cabinet
14: Right Coin, cabinet
15: Center Coin, cabinet
16: Left Coin, cabinet
17: 5th Coin, cabinet
18: Right Post Save, button

[drives]
H1: High Power Sol. 1
H2: High Power Sol. 2
H3: High Power Sol. 3
H4: High Power Sol. 4
H5: High Power Sol. 5
H6: High Power Sol. 6
H7: High Power Sol. 7
H8: High Power Sol. 8

L1: Low Power Sol. 1
L2: Low Power Sol. 2
L3: Low Power Sol. 3
L4: Low Power Sol. 4
L5: Low Power Sol. 5
L6: Low Power Sol. 6
L7: Low Power Sol. 7
L8: Low Power Sol. 8

G1: General Sol. 1
G2: General Sol. 2
G3: General Sol. 3
G4: General Sol. 4
G5: General Sol. 5
G6: General Sol. 6
G7: General Sol. 7
G8: General Sol. 8

A1: Aux. Sol. 1
A2: Aux. Sol. 2
A3: Aux. Sol. 3
A4: Aux. Sol. 4
A5: Aux. Sol. 5
A6: Aux. Sol. 6
A7: Aux. Sol. 7
A8: Aux. Sol. 8

[lamplists]
ALL: 11..88

[deffs]
NULL: c_decl(deff_exit), 0
Amode: page(EFFECT_PAGE), c_decl(default_amode_deff), runner, PRI_AMODE
Inspector: page(COMMON_PAGE), runner, PRI_DEBUGGER
Scores: runner, PRI_SCORES
Scores Important: PRI_SCORES_IMPORTANT
Score Goal: page(EFFECT_PAGE), runner, PRI_SCORE_GOAL
Credits: page(COMMON_PAGE), PRI_CREDITS
Tilt Warning: page(COMMON_PAGE), PRI_TILT_WARNING
Tilt: runner, page(COMMON_PAGE), PRI_TILT
Game Over: page(EFFECT_PAGE), PRI_GAME_OVER
Volume Change: page(EFFECT_PAGE), runner, PRI_VOLUME_CHANGE_DISPLAY
Slam Tilt: page(COMMON_PAGE), runner, PRI_SLAMTILT
Status Report: page(COMMON_PAGE), runner, PRI_STATUS
Nonfatal Error: page(EFFECT_PAGE), PRI_DEBUGGER
HSEntry: page(COMMON_PAGE), runner, PRI_HSENTRY
Match: page(COMMON_PAGE), runner, PRI_MATCH
Buyin Offer: page(COMMON_PAGE), PRI_MATCH
Onecoin Buyin: page(COMMON_PAGE), PRI_MATCH
Locating Balls: page(EFFECT_PAGE), PRI_BALL_SEARCH
Player Tournament Ready: page(COMMON_PAGE), PRI_STATUS
System Reset: PRI_RESET, page(COMMON_PAGE)

[leffs]
NULL: 0
Tilt Warning: runner, PRI_TILT_WARNING, LAMPS(ALL), c_decl(no_lights_leff)
Tilt: runner, PRI_TILT, LAMPS(ALL), GI(ALL), c_decl(no_lights_leff)

[fonts]
mono5:
lucida9:
tinynum:
var5:
fixed6:
cu17:
fixed10:
term6:
times8:

[timers]
Ignore Tilt:

