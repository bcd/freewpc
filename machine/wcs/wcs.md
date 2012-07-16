#--------------------------------------------------------------------------
# WCS machine description for FreeWPC
# (C) Copyright 2006-2011 by Brian Dominy <brian@oddchange.com>
#
# See tools/genmachine for more information about the format of this file.
#--------------------------------------------------------------------------

Title: World Cup

include platform/wpc/wpc-security.md

Pinmame-Zip: wcs_l2.zip
Pinmame-ROM: wcup_lx2.bin
Lamp-Matrix-Width: 45
Lamp-Matrix-Height: 25

define MACHINE_WCS
define MACHINE_NUMBER 531
define MACHINE_CUSTOM_AMODE
define MACHINE_GRAND_CHAMPION_INITIALS { 'W', 'C', 'S' }
define MACHINE_GRAND_CHAMPION_SCORE { 0x00, 0x20, 0x00, 0x00, 0x00 }
define MACHINE_HIGH_SCORE_INITIALS { 'B', 'E', 'K' }, { 'K', 'M', 'E' }, { 'N', 'E', 'S' }, { 'R', 'J', 'S' }
define MACHINE_HIGH_SCORES { 0x00, 0x17, 0x50, 0x00, 0x00 }, { 0x00, 0x15, 0x00, 0x00, 0x00 }, { 0x00, 0x12, 0x50, 0x00, 0x00 }, { 0x00, 0x10, 0x00, 0x00, 0x00 }
define MACHINE_START_BALL_MUSIC   MUS_MAIN_PLUNGER
define MACHINE_BALL_IN_PLAY_MUSIC MUS_MAIN

[lamps]
11: Chicago P, purple, y(39), x(12)
12: Dallas U, purple, y(37), x(11)
13: Boston C, purple, y(35), x(10)
14: New York D, purple, y(33), x( 9)
15: Orlando L, purple, y(31), x( 8)
16: Washington R, purple, y(29), x( 7)
17: San Francisco O, purple, y(27), x( 6)
18: Detroit W, purple, y(25), x( 5)
21: 1 Goal, yellow, y(36), x(13)
22: 2 Goals, yellow, y(34), x(13)
23: 3 Goals, yellow, y(32), x(13)
24: 4 Goals, yellow, y(30), x(13)
25: Ball Speed, white, y(27), x(12)
26: Ball Strength, white, y(25), x(11)
27: Ball Stamina, white, y(23), x(12)
28: L. Ticket Half, purple, y(21), x(11)
31: Free Kick, yellow ,y(16), x(16)
32: TV Award, white , y(17), x(18)
33: Ultra Goalie, white , y(31), x(16)
34: Ultra Ramps, white , y(29), x(17)
35: Ball Spirit, white , y(26), x(14)
36: Ball Skill, white , y(24), x(14)
37: R. Ticket Half, purple, y(21), x(13)
38: Tackle, blue , y(14), x( 9)
41: Kickback Lower, green , y(38), x( 1)
42: Kickback Center, green , y(36), x( 1)
43: Kickback Upper, green , y(34), x( 1)
44: R. Ramp Build, white , y(19), x(20)
45: R. Ramp Lock, yellow , y(21), x(19)
46: Ultra Spinner, white , y(25), x(18)
47: Ultra Jets, white , y(27), x(18)
48: Striker, red , y(3), x(15)
51: Goal Jackpot, red , y(8), x(12)
52: Extra Ball, red , y(7), x(14)
53: Goal, yellow , y(6), x(11)
54: Striker Build, white , y(5), x(15)
55: Light Magna Goalie , y(29), x(21)
56: R. Flipper Lane , y(35), x(22)
57: Shoot Again, red, shoot-again , y(43), x(17)
58: Special, red , y(38), x(24)
61: L. Ramp Build, white , y(19), x( 6)
62: L. Loop Build, white , y(19), x( 2)
63: Travel, yellow , y(21), x( 3)
64: World Cup Final, red , y(23), x( 4)
65: L. Ramp Lock, yellow , y(21), x( 7)
66: Top Lane Left, purple , y(1), x(17)
67: Top Lane Right, purple , y(1), x(19)
68: Skill Lower, yellow , y(30), x(25)
71: Light Jackpots, red , y(16), x( 4)
72: Final Draw, white , y(19), x(17)
73: Magna Goalie, orange , y(43), x( 7)
74: Left Inlane , y(35), x( 3)
75: Light Kickback, green, y(30), x( 4)
76: L. Ramp Ticket, yellow, y(16), x( 6)
77: R. Ramp Ticket, yellow, y(16), x(20)
78: Ultra Ramp Collect, yellow, y(16), x( 5)
81: Rollover 1, red, y(10), x(13)
82: Rollover 2, red, y(13), x(12)
83: Rollover 3, red, y(16), x(13)
84: Rollover 4, red, y(19), x(14)
85: Skill Top, yellow, y(26), x(25)
86: Skill Middle, yellow, y(28), x(25)
87: Buy-In Button, green, buyin, cabinet, y(45), x(21)
88: Start Button, yellow, start, cabinet, y(45), x( 3)

[switches]
11: Unused
12: Magnet Button, button, noscore, cabinet
13: Start Button, start-button, cabinet, intest
14: Tilt, cabinet, tilt, ingame, novalid
15: Left Flipper Lane, ingame
16: Striker 3, ingame
17: Right Flipper Lane, ingame
18: Right Outlane, ingame
21: Slam Tilt, cabinet, ingame, slam-tilt
23: Buyin Button, buyin-button, cabinet
25: Free Kick Target, ingame
26: Kickback Upper, ingame
27: Spinner, ingame
28: Light Kickback, ingame
31: Trough 1, noscore, opto
32: Trough 2, noscore, opto
33: Trough 3, noscore, opto
34: Trough 4, noscore, opto
35: Trough 5, noscore, opto
36: Trough Stack, noscore, opto
37: Light Magna Goalie, ingame
38: Shooter, edge, noscore, shooter, debounce(TIME_200MS)
41: Goal Trough, opto, ingame
42: Goal Popper, opto, ingame
43: Goalie Left, novalid, intest, service, opto
44: Goalie Right, novalid, intest, service, opto
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
81: Left Jet, ingame, novalid
82: Upper Jet, ingame, novalid
83: Lower Jet, ingame, novalid
84: Left Slingshot, ingame, novalid
85: Right Slingshot, ingame, novalid
86: Kickback, ingame
87: Upper Left Lane, ingame
88: Upper Right Lane, ingame

[drives]
H1: Goal Popper, duty(SOL_DUTY_75), time(TIME_133MS)
H2: TV Popper, duty(SOL_DUTY_75), time(TIME_166MS)
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
2500:
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
Skill Lit:

Goal Lit:

Multiball Lit:
Multiball Running:
Left Eject Lock:
Right Eject Lock:
Jackpot Lit:
Super Jackpot Lit:

TV Lit:
Mode Lit:
Super Dog Lit:

Ultra Mania Lit:
Ultra Mania Running:

QuickMB Running:

Magna Goalie Lit:

Final Match Lit:

[globalflags]
Goalie Moving:
Goalie Active:
Ramp Diverting:

[deffs]
PGJ Logo: page(MACHINE_PAGE), c_decl(amode_pgj_logo), PRI_AMODE
Free Kick: page(MACHINE_PAGE), PRI_GAME_QUICK1, D_SCORE+D_RESTARTABLE

Combo: page(MACHINE_PAGE), PRI_GAME_QUICK3, D_SCORE+D_RESTARTABLE

Jet: page(MACHINE_PAGE), PRI_GAME_QUICK1, D_SCORE+D_RESTARTABLE

Lightning Test: page(MACHINE_PAGE), PRI_GAME_QUICK1

Unlit Goal: page(MACHINE_PAGE), PRI_GAME_QUICK1
Goal Lit: page(MACHINE_PAGE), PRI_GAME_QUICK2, D_QUEUED+D_TIMEOUT
Goal Increase: page(MACHINE_PAGE), PRI_GAME_QUICK4
Goal Scored: page(MACHINE_PAGE), PRI_GAME_QUICK8, D_SCORE

Ball Locked: page(MACHINE_PAGE), PRI_GAME_QUICK4
Multiball Lit: page(MACHINE_PAGE), PRI_GAME_QUICK5
Multiball Start: page(MACHINE_PAGE), PRI_GAME_QUICK6, D_PAUSE
Multiball Running: runner, page(MACHINE_PAGE), PRI_GAME_MODE5
Multiball Jackpot: page(MACHINE_PAGE), PRI_GAME_QUICK7, D_SCORE
Multiball Super Jackpot: page(MACHINE_PAGE), PRI_GAME_QUICK8, D_SCORE
Multiball Spinner: page(MACHINE_PAGE), PRI_GAME_QUICK6

TV Lit: page(MACHINE_PAGE), PRI_GAME_QUICK4
TV Award: page(MACHINE_PAGE), PRI_GAME_QUICK6, D_PAUSE

Unlit Striker: page(MACHINE_PAGE), PRI_GAME_QUICK1
Mode Target Status: page(MACHINE_PAGE), PRI_GAME_QUICK2, D_RESTARTABLE
Mode Lit: page(MACHINE_PAGE), PRI_GAME_QUICK4
Mode Started: page(MACHINE_PAGE), PRI_GAME_QUICK6, D_PAUSE
#Mode Running: page(MACHINE_PAGE)

Ultra Start: page(MACHINE_PAGE), PRI_GAME_QUICK6, D_RESTARTABLE
Ultra Collect: page(MACHINE_PAGE), PRI_GAME_QUICK4, D_SCORE+D_RESTARTABLE
Ultra Spot: page(MACHINE_PAGE), PRI_GAME_QUICK6, D_PAUSE
#Ultra Mania Running: page(MACHINE_PAGE)

Skill Shot Ready: page(MACHINE_PAGE), PRI_GAME_MODE6
Skill Shot Made: page(MACHINE_PAGE), PRI_GAME_MODE6
Skill Shot Missed: page(MACHINE_PAGE), PRI_GAME_MODE6

Kickback Relit: page(MACHINE_PAGE), PRI_GAME_QUICK2

[leffs]
Amode: runner, PRI_LEFF1, LAMPS(ALL), GI(ALL), page(MACHINE_PAGE)
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
# This section contains template definitions for device drivers.
# For each device requiring a driver, it specifies the file that
# contains the template code, and any parameters for the specific
# device.
Left Sling: driver(sling), sw=SW_LEFT_SLINGSHOT, sol=SOL_LEFT_SLING
Right Sling: driver(sling), sw=SW_RIGHT_SLINGSHOT, sol=SOL_RIGHT_SLING
Left Jet: driver(jet), sw=SW_LEFT_JET, sol=SOL_LEFT_JET
Upper Jet: driver(jet), sw=SW_UPPER_JET, sol=SOL_UPPER_JET
Lower Jet: driver(jet), sw=SW_LOWER_JET, sol=SOL_LOWER_JET

Spinner: driver(spinner),
	sw_event=sw_spinner, sw_number=SW_SPINNER

Gate: driver(duty),
	sol=SOL_LOOP_GATE,
	ontime=TIME_300MS, duty_ontime=TIME_33MS, duty_offtime=TIME_16MS, timeout=60

Kickback Driver: driver(spsol),
	sw=SW_KICKBACK, sol=SOL_KICKBACK,
	ontime=12, offtime=18

Spinning Ball: driver(duty),
	sol=SOL_BALL_FORWARD,
	ontime=0, duty_ontime=TIME_33MS, duty_offtime=TIME_200MS, timeout=0

Goalie: driver(duty),
	sol=SOL_GOALIE_DRIVE,
	ontime=0, duty_ontime=TIME_16MS, duty_offtime=TIME_33MS, timeout=0

Magna Goalie: driver(duty),
	sol=SOL_MAGNA_GOALIE,
	ontime=TIME_66MS, duty_ontime=TIME_16MS, duty_offtime=TIME_66MS, timeout=TIME_2S

Lock Magnet: driver(duty),
	sol=SOL_LOCK_MAGNET,
	ontime=TIME_66MS, duty_ontime=TIME_16MS, duty_offtime=TIME_100MS, timeout=TIME_2S

Soccer Ball: driver(bivar),
	forward_sol=SOL_BALL_FORWARD,
	reverse_sol=SOL_BALL_REVERSE

