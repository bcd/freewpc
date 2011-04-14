#--------------------------------------------------------------------------
# Theatre of Magic machine description for FreeWPC
# (C) Copyright 2011 by Brian Dominy <brian@oddchange.com>
#
# See tools/genmachine for more information about the format of this file.
#--------------------------------------------------------------------------

Title: Theatre Of Magic

include platform/wpc/wpc-security.md

Pinmame-Zip: wcs_l2.zip
Pinmame-ROM: wcup_lx2.bin
Lamp-Matrix-Width: 45
Lamp-Matrix-Height: 25

define MACHINE_TOM
define MACHINE_NUMBER 531
#define MACHINE_CUSTOM_AMODE
#define MACHINE_GRAND_CHAMPION_INITIALS { 'W', 'C', 'S' }
#define MACHINE_GRAND_CHAMPION_SCORE { 0x00, 0x20, 0x00, 0x00, 0x00 }
#define MACHINE_HIGH_SCORE_INITIALS { 'B', 'E', 'K' }, { 'K', 'M', 'E' }, { 'N', 'E', 'S' }, { 'R', 'J', 'S' }
#define MACHINE_HIGH_SCORES { 0x00, 0x17, 0x50, 0x00, 0x00 }, { 0x00, 0x15, 0x00, 0x00, 0x00 }, { 0x00, 0x12, 0x50, 0x00, 0x00 }, { 0x00, 0x10, 0x00, 0x00, 0x00 }
#define MACHINE_START_BALL_MUSIC   MUS_MAIN_PLUNGER
#define MACHINE_BALL_IN_PLAY_MUSIC MUS_MAIN

[lamps]


[switches]
11: Unused
12: Magnet Button, button, noscore, cabinet
13: Start Button, start-button, cabinet, intest
14: Tilt, cabinet, tilt, ingame, noplay

[drives]
H1: Ball Trough
H2: Magnet Diverter
H3: Trap Door Up
H4: Subway Popper
H5: Right Drain Magnet, magnet
H6: Center Loop Post
H7: Knocker, knocker
H8: Top Diverter Post

L1: Left Sling, duty(SOL_DUTY_100), time(TIME_33MS)
L2: Right Sling, duty(SOL_DUTY_100), time(TIME_33MS)
L3: Bottom Jet, duty(SOL_DUTY_75), time(TIME_33MS)
L4: Middle Jet, duty(SOL_DUTY_75), time(TIME_33MS)
L5: Top Jet, duty(SOL_DUTY_75), time(TIME_33MS)
L6: Trap Door Hold
L7: Left Gate
L8: Right Gate

G1: Trunk Clockwise
G2: Trunk C. Clockwise
G3: Not Used
G4: Return Lane Flasher, flash
G5: Top Kickout
G6: Not Used
G7: Not Used
G8: Trap Door Flasher, flash

A1: Spirit Ring Flasher, flash
A2: Saw Flasher, flash
A3: Jet Flasher, flash
A4: Trunk Flasher, flash

F5: Cube Magnet
F6: Sub Ball Release
F7: Left Drain Magnet, magnet

[gi]

[tests]

#############################################################

[lamplists]
#Ramp Tickets: L. Ramp Ticket, R. Ramp Ticket

[containers]
#Trough: trough, Trough Release, Trough 5, Trough 4, Trough 3, Trough 2, Trough 1, init_max_count(5)
#Goal Popper: Goal Popper, Goal Popper

#############################################################

[scores]

[highscores]

[flags]

[globalflags]

[deffs]

[leffs]

[fonts]
pcsenior:
misctype:
lithograph:

[timers]

[templates]
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

