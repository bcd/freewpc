#--------------------------------------------------------------------------
# AFM machine description for FreeWPC
# (C) Copyright 2006, 2009, 2010 by Brian Dominy <brian@oddchange.com>
#
# See tools/genmachine for more information about the format of this file.
#--------------------------------------------------------------------------

Title: Attack From Mars
include platform/wpc/wpc95.md

Pinmame-Zip: afm_113b.zip
Pinmame-ROM: afm_113b.bin

define MACHINE_NUMBER 541
define MACHINE_CUSTOM_AMODE
define MACHINE_BALL_SAVE_LAMP LM_RETURN_TO_BATTLE

# For Challenge rules
define MACHINE_BALL_SAVE_TIME 0

[lamps]
11: RTU Super Jets
12: RTU Super Jackpot
13: RTU Martian MB
14: RTU Total Ann.
15: Return to Battle
16: RTU Conquer Mars
17: RTU 5 Way
18: Saucer
21: L. Ramp 1
22: L. Ramp 2
23: L. Ramp 3
24: L. Ramp Jackpot
25: L. Ramp Arrow
26: Lock 2
27: Lock 3
28: Lock Jackpot
31: R. Ramp 1
32: R. Ramp 2
33: R. Ramp 3
34: R. Ramp Jackpot
35: R. Ramp Arrow
36: Martian Attack
37: Rule the Universe
38: Stroke of Luck
41: R. Loop Arrow
42: Lock Arrow
43: L. Top Lane
44: R. Top Lane
45: Motor Bank 1
46: Motor Bank 2
47: Motor Bank 3
48: Martian 4
51: Attack Mars
52: Country 5
53: Country 4
54: Light Lock
55: Lock 1
56: Country 3
57: Country 2
58: Country 1
61: Martian 7
62: Martian 6
63: R. Loop 1
64: R. Loop 2
65: R. Loop 3
66: R. Loop Jackpot
67: Extra Ball
68: Martian 5
71: L. Loop 1
72: L. Loop 2
73: L. Loop 3
74: L. Loop Jackpot
75: L. Loop Arrow
76: Martian 1
77: Martian 2
78: Martian 3
81: Shoot Again, shoot-again
82: Left Outlane
83: Left Inlane
84: Right Inlane
85: Right Outlane
86: Launch Button
88: Start Button

[switches]
11: Launch Button, launch-button, cabinet, noplay
13: Start Button, yellow, start-button, cabinet, intest
14: Tilt, tilt, noplay, cabinet
16: Left Outlane
17: Right Inlane
18: Shooter, edge, shooter, noplay, debounce(TIME_200MS)
21: Slam Tilt, slam-tilt, ingame, cabinet
26: Left Inlane
27: Right Outlane
31: Trough Eject, opto, noscore, noplay
32: Trough 1, trough, opto, noscore, noplay
33: Trough 2, trough, opto, noscore, noplay
34: Trough 3, trough, opto, noscore, noplay
35: Trough 4, trough, opto, noscore, noplay
36: Left Popper, opto
37: Right Popper, opto
38: Left Top Lane
41: Martian 6, lamp(LM_MARTIAN_6)
42: Martian 7, lamp(LM_MARTIAN_7)
43: Martian 4, lamp(LM_MARTIAN_4)
44: Martian 5, lamp(LM_MARTIAN_5)
45: Motor Bank 1, ingame
46: Motor Bank 2, ingame
47: Motor Bank 3, ingame
48: Right Top Lane
51: Left Slingshot, noplay
52: Right Slingshot, noplay
53: Left Jet, noplay
54: Bottom Jet, noplay
55: Right Jet, noplay
56: Martian 1, lamp(LM_MARTIAN_1)
57: Martian 2, lamp(LM_MARTIAN_2)
58: Martian 3, lamp(LM_MARTIAN_3)
61: Left Ramp Enter
62: Center Ramp Enter
63: Right Ramp Enter
64: Left Ramp Exit
65: Right Ramp Exit
66: Motor Bank Down, noplay, intest, service
67: Motor Bank Up, noplay, intest, service
71: Right Loop High, noplay
72: Right Loop Low, noplay
73: Left Loop High
74: Left Loop Low
75: L. Saucer Tgt.
76: R. Saucer Tgt.
77: Drop Target
78: Center Trough

[drives]
H1: Auto Launch, launch
H2: Trough Eject, ballserve
H3: Left Popper
H4: Right Popper
H5: L.L. Martian, nosearch
H6: U.L. Martian, nosearch
H7: Knocker, knocker
H8: U.R. Martian, nosearch

L1: Left Slingshot
L2: Right Slingshot
L3: Left Jet
L4: Bottom Jet
L5: Right Jet
L6: L.R. Martian, nosearch
L7: Saucer Shake, nosearch
L8: Drop Target

G1: Saucer Flash 5, flash
G2: Saucer Flash 4, flash
G3: Saucer Flash 6, flash
G4: R. Martian Flash, flash
G5: Saucer Arrow, flash
G7: Saucer Flash, flash

A1: Saucer Flash 2, flash
A2: Saucer Flash 3, flash
A3: Saucer Flash 1, flash
A4: L. Martian Flash, flash
A5: L.E.D. Clock, nosearch
A6: L.E.D. Data, nosearch
A7: Strobe Light, flash
A8: Coin Meter, nosearch

F5: Right Gate
F6: Left Gate
F7: Divertor Power, nosearch
F8: Divertor Hold, nosearch

[gi]
0: Lower Playfield
1: Middle Playfield
2: Upper Playfield

[tests]

#############################################################

[lamplists]
Countries: Country 1, Country 2, Country 3, Country 4, Country 5
Locks: Lock 1, Lock 2, Lock 3
L. Loop Count: L. Loop 1, L. Loop 2, L. Loop 3
L. Ramp Count: L. Ramp 1, L. Ramp 2, L. Ramp 3
R. Ramp Count: R. Ramp 1, R. Ramp 2, R. Ramp 3
R. Loop Count: R. Loop 1, R. Loop 2, R. Loop 3
Arrows: L. Loop Arrow, L. Ramp Arrow, Lock Arrow, R. Ramp Arrow, R. Loop Arrow
Jackpots: L. Loop Jackpot, L. Ramp Jackpot, Lock Jackpot, R. Ramp Jackpot, R. Loop Jackpot
Martians: Martian 1, Martian 2, Martian 3, Martian 4, Martian 5, Martian 6, Martian 7
Bottom Lanes: Left Outlane, Left Inlane, Right Inlane, Right Outlane
RTU Progress: RTU Super Jets, RTU Super Jackpot, RTU Total Ann., RTU Martian MB, RTU Conquer Mars, RTU 5 Way
Motor Bank: Motor Bank 1, Motor Bank 2, Motor Bank 3
Countries and Attack: Countries, Attack Mars
Lock Lane: Light Lock, Locks, Lock Jackpot, Lock Arrow
Top Lanes: L. Top Lane, R. Top Lane
Right Hole: Rule the Universe, Martian Attack, Stroke of Luck, Extra Ball
L. Loop All: L. Loop Count, L. Loop Jackpot, L. Loop Arrow
L. Ramp All: L. Ramp Count, L. Ramp Jackpot, L. Ramp Arrow
R. Ramp All: R. Ramp Count, R. Ramp Jackpot, R. Ramp Arrow
R. Loop All: R. Loop Count, R. Loop Jackpot, R. Loop Arrow

[containers]
Trough: trough, Trough Eject, Trough 1, Trough 2, Trough 3, Trough 4, init_max_count(4)
Left Hole: Left Popper, Left Popper
Right Hole: Right Popper, Right Popper

#############################################################

[adjustments]

[audits]

[system_sounds]

[system_music]

[scores]
1:
10:
100:

[highscores]

[flags]

[globalflags]
Divertor Open:
Bank Up:
Drop Up:

[deffs]
Chal Running: PRI_GAME_MODE1, page(MACHINE_PAGE)
Chal Sudden Death: PRI_GAME_MODE1, page(MACHINE_PAGE)
Chal Award: PRI_GAME_QUICK1, page(MACHINE_PAGE), D_SCORE+D_RESTARTABLE
Chal Phase Finished: PRI_GAME_QUICK1, page(MACHINE_PAGE), D_QUEUED+D_PAUSE, c_decl(chal_all_shots_deff)
Chal Ball Drain: PRI_GAME_QUICK2, page(MACHINE_PAGE)
Chal Timeout Endgame: PRI_GAME_QUICK1, page(MACHINE_PAGE)
Martian Kill: PRI_GAME_QUICK2, page(MACHINE_PAGE), D_RESTARTABLE

BonusMB Running: PRI_GAME_MODE3, page(MACHINE_PAGE)
BonusMB Ending: PRI_GAME_MODE3, page(MACHINE_PAGE), D_QUEUED

[leffs]
Amode: runner, PRI_LEFF1, LAMPS(ALL), GI(ALL), page(MACHINE_PAGE)
Flashfest: PRI_LEFF2, page(MACHINE_PAGE)
Flash Random: PRI_LEFF3, page(MACHINE_PAGE)
GI Strobe: PRI_LEFF4, GI(ALL), page(MACHINE_PAGE)
GI Flash: PRI_LEFF4, GI(ALL), page(MACHINE_PAGE)

[fonts]

[timers]
Center Ramp Entered:
Right Ramp Entered:
Center Trough Entered:
Ball In Jets:

[templates]
Left Sling: driver(spsol),
	sw=SW_LEFT_SLINGSHOT, sol=SOL_LEFT_SLINGSHOT,
	ontime=3, offtime=16

Right Sling: driver(spsol),
	sw=SW_RIGHT_SLINGSHOT, sol=SOL_RIGHT_SLINGSHOT,
	ontime=3, offtime=16

Left Jet: driver(spsol),
	sw=SW_LEFT_JET, sol=SOL_LEFT_JET,
	ontime=3, offtime=16

Right Jet: driver(spsol),
	sw=SW_RIGHT_JET, sol=SOL_RIGHT_JET,
	ontime=3, offtime=16

Bottom Jet: driver(spsol),
	sw=SW_BOTTOM_JET, sol=SOL_BOTTOM_JET,
	ontime=3, offtime=16

Left Gate: driver(duty),
	sol=SOL_LEFT_GATE,
	ontime=TIME_300MS, duty_ontime=TIME_33MS, duty_offtime=TIME_16MS, timeout=60

Right Gate: driver(duty),
	sol=SOL_RIGHT_GATE,
	ontime=TIME_300MS, duty_ontime=TIME_33MS, duty_offtime=TIME_16MS, timeout=60

