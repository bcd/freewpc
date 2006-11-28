#--------------------------------------------------------------------------
# TZ machine description for FreeWPC
# (C) Copyright 2006 by Brian Dominy <brian@oddchange.com>
#
# See tools/genmachine for more information about the format of this file.
#--------------------------------------------------------------------------

Title: Twilight Zone
DMD: Yes
Fliptronic: Yes
DCS: No
WPC95: No
Extra-Defines: MACHINE_TZ, MACHINE_INCLUDE_FLAGS, MACHINE_CUSTOM_AMODE, MACHINE_SOL_EXTBOARD1
Pinmame-Zip: tz_92.zip
Pinmame-ROM: tzone9_2.rom

[lamps]
11: Panel Camera, amber
12: Panel H.H., amber
13: Panel Clock Chaos, amber
14: Panel Super Skill, amber
15: Panel Fast Lock, amber
16: Panel Gumball, amber
17: Panel TSM, amber
18: Panel EB, amber
21: Lock2, red
22: Panel Greed, amber
23: Panel 10M, amber
24: Panel Battle Power, amber
25: Panel Spiral, amber
26: Panel Clock Mil., amber
27: Panel Super Slot, amber
28: BALL, red
31: Left Outlane, red
32: Lock1
33: Left Inlane1, white
34: LITZ, white
35: Left Inlane2, white
36: GUM, red
37: LL 5M, yellow
38: Dead End, white
41: Spiral 2M, yellow
42: Spiral 3M, yellow
43: Spiral 4M, yellow
44: Spiral 5M, yellow
45: Spiral 10M, yellow
46: Spiral EB, orange
47: Shoot Again, red
48: Right Inlane, white
51: Bonus X, white
52: Multiball, white
53: Super Skill, orange
54: Left Powerball, white
55: Camera, white
56: Ramp Battle, orange
57: Lock EB
58: Lock Arrow, orange
61: Left Jet
62: Lower Jet
63: Right Jet
64: ML 5M, yellow
65: UL 5M, yellow
66: Right Outlane, red
67: Right Powerball, white
68: Right Spiral, yellow
71: LR 5M
72: MR1 5M
73: MR2 5M
74: Power Payoff
75: UR 5M
76: MPF 500K
77: MPF 750K
78: MPF 1M
81: Left Spiral, yellow
82: Clock Millions, white
83: Piano Panel
84: Piano Jackpot
85: Slot Machine
86: Gumball Lane
87: Buy-In Button, yellow, buyin
88: Start Button, yellow, start

[switches]
D1: Left Coin
D2: Center Coin
D3: Right Coin
D4: Fourth Coin
D5: Escape, service
D6: Down, service
D7: Up, service
D8: Enter, service
11: Right Inlane, micro
12: Right Outlane, micro
13: Start Button, start-button
14: Tilt, plumb-tilt
15: Right Trough
16: Center Trough
17: Left Trough
18: Outhole, outhole
21: Slam Tilt, slam-tilt
22: Coin Door Closed
23: Buyin Button, buyin-button
24: Always Closed, virtual, opto
25: Far Left Trough
26: Trough Proximity, opto
27: Shooter, manual
28: Rocket Kicker
31: Left Jet, fast, Left Jet
32: Right Jet, fast, Right Jet
33: Bottom Jet, fast, Bottom Jet
34: Left Sling, fast, Left Slingshot
35: Right Sling, fast, Right Slingshot
36: Left Outlane
37: Left Inlane 1
38: Left Inlane 2
41: Dead End
42: Camera
43: Piano
44: MPF Enter
45: MPF Left
46: MPF Right
47: Clock Target, standup
48: Greed 1
51: Gumball Lane
52: Hitchhiker
53: Left Ramp Enter
54: Left Ramp Exit
55: Gumball Geneva
56: Gumball Exit
57: Slot Proximity
58: Slot
61: Skill Bottom
62: Skill Center
63: Skill Top
64: Standup 4
65: Power Payoff
66: Standup 5
67: Standup 6
68: Standup 7
71: Autofire1
72: Autofire2
73: Right Ramp
74: Gumball Popper
75: MPF Top
76: MPF Exit
77: Standup 2
78: Standup 3
81: Lower Right Magnet
82: Upper Right Magnet
83: Left Magnet
84: Lock Center
85: Lock Upper
86: Clock Passage
87: Gumball Enter
88: Lock Lower
F1: Lower Right Flipper EOS
F2: Lower Right Flipper Button
F3: Lower Left Flipper EOS
F4: Lower Left Flipper Button
F5: Upper Right Flipper EOS
F6: Upper Right Flipper Button
F7: Upper Left Flipper EOS
F8: Upper Left Flipper Button

[drives]
H1: Slot
H2: Rocket Kicker
H3: Autofire
H4: Popper
H5: Right Ramp Div.
H6: Gumball Div.
H7: Knocker, knocker
H8: Outhole

L1: Ball Serve, ballserve
L2: Right Sling
L3: Left Sling
L4: Lower Jet
L5: Left Jet
L6: Right Jet
L7: Lock Release
L8: Shooter Div

G1: Jets, flash
G2: Ramp3 Power Payoff, flash
G3: Powerfield, flash
G4: Ramp1, flash
G5: Left Magnet
G6: Upper Right Magnet
G7: Right Magnet
G8: Gumball Release, motor, nosearch

A1: MPF Left Magnet
A2: MPF Right Magnet
A3: Ramp Divertor
A4: Clock Target, flash

X1: UR Flipper, flash
X2: Gumball High, flash
X3: Gumball Mid, flash
X4: Gumball Low, flash
X5: Ramp2, flash
X6: Clock Reverse, motor, nosearch
X7: Clock Forward, motor, nosearch

[gi]
0: Lower Left Playfield
1: Powerfield
2: Clock
3: Mystery
4: Lower Right Playfield

[tests]
clock:
gumball:

#############################################################

[lampsets]

[containers]
Trough: Ball Serve, Left Trough, Center Trough, Right Trough, trough, init_max_count(3)
Lock: Lock Release, Lock Upper, Lock Center, Lock Lower
Rocket: Rocket Kicker, Rocket Kicker
Slot Kickout: Slot, Slot

[targetbanks]

[shots]
Left Loop Complete:
Left Loop Aborted:
Right Loop Complete:
Right Loop Aborted:

#############################################################

[adjustments]

[audits]

