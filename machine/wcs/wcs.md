#--------------------------------------------------------------------------
# WCS machine description for FreeWPC
# (C) Copyright 2006 by Brian Dominy <brian@oddchange.com>
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

include platform/wpc/wpc.md

define MACHINE_NUMBER 531

[lamps]

[switches]
13: Start Button, start-button, cabinet, intest
14: Tilt, tilt
21: Slam Tilt, slam-tilt
31: Trough 1, trough, opto
32: Trough 2, trough, opto
33: Trough 3, trough, opto
34: Trough 4, trough, opto
35: Trough 5, trough, opto
36: Trough Stack, opto
38: Shooter, shooter
41: Goal Trough, opto
42: Goal Popper, opto
43: Goalie Left, opto
44: Goalie Right, opto
45: TV Popper, opto
48: Goalie Target
51: Skill Shot Front, opto
52: Skill Shot Center, opto
53: Skill Shot Rear, opto
54: Right Eject
55: Upper Eject
56: Left Eject
71: Left Ramp Divert.
72: Left Ramp Enter
74: Left Ramp Exit
75: Right Ramp Enter
76: Lock Low
77: Lock High
78: Right Ramp Exit

[drives]
H1: Goal Popper
H2: TV Popper
H3: Kickback
H4: Lock Release
H5: Upper Eject
H6: Trough Release, ballserve
H7: Knocker, knocker
H8: Ramp Divertor

L1: Left Jet
L2: Upper Jet
L3: Lower Jet
L4: Left Sling
L5: Right Sling
L6: Right Eject
L7: Left Eject
L8: Divertor Hold

[gi]

[tests]

#############################################################

[lampsets]

[containers]
Trough: trough, Trough Release, Trough 1, Trough 2, Trough 3, Trough 4, Trough 5, init_max_count(5)
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

[audits]

[system_sounds]

[system_music]

[scores]

[highscores]

[flags]

[deffs]

[leffs]

[fonts]

