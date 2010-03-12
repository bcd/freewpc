#--------------------------------------------------------------------------
# Corvette machine description for FreeWPC
# (C) Copyright 2010 by Dominic Clifton <me@dominicclifton.name>
#
# See tools/genmachine for more information about the format of this file.
#--------------------------------------------------------------------------

Title: Corvette

include platform/wpc/wpc-security.md

Pinmame-Zip: corv_21.zip
Pinmame-ROM: corv_2_1.bin
Lamp-Matrix-Width: 51
Lamp-Matrix-Height: 18

define MACHINE_NUMBER 570
define MACHINE_GRAND_CHAMPION_INITIALS { 'D', 'P', 'C' }
define MACHINE_GRAND_CHAMPION_SCORE { 0x00, 0x20, 0x00, 0x00, 0x00 }
define MACHINE_HIGH_SCORE_INITIALS { 'F', 'R', 'E' }, { 'W', 'P', 'C' }, { 'F', 'R', 'E' }, { 'W', 'P', 'C' }
define MACHINE_HIGH_SCORES { 0x00, 0x17, 0x50, 0x00, 0x00 }, { 0x00, 0x15, 0x00, 0x00, 0x00 }, { 0x00, 0x12, 0x50, 0x00, 0x00 }, { 0x00, 0x10, 0x00, 0x00, 0x00 }
define MACHINE_START_BALL_MUSIC   MUS_MAIN_PLUNGER
define MACHINE_BALL_IN_PLAY_MUSIC MUS_MAIN
#define MACHINE_END_GAME_MUSIC MUS_END_GAME
define MACHINE_BALL_SAVE_LAMP LM_DRIVE_AGAIN
define MACHINE_HAS_UPPER_LEFT_FLIPPER

# DC - To generate the x/y coordinates I took a screengrab of the lamp-locations diagram in the manual such that the
# width of the playfield was 409 pixels wide then I pasted the image into mspaint (windows 7 version) and moved the
# top left of the table so that it was placed at x:70,y:92.
# To determine the x and y location i noted down the pixel co-ordinates of the center (more or less) of each lamp
# and divided the number by 20 and rounded the number up or down manually, aligning with adjacent or in-line lights
# as appropriate.
# Finally, I subtracted 4 from each horizontal value to normalize the horizontal values so that the left most lamps
# values had a horizontal value of 1.
 
# NOTE: y is horizontal, x is vertical due to a bug in genmachine
[lamps]
11: Left Rollover, green , x(1), y(9)
12: Middle Rollover, green, x(1), y(11)
13: Right Rollover, green , x(1), y(13)
14: Skid Pad Arrow, amber, x(23), y(10)
15: Sticky Tires, blue, x(24), y(9)
16: Skid Pad Jackpot, red, x(25), y(8) # torque jackpot
17: Route 66 Arrow, amber, x(29), y(11)
18: Race Today, amber, x(31), y(11)
21: Inner Loop Arrow, amber, x(19), y(12)
22: Fuelie, blue, x(20), y(11)
23: Nitrous, blue, x(21), y(10)
24: Inner Loop Jackpot, red, x(22), y(9) # horsepower jackpot
25: Right Outer Loop Arrow, amber, x(24), y(16)
26: Z07 Suspension, blue, x(25), y(16)
27: Big Brakes, blue, x(26), y(15)
28: Super Charger, blue, x(27), y(15)
31: Left Outer Loop Arrow, amber, x(27), y(4)
32: Lite Lock, amber, x(28), y(5)
33: Qualify, amber, x(29), y(6)
34: Big Block, blue, x(30), y(7)
35: ZR-1 Ramp Lock, green, x(28), y(7)
36: 6 Speed Trans, blue, x(27), y(7)
37: Hi Lift Cams, blue, x(26), y(7)
38: ZR-1 Ramp Arrow, amber, x(25), y(7)
41: Corvette 6, yellow, x(35), y(13)
42: Corvette 3, yellow, x(36), y(11)
43: Corvette 1, yellow, x(37), y(9)
44: Corvette 2, yellow, x(36), y(7)
45: Corvette 4, yellow, x(35), y(5)
46: Left Standup 3, amber, x(30), y(4) # Light
47: Left Standup 2, amber, x(31), y(4) # Turbo
48: Left Standup 1, amber, x(32), y(4) # Boost
51: Corvette 9, yellow, x(33), y(9)
52: Corvette 8, yellow, x(34), y(11)
53: Pit Stop, red, x(16), y(7)
54: Corvette 7, yellow, x(34), y(7)
55: Corvette 5, yellow, x(35), y(9)
56: Pit Stop Arrow, amber, x(18), y(9)
57: Spinner Arrow, amber, x(17), y(11)
58: Drive Again, red, x(45), y(9)
61: Left Outer Tail, red, x(40), y(6) # Bonus X2
62: Left Inner Tail, red, x(40), y(5) # Bonus X4
63: Catch Me, white, x(41), y(9)
64: Right Inner Tail, red, x(40), y(11) # Bonus X6
65: Right Outer Tail, red, x(40), y(12) # Bonus X8
66: Right Standup Arrow, yellow, x(31), y(15)
67: Lite Kickback, yellow, x(32), y(14)
68: Start Challenge, green, x(16), y(7)
71: Kickback Arrow, yellow, x(38), y(1)
72: Left Return Lane, white, x(37), y(2) # Extra Ball
73: Right Return Lane, white, x(37), y(15) # Extra Ball
74: Right Out Lane, white, x(38), y(16) # Extra Ball
75: Million Standup, blue, x(23), y(5)
76: Side Pipe 1, red, x(25), y(1)
77: Side Pipe 2, red, x(23), y(1)
78: Side Pipe 3, red, x(21), y(1)
# TODO verify if the left and right tree values are wrong in the manual or wrong in the visual pinball table
# the original rom matches up to the manual, so i think the VPINMAME table is wrong
81: Right Tree Red, red, x(39), y(17)
82: Left Tree Red, red, x(39), y(17)
83: Tree Bottom Yellow, yellow, x(37), y(17)
84: Tree Top Yellow, yellow, x(36), y(17)
85: Right Tree Green, green, x(38), y(17)
86: Left Tree Green, green, x(38), y(17)
87: Buy In, green, buyin, cabinet, x(50), y(17)
88: Start Button, red, start, cabinet, x(50), y(3)

[switches]
11: Kickback, ingame # actually "Left Out Lane"
12: Right Out Lane, ingame
13: Start Button, start-button, cabinet, intest
14: Tilt, cabinet, tilt, ingame, noplay
# not called 'Shooter', see: trivial.c/CALLSET_ENTRY (trivial, sw_plunger)
15: Plunger, shooter, edge, noplay, debounce(TIME_200MS)
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
H1: Trough Release, ballserve, duty(SOL_DUTY_100), time(TIME_66MS)
H2: ZR-1 Low Rev Gate, duty(SOL_DUTY_50), time(TIME_66MS)
H3: Kickback, duty(SOL_DUTY_75), time(TIME_66MS)
H4: Pit Stop Popper, duty(SOL_DUTY_75), time(TIME_66MS)
H5: ZR-1 Up Rev Gate, duty(SOL_DUTY_50), time(TIME_66MS)
H6: Not Used 1,nosearch
H7: Knocker, knocker, duty(SOL_DUTY_75), time(TIME_33MS)
H8: Route 66 Kickout, duty(SOL_DUTY_75), time(TIME_66MS)

L1: Left Sling, duty(SOL_DUTY_100), time(TIME_33MS)
L2: Right Sling, duty(SOL_DUTY_100), time(TIME_33MS)
L3: Left Jet, duty(SOL_DUTY_75), time(TIME_33MS)
L4: Lower Jet, duty(SOL_DUTY_75), time(TIME_33MS)
L5: Upper Jet, duty(SOL_DUTY_75), time(TIME_33MS) # actually the RIGHT jet
L6: Not Used 2,nosearch
L7: ZR-1 Lockup, duty(SOL_DUTY_75), time(TIME_66MS)
L8: Loop Gate, duty(SOL_DUTY_50), time(TIME_66MS)

# G = J126 on Power Driver Board
G1: Race Direction
G2: Left Race Enable, motor
G3: Right Race Enable, motor
G4: Not Used 3, nosearch
# playfield flashers
G5: Jets, flash, duty(SOL_DUTY_100), time(TIME_33MS)
G6: Right Ramps, flash, duty(SOL_DUTY_75), time(TIME_16MS)
G7: Upper Left Flipper, flash, duty(SOL_DUTY_100), time(TIME_16MS)
G8: Catch Me, flash, duty(SOL_DUTY_75), time(TIME_16MS)


# A = J122
A1: ZR-1 Ramp, flash, duty(SOL_DUTY_75), time(TIME_16MS)
A2: ZR-1 Underside, flash, duty(SOL_DUTY_75), time(TIME_16MS)
A3: Right Rear Panel, flash, duty(SOL_DUTY_75), time(TIME_16MS)
A4: Right Standup, flash, duty(SOL_DUTY_75), time(TIME_16MS)

# F = J902 on Fliptronic II
F5: Ramp Diverter, duty(SOL_DUTY_100), time(TIME_50MS)
F6: Ramp Divertor Hold, duty(SOL_DUTY_100), time(TIME_100MS)
F7: U.L. Flip Power, duty(SOL_DUTY_100), time(TIME_50MS)
F8: U.L. Flip Hold, duty(SOL_DUTY_100), time(TIME_100MS)

[gi]
0: Playfield U.L.
1: Playfield U.R.
2: Playfield L.L.
3: Playfield L.R.
4: Backbox Title

[tests]
corvette_zr1:
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

# todo change the default to YES, this is just for testing...
Enable ZR1_Engine: yes_no, NO

[audits]
ZR1 Errors:

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
20M:
25M:

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
ZR1 Working:
ZR1 Solenoids Powered:

[deffs]

ZR-1 Ball Locked: page(MACHINE_PAGE), PRI_GAME_QUICK4
ZR-1 MB Lit: page(MACHINE_PAGE), PRI_GAME_QUICK5
ZR-1 MB Start: page(MACHINE_PAGE), PRI_GAME_QUICK6, D_PAUSE
ZR-1 MB Running: runner, page(MACHINE_PAGE), PRI_GAME_MODE5
ZR-1 MB HP JP: page(MACHINE_PAGE), PRI_GAME_QUICK7, D_SCORE
ZR-1 MB TRQ JP: page(MACHINE_PAGE), PRI_GAME_QUICK7, D_SCORE 

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
