#--------------------------------------------------------------------------
# AFM machine description for FreeWPC
# (C) Copyright 2006 by Brian Dominy <brian@oddchange.com>
#
# See tools/genmachine for more information about the format of this file.
#--------------------------------------------------------------------------

Title: Attack From Mars
DMD: Yes
Fliptronic: Yes
DCS: Yes
PIC: Yes
WPC95: Yes
Pinmame-Zip: afm_113b.zip
Pinmame-ROM: afm_113b.bin

include platform/wpc/wpc95.md

define MACHINE_NUMBER 541

[lamps]

[switches]
11: Launch Button, launch-button
13: Start Button, yellow, start-button, cabinet, intest
14: Tilt, tilt
16: Left Outlane
17: Right Inlane
18: Shooter, shooter
21: Slam Tilt, slam-tilt
26: Left Inlane
27: Right Outlane
31: Trough Eject, opto
32: Trough 1, trough, opto
33: Trough 2, trough, opto
34: Trough 3, trough, opto
35: Trough 4, trough, opto
36: Left Popper, opto
37: Right Popper, opto
38: Left Top Lane
41: Martian 6
42: Martian 7
43: Martian 4
44: Martian 5
45: Left Motor Bank
46: Center Motor Bank
47: Right Motor Bank
48: Right Top Lane
51: Left Slingshot
52: Right Slingshot
53: Left Jet
54: Bottom Jet
55: Right Jet
56: Martian 1
57: Martian 2
58: Martian 3
61: Left Ramp Enter
62: Center Ramp Enter
63: Right Ramp Enter
64: Left Ramp Exit
65: Right Ramp Exit
66: Motor Bank Down
67: Motor Bank Up
71: Right Loop High
72: Right Loop Low
73: Left Loop High
74: Left Loop Low
75: L. Saucer Tgt.
76: R. Saucer Tgt.

[drives]
H1: Auto Launch, launch
H2: Trough Eject, ballserve
H3: Left Popper
H4: Right Popper
H5: L.L. Martian
H6: U.L. Martian
H7: Knocker, knocker
H8: U.R. Martian

L1: Left Slingshot
L2: Right Slingshot
L3: Left Jet
L4: Bottom Jet
L5: Right Jet
L6: L.R. Martian
L7: Saucer Shake
L8: Drop Target

#Motor Bank
#Right Gate
#Left Gate
#Divertor Power
#Divertor Hold
#L.E.D. Clock
#L.E.D. Data
#Strobe Light
#Coin Meter

# TODO : Divertor Power = 34
# TODO : Divertor Hold = 35
# TODO : Strobe Light = 38

[gi]

[tests]

#############################################################

[lampsets]

[containers]
Trough: trough, Trough Eject, Trough 1, Trough 2, Trough 3, Trough 4, init_max_count(4)
Left Hole: Left Popper, Left Popper
Right Hole: Right Popper, Right Popper

[targetbanks]

[shots]

#############################################################

[adjustments]

[audits]

[system_sounds]

[system_music]

[scores]
1K:

[highscores]

[flags]

[deffs]

[leffs]

[fonts]

