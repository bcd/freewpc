#--------------------------------------------------------------------------
# Platform description for Pinball 2000
# (C) Copyright 2006-2010 by Brian Dominy <brian@oddchange.com>
#
# See tools/genmachine for more information about the format of this file.
#--------------------------------------------------------------------------

include platform/generic.md

[lamps]
11: First Lamp
88: Last Lamp

[switches]
D1: Left Coin, cabinet
D2: Center Coin, cabinet
D3: Right Coin, cabinet
D4: Fourth Coin, cabinet

F1: Slam Tilt
F2: Coin Door Closed
F3: Tilt
F5: Right Button, button, intest, c_decl(sw_right_button)
F6: Left Button, button, intest, c_decl(sw_left_button)
F7: Right Action, button
F8: Left Action, button

#D73: Escape, service, intest, cabinet, button
#D74: Down, service, intest, cabinet, button
#D75: Up, service, intest, cabinet, button
#D76: Enter, service, intest, cabinet, button
#D77: L.R. EOS
#D78: L.L. EOS
#D79: U.R. EOS
#D80: U.L. EOS

[drives]
H1: High Power Sol. 1, notinstalled
H2: High Power Sol. 2, notinstalled
H3: High Power Sol. 3, notinstalled
H4: High Power Sol. 4, notinstalled
H5: High Power Sol. 5, notinstalled
H6: High Power Sol. 6, notinstalled
H7: High Power Sol. 7, notinstalled
H8: High Power Sol. 8, notinstalled

L1: Low Power Sol. 1, notinstalled
L2: Low Power Sol. 2, notinstalled
L3: Low Power Sol. 3, notinstalled
L4: Low Power Sol. 4, notinstalled
L5: Low Power Sol. 5, notinstalled
L6: Low Power Sol. 6, notinstalled
L7: Low Power Sol. 7, notinstalled
L8: Low Power Sol. 8, notinstalled

G1: General Sol. 1, notinstalled
G2: General Sol. 2, notinstalled
G3: General Sol. 3, notinstalled
G4: General Sol. 4, notinstalled
G5: General Sol. 5, notinstalled
G6: General Sol. 6, notinstalled
G7: General Sol. 7, notinstalled
G8: General Sol. 8, notinstalled

A1: Aux. Sol. 1, notinstalled
A2: Aux. Sol. 2, notinstalled
A3: Aux. Sol. 3, notinstalled
A4: Aux. Sol. 4, notinstalled

F1: L.R. Flip Power, time(TIME_33MS)
F2: L.R. Flip Hold, time(TIME_100MS)
F3: L.L. Flip Power, time(TIME_33MS)
F4: L.L. Flip Hold, time(TIME_200MS)
F5: U.R. Flip Power, time(TIME_33MS)
F6: U.R. Flip Hold, time(TIME_100MS)
F7: U.L. Flip Power, time(TIME_33MS)
F8: U.L. Flip Hold, time(TIME_100MS)

[lamplists]
ALL: 11..88
Ball Save: %ball-save


