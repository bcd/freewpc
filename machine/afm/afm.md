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
13: Start Button, start-button
14: Tilt, tilt
18: Shooter, shooter
21: Slam Tilt, slam-tilt
31: Trough Eject, opto
32: Trough 1, trough, opto
33: Trough 2, trough, opto
34: Trough 3, trough, opto
35: Trough 4, trough, opto
36: Left Popper, opto
37: Right Popper, opto
85: L. Saucer Target
86: R. Saucer Target
87: Drop Target
88: Center Trough

[drives]
H1: Auto Launch, launch
H2: Trough Eject, ballserve
H3: Left Popper
H4: Right Popper
H7: Knocker, knocker

L6: Saucer Shake
L7: Drop Target

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

