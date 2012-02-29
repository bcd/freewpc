include platform/generic.md

[lamps]
11: First Lamp
88: Last Lamp

[switches]
D1: Left Coin, cabinet
D2: Center Coin, cabinet
D3: Right Coin, cabinet
D4: Fourth Coin, cabinet
D5: Escape, service, intest, cabinet, button
D6: Down, service, intest, cabinet, button
D7: Up, service, intest, cabinet, button
D8: Enter, service, intest, cabinet, button
22: Coin Door Closed, cabinet, edge, intest
24: Always Closed, virtual, c_decl(sw_unused)

F1: L. R. Flipper EOS, opto, cabinet
F2: L. R. Flipper Button, button, opto, intest, c_decl(sw_right_button)
F3: L. L. Flipper EOS, opto, cabinet
F4: L. L. Flipper Button, button, opto, intest, c_decl(sw_left_button)
F5: U. R. Flipper EOS, opto, cabinet
F6: U. R. Flipper Button, button, opto
F7: U. L. Flipper EOS, opto, cabinet
F8: U. L. Flipper Button, button, opto

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

F1: L.R. Flip Power, duty(SOL_DUTY_75), time(TIME_33MS)
F2: L.R. Flip Hold, duty(SOL_DUTY_75), time(TIME_200MS)
F3: L.L. Flip Power, duty(SOL_DUTY_75), time(TIME_33MS)
F4: L.L. Flip Hold, duty(SOL_DUTY_75), time(TIME_200MS)

[lamplists]
ALL: 11..88
Ball Save: MACHINE_BALL_SAVE_LAMP

[fonts]
mono5:
num5x7:
lucida9:
tinynum:
var5:
fixed6:
cu17:
fixed10:
term6:
times8:
bitmap8:

