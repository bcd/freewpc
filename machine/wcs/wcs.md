#--------------------------------------------------------------------------
# WCS machine description for FreeWPC
# (C) Copyright 2006-2008 by Brian Dominy <brian@oddchange.com>
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

[lamps]
11: noname, x(39), y(12)
12: noname, x(37), y(11)
13: noname, x(35), y(10)
14: noname, x(33), y( 9)
15: noname, x(31), y( 8)
16: noname, x(29), y( 7)
17: noname, x(27), y( 6)
18: noname, x(25), y( 5)
21: noname, x(36), y(13)
22: noname, x(34), y(13)
23: noname, x(32), y(13)
24: noname, x(30), y(13)
25: noname, x(27), y(12)
26: noname, x(25), y(11)
27: noname, x(23), y(12)
28: noname, x(21), y(11)
31: noname, x(16), y(16)
32: noname, x(17), y(18)
33: noname, x(31), y(16)
34: noname, x(29), y(17)
35: noname, x(26), y(14)
36: noname, x(24), y(14)
37: noname, x(21), y(13)
38: noname, x(14), y( 9)
41: noname, x(38), y( 1)
42: noname, x(36), y( 1)
43: noname, x(34), y( 1)
44: noname, x(19), y(20)
45: noname, x(21), y(19)
46: noname, x(25), y(18)
47: noname, x(27), y(18)
48: noname, x(3), y(15)
51: noname, x(8), y(12)
52: noname, x(7), y(14)
53: noname, x(6), y(11)
54: noname, x(5), y(15)
55: noname, x(29), y(21)
56: noname, x(35), y(22)
57: noname, x(43), y(17)
58: noname, x(38), y(24)
61: noname, x(19), y( 6)
62: noname, x(19), y( 2)
63: noname, x(21), y( 3)
64: noname, x(23), y( 4)
65: noname, x(21), y( 7)
66: noname, x(1), y(17)
67: noname, x(1), y(19)
68: noname, x(30), y(25)
71: noname, x(16), y( 4)
72: noname, x(19), y(17)
73: noname, x(43), y( 7)
74: noname, x(35), y( 3)
75: noname, x(30), y( 4)
76: noname, x(16), y( 6)
77: noname, x(16), y(20)
78: noname, x(16), y( 5)
81: noname, x(10), y(13)
82: noname, x(13), y(12)
83: noname, x(16), y(13)
84: noname, x(19), y(14)
85: noname, x(26), y(25)
86: noname, x(28), y(25)
87: noname, x(45), y(21)
88: noname, x(45), y( 3)

[lamps]
11: Chicago P, purple
12: Dallas U, purple
13: Boston C, purple
14: New York D, purple
15: Orlando L, purple
16: Washington R, purple
17: San Francisco O, purple
18: Detroit W, purple
21: 1 Goal, yellow
22: 2 Goals, yellow
23: 3 Goals, yellow
24: 4 Goals, yellow
25: Ball Speed, white
26: Ball Strength, white
27: Ball Stamina, white
28: L. Ticket Half
31: Free Kick, yellow
32: TV Award, white
33: Ultra Goalie, white
34: Ultra Ramps, white
35: Ball Spirit, white
36: Ball Skill, white
37: R. Ticket Half
38: Tackle, blue
41: Kickback Lower, green
42: Kickback Center, green
43: Kickback Upper, green
44: R. Ramp Build, white
45: R. Ramp Lock, yellow
46: Ultra Spinner, white
47: Ultra Jets, white
48: Striker, red
51: Goal Jackpot
52: Extra Ball, red
53: Goal, yellow
54: Striker Build, white
55: Light Magna Goalie
56: R. Flipper Lane
57: Shoot Again, red
58: Special, red
61: L. Ramp Build, white
62: L. Loop Build, white
63: Travel, yellow
64: World Cup Final, red
65: L. Ramp Lock
66: Top Lane Left, purple
67: Top Lane Right, purple
68: Skill Lower, yellow
71: Light Jackpots, red
72: Final Draw, white
73: Magna Goalie, orange
74: Left Inlane
75: Light Kickback, green
76: L. Ramp Ticket, purple
77: R. Ramp Ticket, purple
78: Ultra Ramp Collect, yellow
81: Rollover 1, red
82: Rollover 2, red
83: Rollover 3, red
84: Rollover 4, red
85: Skill Top, yellow
86: Skill Middle, yellow
87: Buy-In Button, green, buyin, cabinet
88: Start Button, yellow, start, cabinet

[switches]
11: Unused
12: Magnet Button, button, noscore, cabinet
13: Start Button, start-button, cabinet, intest
14: Tilt, tilt, ingame, noplay
15: Left Flipper Lane, ingame
16: Striker 3, ingame
17: Right Flipper Lane, ingame
18: Right Outlane, ingame
21: Slam Tilt, slam-tilt
23: Buy Extra Ball, button, cabinet
25: Free Kick Target, ingame
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
38: Shooter, noscore, shooter
41: Goal Trough, opto, ingame
42: Goal Popper, opto, ingame
43: Goalie Left, noscore, opto
44: Goalie Right, noscore, opto
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
66: Striker 1, ingame
67: Striker 2, ingame
71: L. Ramp Diverted, ingame
72: L. Ramp Enter, ingame
74: L. Ramp Exit, ingame
75: R. Ramp Enter, ingame
76: Lock Low, ingame
77: Lock High, ingame
78: R. Ramp Exit, ingame
81: Left Jet
82: Upper Jet
83: Lower Jet
84: Left Slingshot
85: Right Slingshot
86: Kickback, ingame
87: Upper Left Lane, ingame
88: Upper Right Lane, ingame

[drives]
H1: Goal Popper, duty(SOL_DUTY_75), time(TIME_66MS)
H2: TV Popper, duty(SOL_DUTY_75), time(TIME_66MS)
H3: Kickback, duty(SOL_DUTY_50), time(TIME_66MS)
H4: Lock Release
H5: Upper Eject
H6: Trough Release, ballserve, duty(SOL_DUTY_75), time(TIME_100MS)
H7: Knocker, knocker
H8: Ramp Divertor

L1: Left Jet
L2: Upper Jet
L3: Lower Jet
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
F7: Lock Magnet, duty(SOL_DUTY_50), time(TIME_66MS)

[gi]
0: Playfield Left
1: Playfield Right
2: Insert Background
3: Insert Title
4: Playfield Top

[tests]

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
Red Lamps: COLOR:red
White Lamps: COLOR:white
Orange Lamps: COLOR:orange
Yellow Lamps: COLOR:yellow
Green Lamps: COLOR:green
Blue Lamps: COLOR:blue
Purple Lamps: COLOR:purple

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
2500:

[highscores]

[flags]
L. Ramp Light Mode:
R. Ramp Light Mode:
5 Man MB Lit:
Final Draw MB Running:
5 Man MB Running:
L. Loop Build:
L. Ramp Build:
Striker Build:
R. Ramp Build:
Divert Ramp:

[globalflags]
Goalie Moving:

[deffs]
PGJ Logo: page(MACHINE_PAGE), c_decl(amode_pgj_logo), PRI_AMODE

[leffs]
Amode: runner, PRI_LEFF1, LAMPS(ALL), GI(ALL), page(MACHINE_PAGE)

[fonts]

[timers]
Goal Trough Made:
Ignore R. Ramp Exit:
Ramp Gate Debounce:

[templates]
Left Slingshot: driver(sling), sw=SW_LEFT_SLINGSHOT, sol=SOL_LEFT_SLING, ontime=4, offtime=8
Right Slingshot: driver(sling), sw=SW_RIGHT_SLINGSHOT, sol=SOL_RIGHT_SLING, ontime=4, offtime=8
Left Jet: driver(sling), sw=SW_LEFT_JET, sol=SOL_LEFT_JET, ontime=4, offtime=8
Upper Jet: driver(sling), sw=SW_UPPER_JET, sol=SOL_UPPER_JET, ontime=4, offtime=8
Lower Jet: driver(sling), sw=SW_LOWER_JET, sol=SOL_LOWER_JET, ontime=4, offtime=8

