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
Pinmame-Zip: tz_92.zip
Pinmame-ROM: tzone9_2.rom
Lamp-Matrix-Width: 39
Lamp-Matrix-Height: 29

include kernel/freewpc.md

define MACHINE_TZ
define MACHINE_INCLUDE_FLAGS
define MACHINE_SOL_EXTBOARD1
define MACHINE_AMODE_LEFT_FLIPPER_HANDLER amode_left_flipper
define MACHINE_AMODE_RIGHT_FLIPPER_HANDLER amode_right_flipper
define MACHINE_CUSTOM_AMODE


[lamps]
11: Panel Camera, amber ,x(28), y(13)
12: Panel H.H., amber ,x(28), y(10)
13: Panel Clock Chaos, amber ,x(26), y(10)
14: Panel Super Skill, amber ,x(24), y(10)
15: Panel Fast Lock, amber ,x(22), y(10)
16: Panel Gumball, amber ,x(20), y(10)
17: Panel TSM, amber ,x(18), y(10)
18: Panel EB, amber ,x(18), y(13)
21: Lock2, red ,x(25), y(14)
22: Panel Greed, amber ,x(28), y(16)
23: Panel 10M, amber ,x(26), y(16)
24: Panel Battle Power, amber ,x(24), y(16)
25: Panel Spiral, amber ,x(22), y(16)
26: Panel Clock Mil., amber ,x(20), y(16)
27: Panel Super Slot, amber ,x(18), y(16)
28: BALL, red ,x(21), y(14)
31: Left Outlane, red ,x(27), y( 1)
32: Lock1 ,x(25), y(12)
33: Left Inlane1, white ,x(26), y( 3)
34: Panel LITZ, white ,x(23), y(12)
35: Left Inlane2, white ,x(26), y( 5)
36: GUM, red ,x(21), y(12)
37: LL 5M, yellow ,x(23), y( 4)
38: Dead End, white ,x(20), y( 5)
41: Spiral 2M, yellow ,x(32), y( 8)
42: Spiral 3M, yellow ,x(31), y(10)
43: Spiral 4M, yellow ,x(30), y(12)
44: Spiral 5M, yellow ,x(30), y(14)
45: Spiral 10M, yellow ,x(31), y(16)
46: Spiral EB, orange ,x(32), y(18)
47: Shoot Again, red ,x(36), y(13)
48: Right Inlane, white ,x(26), y(21)
51: Bonus X, white ,x( 5), y(11)
52: Multiball, white ,x( 7), y(12)
53: Super Skill, orange ,x( 9), y(13)
54: Left Powerball, white ,x( 6), y( 9)
55: Camera, white ,x(14), y(11)
56: Ramp Battle, orange ,x( 6), y(17)
57: Lock EB, red ,x( 3), y(19)
58: Lock Arrow, orange ,x( 1), y(20)
61: Left Jet ,x(17), y( 1)
62: Lower Jet ,x(22), y( 3)
63: Right Jet ,x(17), y( 6)
64: ML 5M, yellow ,x(19), y( 7)
65: UL 5M, yellow ,x(17), y( 8)
66: Right Outlane, red ,x(27), y(23)
67: Right Powerball, white ,x(14), y(22)
68: Right Spiral, yellow ,x(12), y(23)
71: LR 5M ,x(16), y(17)
72: MR1 5M ,x(14), y(16)
73: MR2 5M ,x(12), y(16)
74: Power Payoff ,x(10), y(18)
75: UR 5M ,x( 5), y(20)
76: MPF 500K ,x( 9), y( 3)
77: MPF 750K ,x( 7), y( 2)
78: MPF 1M ,x( 7), y( 4)
81: Left Spiral, yellow ,x( 5), y( 6)
82: Clock Millions, white ,x( 5), y(15)
83: Piano Panel, yellow ,x( 8), y(21)
84: Piano Jackpot, red ,x( 7), y(21)
85: Slot Machine, yellow ,x(14), y(19)
86: Gumball Lane, red ,x(15), y(23)
87: Buy-In Button, yellow, buyin, cabinet, x(38), y(27)
88: Start Button, yellow, start, cabinet, x(38), y( 1)

# Mark playfield flags! (or opposite?!)
[switches]
11: Right Inlane, micro, sound(SND_INSIDE_LEFT_INLANE), lamp(LM_RIGHT_INLANE)
12: Right Outlane, micro, sound(SND_DRAIN)
13: Start Button, start-button, intest
14: Tilt, tilt, ingame
15: Right Trough, c_decl(sw_trough), noplay
16: Center Trough, c_decl(sw_trough), noplay
17: Left Trough, c_decl(sw_trough), noplay
18: Outhole, outhole, intest, noplay
21: Slam Tilt, slam-tilt, ingame, cabinet
23: Buyin Button, buyin-button
25: Far Left Trough, noplay
26: Trough Proximity, noplay, ingame
27: Shooter, edge, shooter, noplay
28: Rocket Kicker
31: Left Jet, ingame, c_decl(sw_jet), fast, Left Jet, noplay
32: Right Jet, ingame, c_decl(sw_jet), fast, Right Jet, noplay
33: Bottom Jet, ingame, c_decl(sw_jet), fast, Bottom Jet, noplay
34: Left Sling, ingame, c_decl(sw_sling), fast, Left Slingshot, noplay
35: Right Sling, ingame, c_decl(sw_sling), fast, Right Slingshot, noplay
36: Left Outlane, sound(SND_DRAIN)
37: Left Inlane 1, sound(SND_INSIDE_LEFT_INLANE), lamp(LM_LEFT_INLANE1)
38: Left Inlane 2, sound(SND_INSIDE_LEFT_INLANE), lamp(LM_LEFT_INLANE2)
41: Dead End, ingame
42: Camera, ingame
43: Piano
44: MPF Enter, ingame
45: MPF Left, ingame
46: MPF Right, ingame
47: Clock Target, standup, ingame, lamp(LM_CLOCK_MILLIONS)
48: Standup 1, standup, ingame, lamp(LM_LL_5M)
51: Gumball Lane, intest
52: Hitchhiker, ingame, sound(SND_HITCHHIKER_DRIVE_BY)
53: Left Ramp Enter, ingame, sound(SND_LEFT_RAMP_ENTER)
54: Left Ramp Exit, ingame, sound(SND_LEFT_RAMP_MADE)
55: Gumball Geneva, opto, intest
56: Gumball Exit, intest
57: Slot Proximity, noplay, ingame
58: Slot
61: Skill Bottom, ingame, noplay
62: Skill Center, ingame, noplay
63: Skill Top, ingame, noplay
64: Standup 4, standup, ingame, lamp(LM_UR_5M)
65: Power Payoff, c_decl(sw_unused)
66: Standup 5, standup, ingame, lamp(LM_MR1_5M)
67: Standup 6, standup, ingame, lamp(LM_MR2_5M)
68: Standup 7, standup, ingame, lamp(LM_LR_5M)
71: Autofire1, opto, noplay, ingame
72: Autofire2, opto, noplay, ingame
73: Right Ramp, opto, ingame
74: Gumball Popper, opto, intest
75: MPF Top, opto, ingame
76: MPF Exit, opto, ingame
77: Standup 2, standup, ingame, lamp(LM_ML_5M)
78: Standup 3, standup, ingame, lamp(LM_UL_5M)
81: Lower Right Magnet, opto, intest
82: Upper Right Magnet, opto, intest
83: Left Magnet, opto, intest
84: Lock Center, opto, c_decl(sw_lock)
85: Lock Upper, opto, c_decl(sw_lock)
86: Clock Passage, opto, c_decl(sw_unused)
87: Gumball Enter, opto, intest
88: Lock Lower, c_decl(sw_lock)
F5: U. R. Flipper EOS, opto, cabinet
F6: U. R. Flipper Button, button, opto
F7: U. L. Flipper EOS, opto, cabinet
F8: U. L. Flipper Button, button, opto

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
tz_magnet:

#############################################################

[lampsets]
Door Panels: Panel TSM..Panel EB, Panel Super Slot..Panel Greed, Panel Camera..Panel Gumball
Door Panels and Handle: Door Panels, Panel LITZ
Door Locks: Lock1, Lock2
Door Gumball: GUM, BALL
Door Locks and Gumball: Door Locks, Door Gumball
Left Ramp Awards: Bonus X, Multiball, Super Skill
Lock Awards: Lock Arrow, Lock EB
Piano Awards: Piano Panel, Piano Jackpot
Greed Targets: LL 5M, ML 5M, UL 5M, UR 5M, MR2 5M, MR1 5M, LR 5M
Powerfield Values: MPF 500K..MPF 1M
Jets: Left Jet..Right Jet
Lower Lanes: Left Outlane, Left Inlane1, Left Inlane2, Right Inlane, Right Outlane
Loop Awards: Left Spiral, Left Powerball, Right Spiral, Right Powerball
Misc: Slot Machine, Ramp Battle, Clock Millions, Shoot Again
Spiral Awards: Spiral 2M..Spiral EB
Amode All: Door Panels and Handle, Door Locks, Door Gumball, Spiral Awards, Lower Lanes, Jets, Powerfield Values, Left Spiral, Left Powerball, Left Ramp Awards, Ramp Battle, Lock Awards, Piano Awards, Right Spiral, Right Powerball, Greed Targets, Dead End, Slot Machine, Camera
Amode Rand: Amode All
Sort1: PF:lamp_sort_bottom_to_top
Sort2: PF:lamp_sort_top_to_bottom
Sort3: PF:lamp_sort_left_to_right
Sort4: PF:lamp_sort_right_to_left
Circle Out: PF:lamp_sort_circle_out

[containers]
Trough: Ball Serve, trough, init_max_count(3), \
	Left Trough, Center Trough, Right Trough

Lock: Lock Release, \
	Lock Upper, Lock Center, Lock Lower

Rocket: Rocket Kicker, \
	Rocket Kicker

Slot: Slot, \
	Slot

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
3 Panels:
6 Panels:
9 Panels:
12 Panels:

[system_sounds]
Add Coin: SND_LIGHT_SLOT_TIMED
Add Credit: SND_THUD
Start Game: SND_ARE_YOU_READY_TO_BATTLE
Tilt Warning: SND_TILT_WARNING
Tilt: SND_TILT

[system_music]
Start Ball: MUS_MULTIBALL_LIT_PLUNGER
Ball in Play: MUS_MULTIBALL_LIT
End Game: MUS_POWERBALL_MANIA
Volume Change: MUS_SUPER_SLOT

[scores]
10:
100:
500:
1K:
2500:
5K:
5130:
10K:
15K:
20K:
25K:
30K:
40K:
50K:
75K:
100K:
200K:
250K:
500K:
750K:
1M:
2M:
3M:
4M:
5M:
10M:
20M:
30M:
40M:
50M:

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
QUICK_MB_RUNNING:
BTTZ_RUNNING:
MULTIBALL_RUNNING:
BATTLE_THE_POWER_LIT:
POWERBALL_IN_PLAY:
SKILL_SHOT_LIT:
NO_DRAIN_PENALTY:
STEEL_IN_TROUGH:
STEEL_IN_TUNNEL:
PB_ALONE_IN_PLAY:

[deffs]
Amode: runner, PRI_AMODE
Bonus: runner, PRI_BONUS
Brian Image: PRI_EGG1
REPLAY: PRI_REPLAY
JACKPOT: PRI_JACKPOT
SPECIAL: PRI_SPECIAL
Extra Ball: PRI_EB
Left Ramp: PRI_GAME_MODE1+1
Lock Lit: PRI_GAME_MODE1+2
MB Lit: PRI_GAME_MODE1+2
Ball Save: PRI_GAME_MODE1+3
HITCHHIKER: PRI_GAME_MODE1+4
CAMERA_AWARD: PRI_GAME_MODE1+5
DOOR_AWARD: PRI_GAME_MODE1+5
MB Start: PRI_GAME_QUICK1+8
SKILL_SHOT_MADE: PRI_GAME_MODE1+6
MB Running: runner, PRI_GAME_MODE1+8
PB Detect: PRI_GAME_QUICK1+12

[leffs]
FLASHER_HAPPY: PRI_LEFF1
GUMBALL_STROBE: PRI_LEFF1
LEFT_RAMP: PRI_LEFF2
CLOCK_TARGET: PRI_LEFF2, GI(ALL)
NO_GI: L_NORMAL, PRI_LEFF3, GI(ALL)
SLOT_KICKOUT: PRI_LEFF3
AMODE: runner, PRI_LEFF5, LAMPS(AMODE_ALL), GI(ALL)
FLASH_ALL: PRI_LEFF5, LAMPS(AMODE_ALL)
BONUS: runner, PRI_BONUS, LAMPS(ALL), GI(ALL)
Jets Active: shared, 0, LAMPS(JETS)
GAME_TIMEOUT: PRI_TILT, GI(ALL)
CLOCK_START: PRI_LEFF4, GI(ALL), c_decl(clock_round_started_leff)
MB_RUNNING: shared, PRI_LEFF2, LAMPS(DOOR_LOCKS_AND_GUMBALL), c_decl(multiball_running_leff)
Strobe Up: PRI_LEFF2, LAMPS(ALL)

[fonts]
mono5:
mono9:
fixed10:
fixed6:
lucida9:
term6:
times8:
times10:
helv8:
schu:
misctype:
utopia:
fixed12:
var5:
cu17:

