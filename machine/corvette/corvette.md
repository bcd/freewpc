#--------------------------------------------------------------------------
# Corvette machine description for FreeWPC
# (C) Copyright 2006-2009 by Dominic Clifton <me@dominicclifton.name>
#
# See tools/genmachine for more information about the format of this file.
#--------------------------------------------------------------------------

Title: Corvette

include platform/wpc/wpc-security.md

Pinmame-Zip: corv_21.zip
Pinmame-ROM: corv_2_1.bin
Lamp-Matrix-Width: 45
Lamp-Matrix-Height: 25

define MACHINE_NUMBER 570
define MACHINE_GRAND_CHAMPION_INITIALS { 'D', 'P', 'C' }
define MACHINE_GRAND_CHAMPION_SCORE { 0x00, 0x20, 0x00, 0x00, 0x00 }
define MACHINE_HIGH_SCORE_INITIALS { 'F', 'R', 'E' }, { 'W', 'P', 'C' }, { 'F', 'R', 'E' }, { 'W', 'P', 'C' }
define MACHINE_HIGH_SCORES { 0x00, 0x17, 0x50, 0x00, 0x00 }, { 0x00, 0x15, 0x00, 0x00, 0x00 }, { 0x00, 0x12, 0x50, 0x00, 0x00 }, { 0x00, 0x10, 0x00, 0x00, 0x00 }
define MACHINE_START_BALL_MUSIC   MUS_MAIN_PLUNGER
define MACHINE_BALL_IN_PLAY_MUSIC MUS_MAIN
#define MACHINE_END_GAME_MUSIC MUS_END_GAME
define MACHINE_BALL_SAVE_LAMP LM_DRIVE_AGAIN

[lamps]
11: Left Rollover, green , x(), y()
12: Middle Rollover, green, x(), y()
13: Right Rollover, green , x(), y()
14: Skid Pad Arrow, amber, x(), y()
15: Sticky Tires, blue, x(), y()
16: Skid Pad Jackpot, red, x(), y() # torque jackpot
17: Route 66 Arrow, amber, x(), y()
18: Race Today, amber, x(), y()
21: Inner Loop Arrow, amber, x(), y()
22: Fuelie, blue, x(), y()
23: Nitrous, blue, x(), y()
24: Inner Loop Jackpot, red, x(), y() # horsepower jackpot
25: Right Outer Loop Arrow, amber, x(), y()
26: Z07 Suspension, blue, x(), y()
27: Big Brakes, blue, x(), y()
28: Super Charger, blue, x(), y()
31: Left Outer Loop Arrow, amber, x(), y()
32: Lite Lock, amber, x(), y()
33: Qualify, amber, x(), y()
34: Big Block, blue, x(), y()
35: ZR-1 Ramp Lock, green, x(), y()
36: 6 Speed Trans, blue, x(), y()
37: Hi Lift Cams, blue, x(), y()
38: ZR-1 Ramp Arrow, amber, x(), y()
41: Corvette 6, yellow , x(), y()
42: Corvette 3, yellow, x(), y()
43: Corvette 1, yellow, x(), y()
44: Corvette 2, yellow, x(), y()
45: Corvette 4, yellow, x(), y()
46: Left Standup 3, amber, x(), y() # Light
47: Left Standup 2, amber, x(), y() # Turbo
48: Left Standup 1, amber, x(), y() # Boost
51: Corvette 9, yellow, x(), y()
52: Corvette 8, yellow, x(), y()
53: Pit Stop, red, x(), y()
54: Corvette 7, yellow, x(), y()
55: Corvette 5, yellow, x(), y()
56: Pit Stop Arrow, amber, x(), y()
57: Spinner Arrow, amber, x(), y()
58: Drive Again, red, x(), y()
61: Left Outer Tail, red, x(), y() # Bonus X2
62: Left Inner Tail, red, x(), y() # Bonus X4
63: Catch Me, white, x(), y()
64: Right Inner Tail, red, x(), y() # Bonus X6
65: Right Outer Tail, red, x(), y() # Bonus X8
66: Right Standup Arrow, yellow, x(), y()
67: Lite Kickback, yellow, x(), y()
68: Start Challenge, green, x(), y()
71: Kickback Arrow, yellow, x(), y()
72: Left Return Lane, white, x(), y() # Extra Ball
73: Right Return Lane, white, x(), y() # Extra Ball
74: Right Out Lane, white, x(), y() # Extra Ball
75: Million Standup, blue, x(), y()
76: Side Pipe 1, red, x(), y()
77: Side Pipe 2, red, x(), y()
78: Side Pipe 3, red, x(), y()
# TODO verify if the left and right tree values are wrong in the manual or wrong in the visual pinball table
# the original rom matches up to the manual, so i think the VPINMAME table is wrong
81: Right Tree Red, red, x(), y()
82: Left Tree Red, red, x(), y()
83: Tree Bottom Yellow, yellow, x(), y()
84: Tree Top Yellow, yellow, x(), y()
85: Right Tree Green, green, x(), y()
86: Left Tree Green, green, x(), y()
87: Buy In, green, buyin, cabinet, x(), y()
88: Start Button, red, start, cabinet, x(), y()

[switches]
11: Kickback, ingame # actually "Left Out Lane"
12: Right Out Lane, ingame
13: Start Button, start-button, cabinet, intest
14: Tilt, cabinet, tilt, ingame, noplay
15: Shooter, edge, noscore, shooter
16: Left Return Lane, ingame
17: Right Return Lane, ingame
18: Spinner, ingame
21: Slam Tilt, cabinet, ingame, slam-tilt
22: Coin Door Closed, noscore, noplay
23: Buyin Button, buyin-button, cabinet
#24: Always Closed
31: Trough 1, noscore, opto
32: Trough 2, noscore, opto
33: Trough 3, noscore, opto
34: Trough 4, noscore, opto
35: Route 66 Entry, ingame, opto
36: Pit Stop Popper, ingame, opto
37: Trough Eject, ingame, opto # stuck off = ball jam
38: Inner Loop Entry, ingame
41: ZR-1 Bottom Entry, ingame, opto
42: ZR-1 Top Entry, ingame, opto
43: Skid Pad Entry, ingame, opto
44: Skid Pad Exit, ingame
45: Route 66 Exit, ingame
46: Left Standup 3, ingame # Light
47: Left Standup 2, ingame # Turbo
48: Left Standup 1, ingame # Boost
51: Left Race Start, noscore, opto
52: Right Race Start, noscore, opto
55: Left Race Encoder, noscore, opto
56: Right Race Encoder, noscore, opto
57: Route 66 Kickout, ingame
58: Skid Route 66 Exit, ingame
61: Left Slingshot, ingame, noplay
62: Right Slingshot, ingame, noplay
63: Left Jet, ingame, noplay
64: Lower Jet, ingame, noplay
65: Upper Jet, ingame, noplay # actually the RIGHT jet
66: Left Rollover, ingame
67: Middle Rollover, ingame
68: Right Rollover, ingame
71: ZR-1 Full Left, opto, noplay, noscore
72: ZR-1 Full Right, opto, noplay, noscore
75: ZR-1 Exit, ingame
76: ZR-1 Lock Ball 1, ingame
77: ZR-1 Lock Ball 2, ingame
78: ZR-1 Lock Ball 3, ingame
81: Million Standup, standup
82: Skid Pad Standup, standup
83: Right Standup, standup # lite kickback
84: Right Rubber, noplay # playfield has hole for switch but no switch
86: Jet Rubber, noplay # playfield has hole for switch but no switch
87: Left Outer Loop, ingame
88: Right Outer Loop, ingame

[drives]
H1: Trough Release, ballserve, duty(SOL_DUTY_75), time(TIME_100MS)
H2: ZR-1 Low Rev Gate, duty(SOL_DUTY_75), time(TIME_200MS)
H3: Kickback, duty(SOL_DUTY_50), time(TIME_66MS)
H4: Pit Stop Popper
H5: ZR-1 Up Rev Gate, duty(SOL_DUTY_75), time(TIME_200MS)
H6: Not Used 1,nosearch
H7: Knocker, knocker
H8: Route 66 Kickout

L1: Left Sling, duty(SOL_DUTY_100), time(TIME_33MS)
L2: Right Sling, duty(SOL_DUTY_100), time(TIME_33MS)
L3: Left Jet, duty(SOL_DUTY_75), time(TIME_33MS)
L4: Lower Jet, duty(SOL_DUTY_75), time(TIME_33MS)
L5: Upper Jet, duty(SOL_DUTY_75), time(TIME_33MS) # actually the RIGHT jet
L6: Not Used 2,nosearch
L7: ZR-1 Lockup
L8: Loop Gate, duty(SOL_DUTY_75), time(TIME_200MS)

# G = J126 on Power Driver Board
G1: Race Direction
G2: Left Race Enable, motor
G3: Right Race Enable, motor
G4: Not Used
# playfield flashers
G5: Jets, flash
G6: Right Ramps, flash
G7: Upper Left Flipper, flash
G8: Catch Me, flash


# A = J122
A1: ZR-1 Ramp, flash
A2: ZR-1 Underside, flash
A3: Right Rear Panel, flash
A4: Right Standup, flash

#F = J902 on Fliptronic II
F5: Ramp Diverter
F6: Divertor Hold

[gi]
0: Playfield Upper Left
1: Playfield Upper Right
2: Playfield Lower Left
3: Playfield Lower Right
4: Backbox Title

[tests]
#corvette_engine:
#corvette_racetrack:

#############################################################

[lamplists]
Top Rollovers: Left Rollover, Middle Rollover, Right Rollover
Extra Ball Rollovers: Left Return Lane, Right Return Lane, Right Out Lane
Red Lamps: set, COLOR:red
White Lamps: set, COLOR:white
Amber Lamps: set, COLOR:amber
Yellow Lamps: set, COLOR:yellow
Green Lamps: set, COLOR:green
Blue Lamps: set, COLOR:blue
Build Up: PF:lamp_sort_bottom_to_top

[containers]
Trough: trough, Trough Release, Trough 4, Trough 3, Trough 2, Trough 1, init_max_count(4)
ZR1 Popper: ZR-1 Lockup, ZR-1 Lock Ball 3, ZR-1 Lock Ball 2, ZR-1 Lock Ball 1
Pitstop Popper: Pit Stop Popper, Pit Stop Popper
Route 66 Popper: Route 66 Kickout, Route 66 Kickout

#############################################################

[adjustments]
Ball Saver: yes_no, YES

[audits]

[system_sounds]

[system_music]

[scores]
750:
1K:
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
5M:
10M:

[highscores]

[flags]
ZR-1 Multiball Running:
ZR-1 Multiball Lite Lock Lit:
ZR-1 Multiball Lock Lit:
Torque Jackpot Lit:
Horsepower Jackpot Lit:
Loop Gate Opened:
ZR-1 Up Rev Gate Opened:
ZR-1 Low Rev Gate Opened:

[globalflags]
Skillshot Enabled:

[deffs]

ZR-1 Ball Locked: page(MACHINE_PAGE), PRI_GAME_QUICK4
ZR-1 Multiball Lit: page(MACHINE_PAGE), PRI_GAME_QUICK5
ZR-1 Multiball Start: page(MACHINE_PAGE), PRI_GAME_QUICK6, D_PAUSE
ZR-1 Multiball Running: runner, page(MACHINE_PAGE), PRI_GAME_MODE5
ZR-1 Multiball H P Jackpot: page(MACHINE_PAGE), D_SCORE, PRI_GAME_QUICK7
ZR-1 Multiball Torque Jackpot: page(MACHINE_PAGE), D_SCORE, PRI_GAME_QUICK7

Kickback Relit: page(MACHINE_PAGE), PRI_GAME_QUICK2
Jet: page(MACHINE_PAGE), PRI_GAME_QUICK1, D_SCORE+D_RESTARTABLE
Sling: page(MACHINE_PAGE), PRI_GAME_QUICK1, D_SCORE+D_RESTARTABLE

[leffs]
Amode: runner, PRI_LEFF1, LAMPS(ALL), GI(ALL), page(MACHINE_PAGE)
Color Cycle: runner, PRI_LEFF3, LAMPS(ALL), page(MACHINE_PAGE)
Build Up: runner, PRI_LEFF3, LAMPS(ALL), page(MACHINE_PAGE)

[fonts]
pcsenior:
misctype:
lithograph:

[timers]
Kickback Made:

[templates]
# This section contains template definitions for device drivers.
# For each device requiring a driver, it specifies the file that
# contains the template code, and any parameters for the specific
# device.
Left Sling: driver(spsol),
	sw=SW_LEFT_SLINGSHOT, sol=SOL_LEFT_SLING,
	ontime=3, offtime=16

Right Sling: driver(spsol),
	sw=SW_RIGHT_SLINGSHOT, sol=SOL_RIGHT_SLING,
	ontime=3, offtime=16

Left Jet: driver(spsol),
	sw=SW_LEFT_JET, sol=SOL_LEFT_JET,
	ontime=3, offtime=16

Upper Jet: driver(spsol),
	sw=SW_UPPER_JET, sol=SOL_UPPER_JET,
	ontime=3, offtime=16

Lower Jet: driver(spsol),
	sw=SW_LOWER_JET, sol=SOL_LOWER_JET,
	ontime=3, offtime=16

Spinner: driver(spinner),
	sw_event=sw_spinner, sw_number=SW_SPINNER

Loop Gate: driver(duty),
	sol=SOL_LOOP_GATE,
	ontime=TIME_300MS, duty_ontime=TIME_33MS, duty_offtime=TIME_16MS, timeout=60

ZR-1 Low Rev Gate: driver(duty),
	sol=SOL_ZR_1_LOW_REV_GATE,
	ontime=TIME_300MS, duty_ontime=TIME_33MS, duty_offtime=TIME_16MS, timeout=60

ZR-1 Up Rev Gate: driver(duty),
	sol=SOL_ZR_1_UP_REV_GATE,
	ontime=TIME_300MS, duty_ontime=TIME_33MS, duty_offtime=TIME_16MS, timeout=60

Kickback Driver: driver(spsol),
	sw=SW_KICKBACK, sol=SOL_KICKBACK,
	ontime=12, offtime=18
