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

define MACHINE_NUMBER 536
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
 
[lamps]
11: Left Rollover, green , y(1), x(9)
12: Middle Rollover, green, y(1), x(11)
13: Right Rollover, green , y(1), x(13)
14: Skid Pad Arrow, amber, y(23), x(10)
15: Sticky Tires, blue, y(24), x(9)
16: Skid Pad Jackpot, red, y(25), x(8) # torque jackpot
17: Route 66 Arrow, amber, y(29), x(11)
18: Race Today, amber, y(31), x(11)
21: Inner Loop Arrow, amber, y(19), x(12)
22: Fuelie, green, y(20), x(11)
23: Nitrous, blue, y(21), x(10)
24: Inner Loop Jackpot, red, y(22), x(9) # horsepower jackpot
25: Right Outer Loop Arrow, amber, y(24), x(16)
26: Z07 Suspension, blue, y(25), x(16)
27: Big Brakes, blue, y(26), x(15)
28: Super Charger, blue, y(27), x(15)
31: Left Outer Loop Arrow, amber, y(27), x(4)
32: Lite Lock, amber, y(28), x(5)
33: Qualify, amber, y(29), x(6)
34: Big Block, blue, y(30), x(7)
35: ZR1 Ramp Lock, green, y(28), x(7)
36: 6 Speed Trans, blue, y(27), x(7)
37: Hi Lift Cams, blue, y(26), x(7)
38: ZR1 Ramp Arrow, amber, y(25), x(7)
41: Corvette 6, yellow, y(35), x(13)
42: Corvette 3, yellow, y(36), x(11)
43: Corvette 1, yellow, y(37), x(9)
44: Corvette 2, yellow, y(36), x(7)
45: Corvette 4, yellow, y(35), x(5)
46: Left Standup 3, amber, y(30), x(4) # Light
47: Left Standup 2, amber, y(31), x(4) # Turbo
48: Left Standup 1, amber, y(32), x(4) # Boost
51: Corvette 9, yellow, y(33), x(9)
52: Corvette 8, yellow, y(34), x(11)
53: Pit Stop, red, y(16), x(7)
54: Corvette 7, yellow, y(34), x(7)
55: Corvette 5, yellow, y(35), x(9)
56: Pit Stop Arrow, amber, y(18), x(9)
57: Spinner Arrow, amber, y(17), x(11)
58: Drive Again, red, y(45), x(9)
61: Left Outer Tail, red, y(40), x(6) # Bonus X2
62: Left Inner Tail, red, y(40), x(5) # Bonus X4
63: Catch Me, white, y(41), x(9)
64: Right Inner Tail, red, y(40), x(11) # Bonus X6
65: Right Outer Tail, red, y(40), x(12) # Bonus X8
66: Right Standup Arrow, yellow, y(31), x(15)
67: Lite Kickback, yellow, y(32), x(14)
68: Start Challenge, green, y(16), x(7)
71: Kickback Arrow, yellow, y(38), x(1)
72: Left Return Lane, white, y(37), x(2) # Extra Ball
73: Right Return Lane, white, y(37), x(15) # Extra Ball
74: Right Out Lane, white, y(38), x(16) # Extra Ball
75: Million Standup, blue, y(23), x(5)
76: Side Pipe 1, red, y(25), x(1)
77: Side Pipe 2, red, y(23), x(1)
78: Side Pipe 3, red, y(21), x(1)
# TODO verify if the left and right tree values are wrong in the manual or wrong in the visual pinball table
# the original rom matches up to the manual, so i think the VPINMAME table is wrong
81: Right Tree Red, red, y(39), x(17)
82: Left Tree Red, red, y(39), x(17)
83: Tree Bottom Yellow, yellow, y(37), x(17)
84: Tree Top Yellow, yellow, y(36), x(17)
85: Right Tree Green, green, y(38), x(17)
86: Left Tree Green, green, y(38), x(17)
87: Buy In, green, buyin, cabinet, y(50), x(17)
88: Start Button, red, start, cabinet, y(50), x(3)

[switches]
11: Kickback, ingame # actually "Left Out Lane"
12: Right Out Lane, ingame
13: Start Button, start-button, cabinet, intest
14: Tilt, cabinet, tilt, ingame, novalid
# not called 'Shooter', see: trivial.c/CALLSET_ENTRY (trivial, sw_plunger)
15: Plunger, shooter, edge, debounce(TIME_200MS)
16: Left Return Lane, ingame
17: Right Return Lane, ingame
18: Spinner, ingame
21: Slam Tilt, cabinet, ingame, slam-tilt
22: Coin Door Closed, noscore, novalid
23: Buyin Button, buyin-button, cabinet
#24: Always Closed
31: Trough 1, noscore, novalid, opto
32: Trough 2, noscore, novalid, opto
33: Trough 3, noscore, novalid, opto
34: Trough 4, noscore, novalid, opto
35: Route 66 Entry, ingame, opto
36: Pit Stop Popper, ingame, opto
37: Trough Eject, noscore, novalid, opto # stuck off = ball jam
38: Inner Loop Entry, ingame
41: ZR1 Bottom Entry, ingame, opto
42: ZR1 Top Entry, ingame, opto
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
61: Left Slingshot, ingame, novalid
62: Right Slingshot, ingame, novalid
63: Left Jet, ingame, novalid
64: Lower Jet, ingame, novalid
65: Upper Jet, ingame, novalid # actually the RIGHT jet
66: Left Rollover, ingame
67: Middle Rollover, ingame
68: Right Rollover, ingame
71: ZR1 Full Left, opto, novalid, noscore
72: ZR1 Full Right, opto, novalid, noscore
75: ZR1 Exit, ingame
76: ZR1 Lock Ball 1, ingame
77: ZR1 Lock Ball 2, ingame
78: ZR1 Lock Ball 3, ingame
81: Million Standup, standup
82: Skid Pad Standup, standup
83: Right Standup, standup # lite kickback
84: Right Rubber, novalid # playfield has hole for switch but no switch
86: Jet Rubber, novalid # playfield has hole for switch but no switch
87: Left Outer Loop, ingame
88: Right Outer Loop, ingame

[drives]
H1: Trough Release, ballserve, duty(SOL_DUTY_100), time(TIME_66MS)
H2: ZR1 Low Rev Gate, duty(SOL_DUTY_50), time(TIME_66MS)
H3: Kickback, duty(SOL_DUTY_75), time(TIME_66MS)
H4: Pit Stop Popper, duty(SOL_DUTY_75), time(TIME_66MS)
H5: ZR1 Up Rev Gate, duty(SOL_DUTY_50), time(TIME_66MS)
H6: Not Used 1, nosearch
H7: Knocker, knocker, duty(SOL_DUTY_75), time(TIME_50MS)
H8: Route 66 Kickout, duty(SOL_DUTY_75), time(TIME_66MS)

L1: Left Sling, duty(SOL_DUTY_100), time(TIME_33MS)
L2: Right Sling, duty(SOL_DUTY_100), time(TIME_33MS)
L3: Left Jet, duty(SOL_DUTY_75), time(TIME_33MS)
L4: Lower Jet, duty(SOL_DUTY_75), time(TIME_33MS)
L5: Upper Jet, duty(SOL_DUTY_75), time(TIME_33MS) # actually the RIGHT jet
L6: Not Used 2, nosearch
L7: ZR1 Lockup, duty(SOL_DUTY_100), time(TIME_100MS)
L8: Loop Gate, duty(SOL_DUTY_50), time(TIME_66MS)

# G = J126 on Power Driver Board
G1: Race Direction, nosearch
G2: Left Race Enable, motor
G3: Right Race Enable, motor
G4: Future Car, flash, duty(SOL_DUTY_75), time(TIME_16MS)
# playfield flashers
G5: Jets, flash, duty(SOL_DUTY_100), time(TIME_33MS)
G6: Right Ramps, flash, duty(SOL_DUTY_75), time(TIME_16MS)
G7: Upper Left Flipper, flash, duty(SOL_DUTY_100), time(TIME_16MS)
G8: Catch Me, flash, duty(SOL_DUTY_75), time(TIME_16MS)


# A = J122
A1: ZR1 Ramp, flash, duty(SOL_DUTY_75), time(TIME_16MS)
A2: ZR1 Underside, flash, duty(SOL_DUTY_75), time(TIME_16MS)
A3: Right Rear Panel, flash, duty(SOL_DUTY_75), time(TIME_16MS)
A4: Right Standup, flash, duty(SOL_DUTY_75), time(TIME_16MS)

# F = J902 on Fliptronic II
F5: Ramp Diverter, duty(SOL_DUTY_100), time(TIME_100MS)
F6: Ramp Diverter Hold, duty(SOL_DUTY_100), time(TIME_100MS)
F7: U.L. Flip Power, duty(SOL_DUTY_100), time(TIME_50MS)
F8: U.L. Flip Hold, duty(SOL_DUTY_100), time(TIME_100MS)

[gi]
0: Playfield U.L.
1: Playfield U.R.
2: Playfield L.L.
3: Playfield L.R.
4: Backbox Title

#############################################################

[tests]
#Templates that use include test must still be listed here otherwise it won't be listed in the test menu
corvette_zr1:
diverter: #autogenerated-test, see 'Diverter' defintion in [templates] section.
corvette_racetrack:

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
ZR1 Popper: ZR1 Lockup, ZR1 Lock Ball 3, ZR1 Lock Ball 2, ZR1 Lock Ball 1, settle_delay(TIME_1500MS)
Pitstop Popper: Pit Stop Popper, Pit Stop Popper
Route 66 Popper: Route 66 Kickout, Route 66 Kickout

#############################################################

[adjustments]
#TODO allow and enable the ball saver when it doesn't save balls during multi-ball on a machine with no autofire
Ball Saver: yes_no, NO

Enable ZR1_Engine: yes_no, YES
Enable Racetrack: yes_no, YES

[audits]
ZR1 Errors:
Racetrack Errors:

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
ZR1 Multiball Running:
ZR1 Multiball Lite Lock Lit:
ZR1 Multiball Lock Lit:
Torque Jackpot Lit:
Horsepower Jackpot Lit:
Loop Gate Opened:
ZR1 Up Rev Gate Opened:
ZR1 Low Rev Gate Opened:
Diverter Opened:

[globalflags]
Skillshot Rollover Enabled:
ZR1 Working:
ZR1 Solenoids Powered:
Racetrack Working:

[deffs]
ZR1 Ball Locked: page(MACHINE_PAGE), PRI_GAME_QUICK4, D_PAUSE
ZR1 MB Lit: page(MACHINE_PAGE), PRI_GAME_QUICK5
ZR1 MB Start: page(MACHINE_PAGE), PRI_GAME_QUICK6, D_PAUSE
ZR1 MB Running: runner, page(MACHINE_PAGE), PRI_GAME_MODE5
ZR1 MB HP JP: page(MACHINE_PAGE), PRI_GAME_QUICK7, D_SCORE
ZR1 MB TRQ JP: page(MACHINE_PAGE), PRI_GAME_QUICK7, D_SCORE 

Kickback Relit: page(MACHINE_PAGE), PRI_GAME_QUICK2
Jet: page(MACHINE_PAGE), PRI_GAME_QUICK1, D_SCORE+D_RESTARTABLE
Sling: page(MACHINE_PAGE), PRI_GAME_QUICK1, D_SCORE+D_RESTARTABLE
Skill Menu: page(MACHINE_PAGE), PRI_BALL_LAUNCH_MENU, D_RESTARTABLE

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
Left Sling: driver(sling), sw=SW_LEFT_SLINGSHOT, sol=SOL_LEFT_SLING
Right Sling: driver(sling), sw=SW_RIGHT_SLINGSHOT, sol=SOL_RIGHT_SLING
Left Jet: driver(jet), sw=SW_LEFT_JET, sol=SOL_LEFT_JET
Upper Jet: driver(jet), sw=SW_UPPER_JET, sol=SOL_UPPER_JET
Lower Jet: driver(jet), sw=SW_LOWER_JET, sol=SOL_LOWER_JET

Spinner: driver(spinner), sw_event=sw_spinner, sw_number=SW_SPINNER

Loop Gate: driver(duty),
	sol=SOL_LOOP_GATE,
	ontime=TIME_300MS, duty_ontime=TIME_33MS, duty_offtime=TIME_16MS, timeout=60

Diverter: driver(divhold),
	power_sol=SOL_RAMP_DIVERTER,
	hold_sol=SOL_RAMP_DIVERTER_HOLD,
	mode=1,
	power_pulse_ms=96,
	schedule_ms=32,
	includetest(yes)

ZR1 Low Rev Gate: driver(duty),
	sol=SOL_ZR1_LOW_REV_GATE,
	ontime=TIME_300MS, duty_ontime=TIME_33MS, duty_offtime=TIME_16MS, timeout=60

ZR1 Up Rev Gate: driver(duty),
	sol=SOL_ZR1_UP_REV_GATE,
	ontime=TIME_300MS, duty_ontime=TIME_33MS, duty_offtime=TIME_16MS, timeout=60

Kickback Driver: driver(spsol),
	sw=SW_KICKBACK, sol=SOL_KICKBACK,
	ontime=12, offtime=18, auto=1
