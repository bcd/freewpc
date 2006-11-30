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

define MACHINE_TZ
define MACHINE_INCLUDE_FLAGS
define MACHINE_SOL_EXTBOARD1
define MACHINE_AMODE_LEFT_FLIPPER_HANDLER amode_left_flipper
define MACHINE_AMODE_RIGHT_FLIPPER_HANDLER amode_right_flipper
define MACHINE_CUSTOM_AMODE

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
34: Panel LITZ, white
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
15: Right Trough, c_decl(sw_trough)
16: Center Trough, c_decl(sw_trough)
17: Left Trough, c_decl(sw_trough)
18: Outhole, outhole
21: Slam Tilt, slam-tilt
22: Coin Door Closed
23: Buyin Button, buyin-button
24: Always Closed, virtual, opto, c_decl(sw_unused)
25: Far Left Trough
26: Trough Proximity, opto
27: Shooter, shooter, c_decl(sw_unused)
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
61: Skill Bottom, c_decl(sw_skill
62: Skill Center
63: Skill Top
64: Standup 4, standup
65: Power Payoff, c_decl(sw_unused)
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
84: Lock Center, opto, c_decl(sw_lock)
85: Lock Upper, opto, c_decl(sw_lock)
86: Clock Passage, opto, c_decl(sw_unused)
87: Gumball Enter, opto
88: Lock Lower, c_decl(sw_lock)
F1: L. R. Flipper EOS, opto
F2: L. R. Flipper Button, button, opto
F3: L. L. Flipper EOS, opto
F4: L. L. Flipper Button, button, opto
F5: U. R. Flipper EOS, opto
F6: U. R. Flipper Button, button, opto
F7: U. L. Flipper EOS, opto
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

#############################################################

[lampsets]
ALL: 11..88
Door Panels: Panel TSM..Panel EB, Panel Super Slot..Panel Greed, Panel Camera..Panel Gumball
Door Panels and Handle:
Door Locks:
Door Gumball:
Door Locks and Gumball:
Left Ramp Awards:
Lock Awards:
Piano Awards:
Greed Targets:
Powerfield Values:
Jets:
Lower Lanes:
Spiral Awards:
Amode All:
Amode Rand:

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
NULL: 0
Scores: runner, PRI_SCORES
Coin Insert: PRI_COINS
Credits: PRI_CREDITS
Tilt Warning: PRI_TILT_WARNING
Tilt: runner, PRI_TILT
Game Over: PRI_GAME_OVER
Volume Change: runner, PRI_VOLUME_CHANGE
Slam Tilt: runner, PRI_SLAMTILT
Status Report: runner, PRI_STATUS
Nonfatal Error: PRI_DEBUGGER
HSEntry: runner, PRI_HSENTRY
Match: runner, PRI_MATCH
Locating Balls: PRI_BALL_SEARCH
Buyin Offer: PRI_MATCH

Inspector: runner, PRI_DEBUGGER
Amode: runner, PRI_AMODE
Bonus: runner, PRI_BONUS
BRIAN_IMAGE: PRI_EGG1
REPLAY: PRI_REPLAY
JACKPOT: PRI_JACKPOT
SPECIAL: PRI_SPECIAL
EXTRA_BALL: PRI_EB
LEFT_RAMP: PRI_GAME_MODE1+1
LOCK_LIT: PRI_GAME_MODE1+2
MB_LIT: PRI_GAME_MODE1+2
BALL_SAVE: PRI_GAME_MODE1+3
HITCHHIKER: PRI_GAME_MODE1+4
CAMERA_AWARD: PRI_GAME_MODE1+5
DOOR_AWARD: PRI_GAME_MODE1+5
MB_START: PRI_GAME_MODE1+7
SKILL_SHOT_MADE: PRI_GAME_MODE1+8
MB_RUNNING: PRI_GAME_MODE1+2

[leffs]
#FLASHER_HAPPY: PRI_LEFF1
#GUMBALL_STROBE: PRI_LEFF1
#LEFT_RAMP: PRI_LEFF2
#CLOCK_TARGET: PRI_LEFF2, GI(ALL)
#NO_GI: L_NORMAL: PRI_LEFF3, GI(ALL)
#SLOT_KICKOUT: PRI_LEFF3
#AMODE: L_RUNNING, PRI_LEFF5, LAMPS(AMODE_ALL)
#FLASH_ALL: PRI_LEFF5, LAMPS(AMODE_ALL)
#BONUS: L_RUNNING, PRI_BONUS, LAMPS(ALL), GI(ALL)
#TILT_WARNING: runner, PRI_TILT_WARNING, LAMPS(ALL)
#TILT: runner, PRI_TILT, LAMPS(ALL), GI(ALL)
#JETS_ACTIVE: shared, 0, LAMPS(JETS)
#GAME_TIMEOUT: PRI_TILT, GI(ALL)
#CLOCK_START: PRI_LEFF4, GI(ALL)
#MB_RUNNING, shared, PRI_LEFF2, LAMPS(DOOR_LOCKS_AND_GUMBALL)


###################################################################

define MACHINE_LAMP_EFFECTS \
DECL_LEFF (LEFF_FLASHER_HAPPY, L_NORMAL, PRI_LEFF1, L_NOLAMPS, L_NOGI, flasher_happy_leff) \
DECL_LEFF (LEFF_GUMBALL_STROBE, L_NORMAL, PRI_LEFF1, L_NOLAMPS, L_NOGI, gumball_strobe_leff) \
DECL_LEFF (LEFF_LEFT_RAMP, L_NORMAL, PRI_LEFF2, L_NOLAMPS, L_NOGI, left_ramp_leff) \
DECL_LEFF (LEFF_CLOCK_TARGET, L_NORMAL, PRI_LEFF2, L_NOLAMPS, L_ALL_GI, clock_target_leff) \
DECL_LEFF (LEFF_NO_GI, L_NORMAL, PRI_LEFF3, L_NOLAMPS, L_ALL_GI, no_gi_leff) \
DECL_LEFF (LEFF_SLOT_KICKOUT, L_NORMAL, PRI_LEFF3, L_NOLAMPS, L_NOGI, slot_kickout_leff) \
DECL_LEFF (LEFF_AMODE, L_RUNNING, PRI_LEFF5, LAMPSET_AMODE_ALL, L_NOGI, amode_leff) \
DECL_LEFF (LEFF_FLASH_ALL, L_NORMAL, PRI_LEFF5, LAMPSET_AMODE_ALL, L_NOGI, flash_all_leff) \
DECL_LEFF (LEFF_BONUS, L_RUNNING, PRI_BONUS, L_ALL_LAMPS, L_ALL_GI, bonus_leff) \
DECL_LEFF (LEFF_TILT_WARNING, L_RUNNING, PRI_TILT_WARNING, L_ALL_LAMPS, L_NOGI, no_lights_leff) \
DECL_LEFF (LEFF_TILT, L_RUNNING, PRI_TILT, L_ALL_LAMPS, L_ALL_GI, no_lights_leff) \
DECL_LEFF (LEFF_JETS_ACTIVE, L_SHARED, 0, LAMPSET_JETS, L_NOGI, leff_exit) \
DECL_LEFF (LEFF_GAME_TIMEOUT, L_NORMAL, PRI_TILT, L_NOLAMPS, L_ALL_GI, game_timeout_leff) \
DECL_LEFF (LEFF_CLOCK_START, L_NORMAL, PRI_LEFF4, L_NOLAMPS, L_ALL_GI, clock_round_started_leff) \
DECL_LEFF (LEFF_MB_RUNNING, L_SHARED, PRI_LEFF2, LAMPSET_DOOR_LOCKS_AND_GUMBALL, L_NOGI, multiball_running_leff)

define MACHINE_LAMPSETS \
	DECL_LAMPSET(LAMPSET_DOOR_PANELS, LMSET_DOOR_PANEL_AWARDS) \
	DECL_LAMPSET(LAMPSET_DOOR_PANELS_AND_HANDLE, LMSET_DOOR_PANELS_AND_HANDLE) \
	DECL_LAMPSET(LAMPSET_DOOR_LOCKS, LMSET_DOOR_LOCKS) \
	DECL_LAMPSET(LAMPSET_DOOR_GUMBALL, LMSET_DOOR_GUMBALL) \
	DECL_LAMPSET(LAMPSET_DOOR_LOCKS_AND_GUMBALL, LMSET_DOOR_LOCKS_AND_GUMBALL) \
	DECL_LAMPSET(LAMPSET_LEFT_RAMP_AWARDS, LMSET_LEFT_RAMP_AWARDS) \
	DECL_LAMPSET(LAMPSET_LOCK_AWARDS, LMSET_LOCK_AWARDS) \
	DECL_LAMPSET(LAMPSET_PIANO_AWARDS, LMSET_PIANO_AWARDS) \
	DECL_LAMPSET(LAMPSET_GREED_TARGETS, LMSET_GREED_TARGETS) \
	DECL_LAMPSET(LAMPSET_POWERFIELD_VALUES, LMSET_POWERFIELD_VALUES) \
	DECL_LAMPSET(LAMPSET_JETS, LMSET_JETS) \
	DECL_LAMPSET(LAMPSET_LOWER_LANES, LMSET_LOWER_LANES) \
	DECL_LAMPSET(LAMPSET_SPIRAL_AWARDS, LMSET_SPIRAL_AWARDS) \
	DECL_LAMPSET(LAMPSET_AMODE_ALL, LMSET_AMODE_ALL) \
	DECL_LAMPSET(LAMPSET_AMODE_RAND, LMSET_AMODE_RAND)

define LMSET_DOOR_PANEL_AWARDS \
	LM_PANEL_TSM, LM_PANEL_EB, LM_PANEL_SUPER_SLOT, \
	LM_PANEL_CLOCK_MIL, \
	LM_PANEL_SPIRAL, \
	LM_PANEL_BATTLE_POWER, \
	LM_PANEL_10M, \
	LM_PANEL_GREED, LM_PANEL_CAMERA, LM_PANEL_HH, \
	LM_PANEL_CLOCK_CHAOS, \
	LM_PANEL_SUPER_SKILL, \
	LM_PANEL_FAST_LOCK, \
	LM_PANEL_GUMBALL

define LMSET_DOOR_PANELS_AND_HANDLE \
	LMSET_DOOR_PANEL_AWARDS, LM_PANEL_LITZ

define LMSET_DOOR_LOCKS			LM_LOCK1, LM_LOCK2

define LMSET_DOOR_GUMBALL 		LM_GUM, LM_BALL

define LMSET_DOOR_LOCKS_AND_GUMBALL  LMSET_DOOR_LOCKS, LMSET_DOOR_GUMBALL

define LMSET_LEFT_RAMP_AWARDS	LM_BONUS_X, LM_MULTIBALL, LM_SUPER_SKILL

define LMSET_LOCK_AWARDS			LM_LOCK_ARROW, LM_LOCK_EB

define LMSET_PIANO_AWARDS			LM_PIANO_PANEL, LM_PIANO_JACKPOT

define LMSET_GREED_TARGETS \
	LM_LL_5M, LM_ML_5M, LM_UL_5M, LM_UR_5M, LM_MR2_5M, LM_MR1_5M, LM_LR_5M

define LMSET_POWERFIELD_VALUES	LM_MPF_500K, LM_MPF_750K, LM_MPF_1M

define LMSET_JETS					LM_LEFT_JET, LM_RIGHT_JET, LM_LOWER_JET

define LMSET_LOWER_LANES \
	LM_LEFT_OUTLANE, LM_LEFT_INLANE1, LM_LEFT_INLANE2, LM_RIGHT_INLANE, \
	LM_RIGHT_OUTLANE

define LMSET_SPIRAL_AWARDS \
	LM_SPIRAL_2M, LM_SPIRAL_3M, LM_SPIRAL_4M, \
	LM_SPIRAL_5M, LM_SPIRAL_10M, LM_SPIRAL_EB

define LMSET_AMODE_ALL \
	LMSET_DOOR_PANELS_AND_HANDLE, \
	LMSET_DOOR_LOCKS, \
	LMSET_DOOR_GUMBALL, \
	LMSET_SPIRAL_AWARDS, \
	LMSET_LOWER_LANES, \
	LMSET_JETS, \
	LMSET_POWERFIELD_VALUES, \
	LM_LEFT_SPIRAL, LM_LEFT_POWERBALL, \
	LMSET_LEFT_RAMP_AWARDS, \
	LM_RAMP_BATTLE, \
	LMSET_LOCK_AWARDS, \
	LMSET_PIANO_AWARDS, \
	LM_RIGHT_SPIRAL, \
	LM_RIGHT_POWERBALL, \
	LMSET_GREED_TARGETS, \
	LM_DEAD_END, \
	LM_SLOT_MACHINE, \
	LM_CAMERA

define LMSET_AMODE_RAND 22,21,2,58,13,15,49,12,38,54,59,42,37,39,40,28,45,17,4,10,26,52,11,53,34,43,50,56,3,44,41,23,46,14,55,31,18,32,16,9,61,8,1,19,0,24,35,29,27,6,7,20,25,33,5,48 


