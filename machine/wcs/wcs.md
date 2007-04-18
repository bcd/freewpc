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

include kernel/freewpc.md

[lamps]

[switches]
13: Start Button, start-button
14: Tilt, tilt
21: Slam Tilt, slam-tilt
31: Trough 1, trough, opto
32: Trough 2, trough, opto
33: Trough 3, trough, opto
34: Trough 4, trough, opto
35: Trough 5, trough, opto
36: Trough Stack
38: Shooter, shooter
41: Goal Trough, opto
42: Goal Popper, opto
43: Goalie Left, opto
44: Goalie Right, opto
45: TV Popper, opto

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

[drivers]

[tests]

#############################################################

[lampsets]

[containers]
Trough: trough, Trough Release, Trough 1, Trough 2, Trough 3, Trough 4, Trough 5
Goal Popper: Goal Popper, Goal Popper
TV Popper: TV Popper, TV Popper

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

