# TZ machine description

Title: Twilight Zone
DMD: Yes
Fliptronic: Yes
DCS: No
WPC95: No
Extra-Defines: MACHINE_TZ
Pinmame-Zip: tz_92.zip
Pinmame-ROM: tzone9_2.rom

[lamps]
11: Panel Camera, amber
12: Panel Hitchhiker, amber
13: Panel Clock Chaos
14: Panel Super Skill
15: Panel Fast Lock
16: Panel Gumball
17: Panel TSM
18: Panel EB
21: Lock 2
22: Panel Greed
23: Panel 10M
24: Panel Battle Power
25: Panel Spiral
26: Panel Clock Mil.
27: Panel Super Slot
28: BALL
31: Left Outlane
32: Lock 1
33: Left Inlane 1
34: LITZ
35: Left Inlane 2
36: GUM
37: Lower Left 5M
38: Dead End
41: Spiral 2M
42: Spiral 3M
43: Spiral 4M
44: Spiral 5M
45: Spiral 10M
51: Bonus X
52: Multiball
53: Super Skill
54: Left Powerball
55: Camera
56: Ramp Battle
57: Lock EB
61: Left Jet
62: Lower Jet
63: Right Jet
64: ML 5M
65: UL 5M
66: Right Outlane
67: Right Powerball
68: Right Spiral
82: Clock Millions
87: Buy-In Button, yellow, buyin
88: Start Button, yellow, start

[switches]
D1: Left Coin
D2: Center Coin
D3: Right Coin
D4: Fourth Coin
D5: Escape
D6: Down
D7: Up
D8: Enter
11: Right Inlane, micro
12: Right Outlane, micro
13: Start Button, start-button
14: Tilt, plumb-tilt
15: Right Trough
16: Center Trough
17: Left Trough
18: Outhole, drain
21: Slam Tilt
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
58: Slot Machine
61: Skill Bottom
62: Skill Center
63: Skill Top
64: Standup 4
65: Power Payoff
66: Standup 5
67: Standup 6
68: Standup 7
71: Autofire 1
72: Autofire 2
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
H1: Slot Machine
H2: Rocket Kicker
H3: Autofire
H4: Gumball Popper
H5: Right Ramp Divertor
H6: Gumball Divertor
H7: Knocker, knocker
H8: Outhole

L1: Ball Serve, ballserve
L2: Right Slingshot
L3: Left Slingshot
L4: Lower Jet
L5: Left Jet
L6: Right Jet
L7: Lock Release
L8: Shooter Divertor

G1: Jet Bumpers, flash
G2: Power Payoff, flash
G3: Powerfield, flash
G4: Ramp 1, flash
G5: Left Magnet
G6: Upper Right Magnet
G7: Right Magnet
G8: Gumball Release

X1: Upper Right Flipper, flash
X2: Gumball High, flash
X3: Gumball Middle, flash
X4: Gumball Low, flash
X5: Ramp 2, flash
X6: Clock Reverse, motor
X7: Clock Forward, motor

[gi]
0: Lower Left Playfield
1: Powerfield
2: Clock
3: Mystery
4: Lower Right Playfield

#############################################################

[lampsets]

[containers]
Trough: Ball Serve, Left Trough, Center Trough, Right Trough, init_max_count(3)
Lock: Lock Release, Lock Upper, Lock Center, Lock Lower
Rocket: Rocket Kicker, Rocket Kicker
Slot Kickout: Slot Machine, Slot Machine

[targetbanks]

[shots]
Left Loop Complete:
Left Loop Aborted:
Right Loop Complete:
Right Loop Aborted:
