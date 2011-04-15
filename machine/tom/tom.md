#--------------------------------------------------------------------------
# Theatre of Magic machine description for FreeWPC
# (C) Copyright 2011 by Brian Dominy <brian@oddchange.com>
#
# See tools/genmachine for more information about the format of this file.
#--------------------------------------------------------------------------

Title: Theatre Of Magic

include platform/wpc/wpc-security.md

Pinmame-Zip: tom_13.zip
Pinmame-ROM: tom1_3x.rom
Lamp-Matrix-Width: 45
Lamp-Matrix-Height: 25

define MACHINE_TOM
define MACHINE_NUMBER 539
#define MACHINE_CUSTOM_AMODE
define MACHINE_GRAND_CHAMPION_INITIALS { 'T', 'O', 'M' }
define MACHINE_GRAND_CHAMPION_SCORE { 0x00, 0x20, 0x00, 0x00, 0x00 }
define MACHINE_HIGH_SCORE_INITIALS { 'J', ' ', 'P' }, { 'B', 'C', 'D' }, { 'C', 'O', 'P' }, { 'H', 'A', 'T' }
define MACHINE_HIGH_SCORES { 0x00, 0x40, 0x00, 0x00, 0x00 }, { 0x00, 0x35, 0x00, 0x00, 0x00 }, { 0x00, 0x30, 0x50, 0x00, 0x00 }, { 0x00, 0x25, 0x00, 0x00, 0x00 }
#define MACHINE_START_BALL_MUSIC   MUS_MAIN_PLUNGER
#define MACHINE_BALL_IN_PLAY_MUSIC MUS_MAIN
define MACHINE_LACKS_ALL_INCLUDES

[lamps]
11: Theatre 1
12: Theatre 2
13: Theatre 3
14: Theatre 4
15: Theatre 5
16: Theatre 6
17: Theatre 7
18: Magic 1
21: Basement
22: Metamorphosis Award
23: Right Magic
24: Spirit Ring
25: Advance Clock
26: Jacket Award
27: Magic 2
28: Trunk 3
31: Magic 3
32: Magic 5
33: Open Trap Door
34: Center Magic
35: Levitate Award
36: Magic 4
37: Rollover 1
38: Rollover 2
41: Extra Ball
42: Vanish
43: Spell Theatre
44: Jackpot
45: Safe Award
46: Tiger Saw Award
47: Start Finale
48: Trunk 1
51: Trunk 2
52: Hurry Up
53: Escape Award
54: Lock Ball
55: Hat Trick Award
56: Start Illusion
57: Start Multiball
58: Light Vanish
61: Tiger Saw
62: Levitate Woman
63: Grand Finale
64: Trunk Escape
65: Spirit Cards
66: Safe Escape
67: Metamorphosis
68: Strait Jacket
71: Hat Magic
72: Spirit Award
73: Theatre
74: Multiball
75: Midnight
76: Illusions
77: Saw Multiball
78: Hocus Pocus
81: Special
85: Trunk Lamp
86: Shoot Again, shoot-again
87: Buyin, yellow, buyin, cabinet
88: Start Button, yellow, start, cabinet

[switches]
11: Unused
12: Magnet Button, button, noscore, cabinet
13: Start Button, start-button, cabinet, intest
14: Tilt, cabinet, tilt, ingame, noplay
15: Shooter Lane, edge, shooter, noscore, debounce(TIME_200MS)
25: Left Outlane
26: Left Inlane
27: Right Inlane
28: Right Outlane
31: Trough Jam, opto
32: Trough 1, opto, noscore
33: Trough 2, opto, noscore
34: Trough 3, opto, noscore
35: Trough 4, opto, noscore
36: Subway Opto, opto
37: Spinner
38: Right Lower Target, ingame
41: Lock 1
42: Lock 2
43: Lock 3
44: Popper, edge
45: Left Drain Eddy
47: Subway Micro
48: Right Drain Eddy
51: Left Bank
52: Captive Ball Rest
53: Right Lane Enter
54: Left Lane Enter
55: Cube Pos. 4, opto
56: Cube Pos. 1, opto
57: Cube Pos. 2, opto
58: Cube Pos. 3, opto
61: Left Sling, ingame, noplay
62: Right Sling, ingame, noplay
63: Bottom Jet, ingame, noplay
64: Middle Jet, ingame, noplay
65: Top Jet, ingame, noplay
66: Top Lane 1
67: Top Lane 2
71: Center Ramp Exit
73: Right Ramp Exit 1
74: Right Ramp Exit 2
75: Center Ramp Enter
76: Right Ramp Enter
77: Captive Ball Top
78: Loop Left
82: Ramp Targets, ingame
83: Vanish Lock 1
84: Vanish Lock 2
85: Trunk Hit
86: Right Lane Exit
87: Left Lane Exit

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
G3: Not Used, notinstalled
G4: Return Lane Flasher, flash
G5: Top Kickout
G6: Not Used, notinstalled
G7: Not Used, notinstalled
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
Theatre Spell: Theatre 1, Theatre 2, Theatre 3, Theatre 4, Theatre 5, Theatre 6, Theatre 7
Magic Spell: Magic 1, Magic 2, Magic 3, Magic 4, Magic 5
Modes: Tiger Saw, Levitate Woman, Trunk Escape, Spirit Cards, Safe Escape, Metamorphosis, Strait Jacket, Hat Magic
Finale Tasks: Theatre, Multiball, Midnight, Illusions
Rollovers: Rollover 1, Rollover 2

[containers]
Trough: trough, Ball Trough, Trough 4, Trough 3, Trough 2, Trough 1, init_max_count(4)
Subway: Sub Ball Release, Lock 3, Lock 2, Lock 1, init_max_count(0)
Vanish: Top Kickout, Vanish Lock 2, Vanish Lock 1, init_max_count(0)

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
Left Sling: driver(sling), sw=SW_LEFT_SLING, sol=SOL_LEFT_SLING
Right Sling: driver(sling), sw=SW_RIGHT_SLING, sol=SOL_RIGHT_SLING
Top Jet: driver(jet), sw=SW_TOP_JET, sol=SOL_TOP_JET
Middle Jet: driver(jet), sw=SW_MIDDLE_JET, sol=SOL_MIDDLE_JET
Bottom Jet: driver(jet), sw=SW_BOTTOM_JET, sol=SOL_BOTTOM_JET

#Spinner: driver(spinner), sw_event=sw_spinner, sw_number=SW_SPINNER
#Left Gate: driver(duty), sol=SOL_LOOP_GATE, ontime=TIME_300MS, duty_ontime=TIME_33MS, duty_offtime=TIME_16MS, timeout=60
#Right Gate:
#Trunk Divert:
#Vanish Divert:
#Hocus Pocus:
#Ramp Magnet:
#Trunk Magnet:
#Trunk Motor:
