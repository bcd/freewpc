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
Extra-Defines: MACHINE_TZ, MACHINE_INCLUDE_FLAGS, MACHINE_SOL_EXTBOARD1
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
57: Lock EB, red
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
83: Piano Panel, yellow
84: Piano Jackpot, red
85: Slot Machine, yellow
86: Gumball Lane, red
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
14: Tilt, tilt
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
27: Shooter, shooter
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
48: Standup 1, standup
51: Gumball Lane
52: Hitchhiker
53: Left Ramp Enter
54: Left Ramp Exit
55: Gumball Geneva, opto
56: Gumball Exit
57: Slot Proximity
58: Slot
61: Skill Bottom
62: Skill Center
63: Skill Top
64: Standup 4, standup
65: Power Payoff
66: Standup 5, standup
67: Standup 6, standup
68: Standup 7, standup
71: Autofire1, opto
72: Autofire2, opto
73: Right Ramp, opto
74: Gumball Popper, opto
75: MPF Top, opto
76: MPF Exit, opto
77: Standup 2, standup
78: Standup 3, standup
81: Lower Right Magnet, opto
82: Upper Right Magnet, opto
83: Left Magnet, opto
84: Lock Center, opto
85: Lock Upper, opto
86: Clock Passage, opto
87: Gumball Enter, opto
88: Lock Lower
F1: Lower Right Flipper EOS
F2: Lower Right Flipper Button, button
F3: Lower Left Flipper EOS
F4: Lower Left Flipper Button, button
F5: Upper Right Flipper EOS
F6: Upper Right Flipper Button, button
F7: Upper Left Flipper EOS
F8: Upper Left Flipper Button, button

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

[drivers]
slingshot: poll(8)
jets: poll(8)
magnet_switch: poll(8)
tz_clock: poll(32)
magnet_duty: poll(32)

[tests]
tz_clock:
tz_gumball:

#############################################################

[lampsets]

[containers]
Trough: Ball Serve, Left Trough, Center Trough, Right Trough, trough, init_max_count(3)
Lock: Lock Release, Lock Upper, Lock Center, Lock Lower
Rocket: Rocket Kicker, Rocket Kicker
Slot: Slot, Slot

[targetbanks]

[shots]
Left Loop Complete:
Left Loop Aborted:
Right Loop Complete:
Right Loop Aborted:

#############################################################

[adjustments]
Ball Saves: integer, 1
Ball Save Time: integer, 7
Installed Balls: integer, 6
Disable Clock: yes_no, NO
Disable Gumball: yes_no, NO
Powerball Missing: yes_no, NO
Have Third Magnet: yes_no, NO

[audits]

[system_sounds]
Add Coin: SND_LIGHT_SLOT_TIMED
Add Credit: SND_THUD
Volume Change: MUS_SUPER_SLOT
Start Game: SND_ARE_YOU_READY_TO_BATTLE
Tilt Warning: SND_TILT_WARNING
Tilt: SND_TILT

[system_music]
Start Ball: MUS_MULTIBALL_LIT_PLUNGER
Ball in Play: MUS_MULTIBALL_LIT
End Game: MUS_POWERBALL_MANIA

[highscores]
GC: QQQ, 15.000.000
1: CCD, 10.000.000
2: MLD, 9.000.000
3: BDD, 8.000.000
4: NWU, 7.000.000

[flags]
DOOR_AWARD_LIT:
LEFT_OUTLANE_LIT:
RIGHT_OUTLANE_LIT:
QUICK_MULTIBALL_RUNNING:
BTTZ_RUNNING:
MULTIBALL_RUNNING:
BATTLE_THE_POWER_LIT:
POWERBALL_IN_PLAY:
SKILL_SHOT_LIT:
NO_DRAIN_PENALTY:
STEEL_IN_TROUGH:
STEEL_IN_TUNNEL:
PB_ALONE_IN_PLAY:

#endif /* _MACH_LAMP_H */

