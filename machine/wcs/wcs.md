#--------------------------------------------------------------------------
# WCS machine description for FreeWPC
# (C) Copyright 2006, 2009-2009 by Brian Dominy <brian@oddchange.com>
#
# See tools/genmachine for more information about the format of this file.
#--------------------------------------------------------------------------

Title: World Cup
DMD: Yes
Fliptronic: Yes
DCS: Yes
PIC: Yes
WPC95: No
Pinmame-Zip: wcs_l2.zip
Pinmame-ROM: wcup_lx2.bin
Lamp-Matrix-Width: 45
Lamp-Matrix-Height: 25

include platform/wpc/wpc.md

define MACHINE_NUMBER 531
define MACHINE_CUSTOM_AMODE
define MACHINE_GRAND_CHAMPION_INITIALS { 'W', 'C', 'S' }
define MACHINE_HIGH_SCORE_INITIALS { 'B', 'E', 'K' }, { 'K', 'M', 'E' }, { 'N', 'E', 'S' }, { 'R', 'J', 'S' }
define MACHINE_START_BALL_MUSIC   MUS_MAIN_PLUNGER
define MACHINE_BALL_IN_PLAY_MUSIC MUS_MAIN

[lamps]
11: Chicago P, purple, x(39), y(12)
12: Dallas U, purple, x(37), y(11)
13: Boston C, purple, x(35), y(10)
14: New York D, purple, x(33), y( 9)
15: Orlando L, purple, x(31), y( 8)
16: Washington R, purple, x(29), y( 7)
17: San Francisco O, purple, x(27), y( 6)
18: Detroit W, purple, x(25), y( 5)
21: 1 Goal, yellow, x(36), y(13)
22: 2 Goals, yellow, x(34), y(13)
23: 3 Goals, yellow, x(32), y(13)
24: 4 Goals, yellow, x(30), y(13)
25: Ball Speed, white, x(27), y(12)
26: Ball Strength, white, x(25), y(11)
27: Ball Stamina, white, x(23), y(12)
28: L. Ticket Half, purple, x(21), y(11)
31: Free Kick, yellow ,x(16), y(16)
32: TV Award, white , x(17), y(18)
33: Ultra Goalie, white , x(31), y(16)
34: Ultra Ramps, white , x(29), y(17)
35: Ball Spirit, white , x(26), y(14)
36: Ball Skill, white , x(24), y(14)
37: R. Ticket Half, purple, x(21), y(13)
38: Tackle, blue , x(14), y( 9)
41: Kickback Lower, green , x(38), y( 1)
42: Kickback Center, green , x(36), y( 1)
43: Kickback Upper, green , x(34), y( 1)
44: R. Ramp Build, white , x(19), y(20)
45: R. Ramp Lock, yellow , x(21), y(19)
46: Ultra Spinner, white , x(25), y(18)
47: Ultra Jets, white , x(27), y(18)
48: Striker, red , x(3), y(15)
51: Goal Jackpot, red , x(8), y(12)
52: Extra Ball, red , x(7), y(14)
53: Goal, yellow , x(6), y(11)
54: Striker Build, white , x(5), y(15)
55: Light Magna Goalie , x(29), y(21)
56: R. Flipper Lane , x(35), y(22)
57: Shoot Again, red, extra-ball , x(43), y(17)
58: Special, red , x(38), y(24)
61: L. Ramp Build, white , x(19), y( 6)
62: L. Loop Build, white , x(19), y( 2)
63: Travel, yellow , x(21), y( 3)
64: World Cup Final, red , x(23), y( 4)
65: L. Ramp Lock, yellow , x(21), y( 7)
66: Top Lane Left, purple , x(1), y(17)
67: Top Lane Right, purple , x(1), y(19)
68: Skill Lower, yellow , x(30), y(25)
71: Light Jackpots, red , x(16), y( 4)
72: Final Draw, white , x(19), y(17)
73: Magna Goalie, orange , x(43), y( 7)
74: Left Inlane , x(35), y( 3)
75: Light Kickback, green, x(30), y( 4)
76: L. Ramp Ticket, yellow, x(16), y( 6)
77: R. Ramp Ticket, yellow, x(16), y(20)
78: Ultra Ramp Collect, yellow, x(16), y( 5)
81: Rollover 1, red, x(10), y(13)
82: Rollover 2, red, x(13), y(12)
83: Rollover 3, red, x(16), y(13)
84: Rollover 4, red, x(19), y(14)
85: Skill Top, yellow, x(26), y(25)
86: Skill Middle, yellow, x(28), y(25)
87: Buy-In Button, green, buyin, cabinet, x(45), y(21)
88: Start Button, yellow, start, cabinet, x(45), y( 3)

[switches]
11: Unused
12: Magnet Button, button, noscore, cabinet
13: Start Button, start-button, cabinet, intest
14: Tilt, tilt, ingame, noplay
15: Left Flipper Lane, ingame
16: Striker 3, ingame, debounce(TIME_50MS)
17: Right Flipper Lane, ingame
18: Right Outlane, ingame
21: Slam Tilt, slam-tilt
23: Buyin Button, buyin-button, cabinet
25: Free Kick Target, ingame, debounce(TIME_50MS)
26: Kickback Upper, ingame
27: Spinner, ingame
28: Light Kickback, ingame
31: Trough 1, trough, noscore, opto
32: Trough 2, trough, noscore, opto
33: Trough 3, trough, noscore, opto
34: Trough 4, trough, noscore, opto
35: Trough 5, trough, noscore, opto
36: Trough Stack, noscore, opto
37: Light Magna Goalie, ingame
38: Shooter, edge, noscore, shooter
41: Goal Trough, opto, ingame
42: Goal Popper, opto, ingame
43: Goalie Left, noplay, intest, opto
44: Goalie Right, noplay, intest, opto
45: TV Popper, opto, ingame
46: Unused
47: Travel Lane, ingame
48: Goalie Target, ingame
51: Skill Shot Front, opto, ingame
52: Skill Shot Center, opto, ingame
53: Skill Shot Rear, opto, ingame
54: Right Eject, ingame
55: Upper Eject, ingame
56: Left Eject, ingame
61: Rollover 1, ingame
62: Rollover 2, ingame
63: Rollover 3, ingame
64: Rollover 4, ingame
65: Tackle, ingame
66: Striker 1, ingame, debounce(TIME_50MS)
67: Striker 2, ingame, debounce(TIME_50MS)
71: L. Ramp Diverted, ingame
72: L. Ramp Enter, ingame
74: L. Ramp Exit, ingame
75: R. Ramp Enter, ingame
76: Lock Low, ingame
77: Lock High, ingame
78: R. Ramp Exit, ingame
81: Left Jet, ingame, noplay
82: Upper Jet, ingame, noplay
83: Lower Jet, ingame, noplay
84: Left Slingshot, ingame, noplay
85: Right Slingshot, ingame, noplay
86: Kickback, ingame
87: Upper Left Lane, ingame
88: Upper Right Lane, ingame

[drives]
H1: Goal Popper, duty(SOL_DUTY_75), time(TIME_100MS)
H2: TV Popper, duty(SOL_DUTY_75), time(TIME_100MS)
H3: Kickback, duty(SOL_DUTY_50), time(TIME_66MS)
H4: Lock Release
H5: Upper Eject
H6: Trough Release, ballserve, duty(SOL_DUTY_75), time(TIME_100MS)
H7: Knocker, knocker
H8: Ramp Divertor

L1: Left Jet, duty(SOL_DUTY_75), time(TIME_33MS)
L2: Upper Jet, duty(SOL_DUTY_75), time(TIME_33MS)
L3: Lower Jet, duty(SOL_DUTY_75), time(TIME_33MS)
L4: Left Sling, duty(SOL_DUTY_100), time(TIME_33MS)
L5: Right Sling, duty(SOL_DUTY_100), time(TIME_33MS)
L6: Right Eject
L7: Left Eject
L8: Divertor Hold

G1: Goal Cage Top, flash
G2: Goal, flash
G3: Skill Shot, flash
G4: Jet Bumpers, flash
G5: Goalie Drive, motor
G6: Soccer Ball, flash
G7: Ball Forward, motor
G8: Ball Reverse, motor

A1: L. Ramp Enter, flash
A2: Lock Area, flash
A3: Flipper Lanes, flash
A4: Ramp Rear, flash

F5: Magna Goalie, duty(SOL_DUTY_50), time(TIME_66MS)
F6: Loop Gate, duty(SOL_DUTY_75), time(TIME_200MS)
F7: Lock Magnet, duty(SOL_DUTY_12), time(TIME_100MS)

[gi]
0: Playfield Left
1: Playfield Right
2: Insert Background
3: Insert Title
4: Playfield Top

[tests]
wcs_goalie:

#############################################################

[lamplists]
Goal Counts: 1 Goal, 2 Goals, 3 Goals, 4 Goals
Ball Panels: Ball Stamina, Ball Skill, Ball Spirit, Ball Speed, Ball Strength
Build Shots: L. Loop Build, L. Ramp Build, Striker Build, R. Ramp Build
Kickbacks: Kickback Lower, Kickback Center, Kickback Upper
Ultra Modes: Ultra Spinner, Ultra Jets, Ultra Ramps, Ultra Goalie
Rollovers: Rollover 1, Rollover 2, Rollover 3, Rollover 4
Ticket Halves: L. Ticket Half, R. Ticket Half
Cities: Chicago P, Dallas U, Boston C, New York D, Orlando L, Washington R, San Francisco O, Detroit W
Skill Shots: Skill Top, Skill Middle, Skill Lower
Rollovers: Rollover 1, Rollover 2, Rollover 3, Rollover 4
Top Lanes: Top Lane Left, Top Lane Right
Locks: L. Ramp Lock, R. Ramp Lock
Ramp Tickets: L. Ramp Ticket, R. Ramp Ticket
Red Lamps: set, COLOR:red
White Lamps: set, COLOR:white
Orange Lamps: set, COLOR:orange
Yellow Lamps: set, COLOR:yellow
Green Lamps: set, COLOR:green
Blue Lamps: set, COLOR:blue
Purple Lamps: set, COLOR:purple
Circle Out: PF:lamp_sort_circle_out
Build Up: PF:lamp_sort_bottom_to_top

[containers]
Trough: trough, Trough Release, Trough 5, Trough 4, Trough 3, Trough 2, Trough 1, init_max_count(5)
Goal Popper: Goal Popper, Goal Popper
TV Popper: TV Popper, TV Popper
Left Eject: Left Eject, Left Eject
Right Eject: Right Eject, Right Eject
Upper Eject: Upper Eject, Upper Eject
Lock: Lock Release, Lock High, Lock Low

[targetbanks]

[shots]

#############################################################

[adjustments]
Ball Saver: yes_no, YES
Evil Goalie: yes_no, NO
Max Lock Count: integer, 2
Reverse Spin: yes_no, NO
Disable Ball: yes_no, NO
Disable Goalie: yes_no, NO

[audits]

[system_sounds]

[system_music]

[scores]
730:
5K:
10K:
25K:
30K:
50K:
100K:
200K:
250K:
300K:
400K:
500K:
750K:
1M:
1500K:
2M:
2500K:
3M:

[highscores]

[flags]
5 Man MB Running:
Divert Ramp:
Lock Magnet Enable:

Goal Lit:

Multiball Lit:
Multiball Running:
Left Eject Lock:
Right Eject Lock:
Jackpot Lit:
Super Jackpot Lit:

TV Lit:
Mode Lit:
Ultra Mania Lit:

[globalflags]
Goalie Moving:
Goalie Active:

[deffs]
PGJ Logo: page(MACHINE_PAGE), c_decl(amode_pgj_logo), PRI_AMODE
Free Kick: page(MACHINE_PAGE), PRI_GAME_QUICK1, D_SCORE+D_RESTARTABLE

#NewMatch Running: page(MACHINE_PAGE), runner, PRI_GAME_LOW3
#NewMatch Win: page(MACHINE_PAGE), PRI_GAME_QUICK6
#MyMode Starting: page(MACHINE_PAGE), PRI_GAME_QUICK6
#MyMode Running: page(MACHINE_PAGE), PRI_GAME_MODE2
#MyMode Ending: page(MACHINE_PAGE), PRI_GAME_QUICK6
#Match Running: page(MACHINE_PAGE), runner, PRI_GAME_LOW2
#Match Started: page(MACHINE_PAGE), PRI_GAME_LOW1
#Yards Awarded: page(MACHINE_PAGE), PRI_GAME_MODE1
#Goal Lit: page(MACHINE_PAGE), PRI_GAME_MODE1
#Goal Unlit: page(MACHINE_PAGE), PRI_GAME_MODE1
#Goal Scored: page(MACHINE_PAGE), PRI_GAME_MODE2
#Match Won: page(MACHINE_PAGE), PRI_GAME_MODE4

Combo: page(MACHINE_PAGE), PRI_GAME_QUICK3, D_SCORE+D_RESTARTABLE

Jet: page(MACHINE_PAGE), PRI_GAME_QUICK1, D_SCORE+D_RESTARTABLE

Lightning Test: page(MACHINE_PAGE), PRI_GAME_QUICK1

Goal Lit: page(MACHINE_PAGE), PRI_GAME_QUICK2
Goal Scored: page(MACHINE_PAGE), PRI_GAME_MODE4, D_SCORE

Ball Locked: page(MACHINE_PAGE), PRI_GAME_QUICK4
Multiball Lit: page(MACHINE_PAGE), PRI_GAME_QUICK5
Multiball Start: page(MACHINE_PAGE), PRI_GAME_QUICK6
Multiball Running: runner, page(MACHINE_PAGE), PRI_GAME_MODE5
Multiball Jackpot: page(MACHINE_PAGE), D_SCORE, PRI_GAME_QUICK7
Multiball Super Jackpot: page(MACHINE_PAGE), D_SCORE, PRI_GAME_QUICK8
Multiball Spinner: page(MACHINE_PAGE), PRI_GAME_QUICK6

Mode Target Status: page(MACHINE_PAGE), PRI_GAME_QUICK2
Mode Lit: page(MACHINE_PAGE), PRI_GAME_QUICK4
Mode Started: page(MACHINE_PAGE), PRI_GAME_QUICK6

[leffs]
Amode: runner, PRI_LEFF1, LAMPS(ALL), GI(ALL), page(MACHINE_PAGE)
# Goal Scored: PRI_LEFF1, page(MACHINE_PAGE)
Color Cycle: runner, PRI_LEFF3, LAMPS(ALL), page(MACHINE_PAGE)
Build Up: runner, PRI_LEFF3, LAMPS(ALL), page(MACHINE_PAGE)

[fonts]
pcsenior:
misctype:
lithograph:

[timers]
Goal Trough Made:
Ignore R. Ramp Exit:
Ramp Gate Debounce:
Kickback Made:
Lock Kicking:

[templates]
Left Sling: driver(spsol),
	sw=SW_LEFT_SLINGSHOT,
	sol=SOL_LEFT_SLING,
	ontime=3, offtime=16
Right Sling: driver(spsol),
	sw=SW_RIGHT_SLINGSHOT,
	sol=SOL_RIGHT_SLING,
	ontime=3, offtime=16

Left Jet: driver(spsol),
	sw=SW_LEFT_JET, sol=SOL_LEFT_JET, ontime=3, offtime=16
Upper Jet: driver(spsol),
	sw=SW_UPPER_JET, sol=SOL_UPPER_JET, ontime=3, offtime=16
Lower Jet: driver(spsol),
	sw=SW_LOWER_JET, sol=SOL_LOWER_JET, ontime=3, offtime=16

Spinner: driver(spinner),
	sw_event=sw_spinner, sw_number=SW_SPINNER

Gate: driver(duty),
	sol=SOL_LOOP_GATE,
	ontime=TIME_300MS, duty_ontime=TIME_33MS, duty_offtime=TIME_16MS, timeout=60

Kickback Driver: driver(spsol),
	sw=SW_KICKBACK, sol=SOL_KICKBACK, ontime=5, offtime=30

Spinning Ball: driver(duty),
	sol=SOL_BALL_FORWARD,
	ontime=0, duty_ontime=TIME_33MS, duty_offtime=TIME_66MS, timeout=0

Goalie: driver(duty),
	sol=SOL_GOALIE_DRIVE,
	ontime=0, duty_ontime=TIME_16MS, duty_offtime=TIME_33MS, timeout=0

