#--------------------------------------------------------------------------
# TZ machine description for FreeWPC
# (C) Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
#
# See tools/genmachine for more information about the format of this file.
#--------------------------------------------------------------------------

# This file describes many characteristics of a pinball machine,
# mostly physical stuff, although some rules can be coded here also.

##########################################################################
# General section (before a [section] header is given.
# Miscellaneous parameters are specified here.
##########################################################################
Title: Twilight Zone

# NOTE: These next 4 lines do not belong here.  They should be
# included via the kernel md file.
DMD: Yes
Fliptronic: Yes
DCS: No
WPC95: No

Pinmame-Zip: tz_92.zip
Pinmame-ROM: tzone9_2.rom
Lamp-Matrix-Width: 39
Lamp-Matrix-Height: 29

##########################################################################
# Include standard definitions that apply to all WPC games.
# This will set some defaults for things you leave out here.
##########################################################################
include platform/wpc/wpc.md

##########################################################################
# Use 'define' to emit a plain #define for anything not covered by
# some other means.
##########################################################################
define MACHINE_TZ
define MACHINE_INCLUDE_FLAGS
define MACHINE_SOL_EXTBOARD1
define MACHINE_CUSTOM_AMODE
define MACHINE_SCORE_DIGITS               10
define MACHINE_MUSIC_GAME                 MUS_MULTIBALL_LIT
define MACHINE_MUSIC_PLUNGER              MUS_MULTIBALL_LIT_PLUNGER
define MACHINE_REPLAY_CODE_TO_SCORE       replay_code_to_score
define MACHINE_DEBUGGER_HOOK              tz_debugger_hook
define MACHINE_REPLAY_SCORE_CHOICES       10
define MACHINE_REPLAY_START_CHOICE        5
define MACHINE_OUTHOLE_KICK_HOOK          tz_outhole_kick_hook
define MACHINE_AMODE_LEFF                 tz_amode_leff
#ifdef MACHINE_AMODE_FLIPPER_SOUND_CODE   SND_THUD

##########################################################################
# Lamp Description
# The key is given in column/row format.  The first parameter must be
# the lamp name.  Optionally you can specify a color, and x() and y()
# for the location on the playfield.  Only a subset of lamp colors are
# recognized; see tools/genmachine for details.  Lamp location is
# given in terms of the Lamp-Matrix-Width and Lamp-Matrix-Height.
##########################################################################
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
32: Lock1 , red, x(25), y(12)
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
46: Spiral EB, red ,x(32), y(18)
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
61: Left Jet ,red, x(17), y( 1)
62: Lower Jet ,orange, x(22), y( 3)
63: Right Jet ,yellow, x(17), y( 6)
64: ML 5M, yellow ,x(19), y( 7)
65: UL 5M, yellow ,x(17), y( 8)
66: Right Outlane, red ,x(27), y(23)
67: Right Powerball, white ,x(14), y(22)
68: Right Spiral, orange ,x(12), y(23)
71: LR 5M ,yellow, x(16), y(17)
72: MR1 5M ,yellow, x(14), y(16)
73: MR2 5M ,yellow, x(12), y(16)
74: Power Payoff ,white, x(10), y(18)
75: UR 5M ,yellow, x( 5), y(20)
76: MPF 500K ,yellow, x( 9), y( 3)
77: MPF 750K ,orange, x( 7), y( 2)
78: MPF 1M ,red, x( 7), y( 4)
81: Left Spiral, yellow ,x( 5), y( 6)
82: Clock Millions, white ,x( 5), y(15)
83: Piano Panel, yellow ,x( 8), y(21)
84: Piano Jackpot, red ,x( 7), y(21)
85: Slot Machine, yellow ,x(14), y(19)
86: Gumball Lane, red ,x(15), y(23)
87: Buy-In Button, yellow, buyin, cabinet, x(38), y(27)
88: Start Button, yellow, start, cabinet, x(38), y( 1)

##########################################################################
# Switch Description
# The key is in column/row format.  The first parameter must be the switch
# name.  Options can be given in any order:
#    ingame - only service the switch during a game
#    intest - also service the switch in test mode
#    noplay - tripping this switch does NOT mark ball in play
#    standup - this is a standup
#    button - this is a button
#    edge - this switch should be serviced on either transition
#    opto - this switch is optical and activates on closed->open
#
# These parameters mark various well-known switches.  Only one of these
# can exist per type:
#    outhole, slam-tilt, tilt, shooter, start-button, buyin-button
#
# Use sound() to invoke a sound call automatically when the switch activates.
# Use lamp() to flicker a lamp automatically on activation.  TODO:
#   this requires the c_name, and not the friendly md name.
# Use c_decl() to override the default name of the switch event.
#
##########################################################################
[switches]
11: Right Inlane, ingame, sound(SND_INSIDE_LEFT_INLANE), lamp(LM_RIGHT_INLANE)
12: Right Outlane, ingame, sound(SND_DRAIN)
13: Start Button, start-button, intest
14: Tilt, tilt, ingame, noplay
15: Right Trough, c_decl(sw_trough), noscore
16: Center Trough, c_decl(sw_trough), noscore
17: Left Trough, c_decl(sw_trough), noscore
18: Outhole, outhole, noscore
21: Slam Tilt, slam-tilt, ingame, cabinet
23: Buyin Button, buyin-button
25: Far Left Trough, noscore
26: Trough Proximity, edge, noscore
27: Shooter, edge, shooter, noscore
28: Rocket Kicker
31: Left Jet, ingame, c_decl(sw_jet), fast, Left Jet, noplay
32: Right Jet, ingame, c_decl(sw_jet), fast, Right Jet, noplay
33: Bottom Jet, ingame, c_decl(sw_jet), fast, Bottom Jet, noplay
34: Left Sling, ingame, c_decl(sw_sling), fast, Left Slingshot, noplay
35: Right Sling, ingame, c_decl(sw_sling), fast, Right Slingshot, noplay
36: Left Outlane, ingame, sound(SND_DRAIN)
37: Left Inlane 1, ingame, sound(SND_INSIDE_LEFT_INLANE), lamp(LM_LEFT_INLANE1)
38: Left Inlane 2, ingame, sound(SND_INSIDE_LEFT_INLANE), lamp(LM_LEFT_INLANE2)
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
55: Gumball Geneva, noscore
56: Gumball Exit, noscore
57: Slot Proximity, noscore
58: Slot
61: Skill Bottom, ingame, noplay
62: Skill Center, ingame, noplay
63: Skill Top, ingame, noplay
64: Standup 4, standup, ingame, lamp(LM_UR_5M)
65: Power Payoff, standup, ingame
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

##########################################################################
# Drives
# This describes the names of the solenoid/motor drives.
# Hn = high power solenoids
# Ln = low power solenoids
# Gn = general purpose solenoids
# etc.
#
# The following options are supported:
#    flash - this is a flasher (default is solenoid, flash implies nosearch)
#    motor - this is a motor (default is solenoid)
#    nosearch - do not activate this during ball search
#
# The following options denote well-known drives:
#    knocker, ballserve
#
##########################################################################
[drives]
H1: Slot
H2: Rocket Kicker
H3: Autofire, nosearch
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
L8: Shooter Div, nosearch

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


##########################################################################
# General Illumination
##########################################################################
[gi]
0: L.Left Playfield
1: Powerfield
2: Clock
3: Mystery
4: L.Right Playfield

##########################################################################
# Tests
# These are additional test items that should appear in the TESTS menu.
##########################################################################
[tests]
tz_clock:
tz_gumball:
tz_magnet:
tz_powerball:


##########################################################################
# Lampsets
# These denote logical groupings of lamps.
# The key is a textual name for the set.  The options specify which lamps
# are a part of that set.  You can specify 1 or more lamp values,
# separated by commas, where each value can be: 1) a single lamp name,
# 2) another lampset name, already defined, or 3) a lamp range in the
# form lamp1..lampN.
#
# The special construct PF:function indicates that a Perl subroutine
# should be used to select and sort playfield lamps.  Some functions are
# builtin to genmachine; others can be defined by you and included
# via the 'perlinclude' directive.
#
# Note that a lampset may contain only one lamp.  The lampset is the
# unit of 'allocation' for a lamp effect.
##########################################################################
[lamplists]
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
Misc: Slot Machine, Ramp Battle, Clock Millions, Shoot Again, Power Payoff, Gumball Lane, Camera, Dead End
Spiral Awards: Spiral 2M..Spiral EB
Amode All: Door Panels and Handle, Door Locks, Door Gumball, Spiral Awards, Lower Lanes, Jets, Powerfield Values, Left Spiral, Left Powerball, Left Ramp Awards, Ramp Battle, Lock Awards, Piano Awards, Right Spiral, Right Powerball, Greed Targets, Dead End, Slot Machine, Camera
Amode Rand: Amode All
Sort1: PF:lamp_sort_bottom_to_top
Sort2: PF:lamp_sort_top_to_bottom
Sort3: PF:lamp_sort_left_to_right
Sort4: PF:lamp_sort_right_to_left
Circle Out: PF:lamp_sort_circle_out
Lock Test: PF:build_lampset_from_lock
Ball Save: Shoot Again
Red Lamps: COLOR:red
White Lamps: COLOR:white
Orange Lamps: COLOR:orange
Yellow Lamps: COLOR:yellow
Green Lamps: COLOR:green
Blue Lamps: COLOR:blue
Amber Lamps: COLOR:amber

##########################################################################
# Containers
# These denote devices that can hold, count, and eject pinballs.
# The key is the name of the device.  The first parameter is the name
# of the solenoid that kicks out a ball.  The 'trough' parameter should
# be specified on the single trough device.  The 'init_max_count'
# parameter should say how many balls are normally held in this device
# at startup.  The remaining parameters are the names of the switches
# that count pinball, in front from entry to exit.
##########################################################################
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



#------------------------------------------------------------------------
# The remaining sections describe software aspects, and not the physical
# machine.
#------------------------------------------------------------------------

##########################################################################
# Items for the Feature Adjustments menu.  Parameters indicate the
# type of adjustment and the default value.
##########################################################################
[adjustments]
Ball Saves: integer, 1
Ball Save Time: integer, 7
Installed Balls: integer, 6
Disable Clock: yes_no, NO
Disable Gumball: yes_no, NO
Powerball Missing: yes_no, NO
Have Third Magnet: yes_no, NO

##########################################################################
# Items for the Feature Audits menu.
##########################################################################
[audits]
3 Panels:
6 Panels:
9 Panels:
12 Panels:

##########################################################################
# Sound calls for well-known events
##########################################################################
[system_sounds]
Add Coin: SND_LIGHT_SLOT_TIMED
Add Credit: SND_THUD
Start Game: SND_ARE_YOU_READY_TO_BATTLE
Tilt Warning: SND_TILT_WARNING
Tilt: SND_TILT

##########################################################################
# Music calls for well-known events
##########################################################################
[system_music]
Start Ball: MUS_MULTIBALL_LIT_PLUNGER
Ball in Play: MUS_MULTIBALL_LIT
End Game: MUS_POWERBALL_MANIA
Volume Change: MUS_SUPER_SLOT

##########################################################################
# A list of all scores needed by the game rules.
##########################################################################
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

##########################################################################
# The default high scores.  Use GC to indicate the grand champion.
# The parameters are the initials followed by the score value.  Periods
# may optionally be used to group digits, but they are not necessary.
# Commas _cannot_ be used for this purpose since they separate parameters.
##########################################################################
[highscores]
GC: QQQ, 500.000.000
1: CCD, 400.000.000
2: MLD, 350.000.000
3: BDD, 300.000.000
4: NWU, 250.000.000


##########################################################################
# Bit flags.
##########################################################################
[flags]
Piano Door Lit:
Slot Door Lit:
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
SSSMB_RUNNING:
SSSMB_RED_JACKPOT:
SSSMB_ORANGE_JACKPOT:
SSSMB_YELLOW_JACKPOT:
CHAOSMB_RUNNING:
TSM_RUNNING:
MB_JACKPOT_LIT:

[globalflags]
Clock Working:

##########################################################################
# Display effects
##########################################################################
[deffs]
Bonus: page(MACHINE_PAGE), runner, PRI_BONUS
# Brian Image: PRI_EGG1
Replay: page(MACHINE_PAGE), PRI_REPLAY
Jackpot: page(MACHINE_PAGE), PRI_JACKPOT
Special: page(MACHINE_PAGE), PRI_SPECIAL
Extra Ball: page(MACHINE_PAGE), PRI_EB

Greed Round: page(MACHINE_PAGE), runner, PRI_GAME_MODE3
Skill Shot Ready: page(MACHINE_PAGE), runner, PRI_GAME_MODE5
MB Running: page(MACHINE_PAGE), runner, PRI_GAME_MODE7
Video Mode: page(MACHINE_PAGE), PRI_GAME_MODE8

Left Ramp: page(MACHINE_PAGE), PRI_GAME_QUICK2
TV Static: page(MACHINE_PAGE), PRI_GAME_QUICK3
Text Color Flash: page(MACHINE_PAGE), PRI_GAME_QUICK3
Two Color Flash: page(MACHINE_PAGE), PRI_GAME_QUICK3
Spell Test: page(MACHINE_PAGE), PRI_GAME_QUICK3
Hitchhiker: page(MACHINE_PAGE), PRI_GAME_QUICK5
Door Award: PRI_GAME_QUICK7, D_QUEUED+D_TIMEOUT
Clock Millions Hit: page(MACHINE_PAGE), PRI_GAME_QUICK8
Lock Lit: page(MACHINE_PAGE), PRI_GAME_QUICK8
PB Loop: page(MACHINE_PAGE), PRI_GAME_QUICK8
MB Start: page(MACHINE_PAGE), PRI_GAME_QUICK8, D_ABORTABLE
MB Lit: page(MACHINE_PAGE), PRI_GAME_MODE8
Ball Save: page(MACHINE_PAGE), PRI_BALLSAVE
PB Detect: page(MACHINE_PAGE), PRI_GAME_QUICK8, D_QUEUED+D_TIMEOUT
Skill Shot Made: page(MACHINE_PAGE), PRI_GAME_QUICK8
Camera Award: page(MACHINE_PAGE), PRI_GAME_QUICK8
LITZ Award: PRI_GAME_QUICK8

SSSMB Jackpot Collected: page(MACHINE_PAGE), PRI_JACKPOT
SSSMB Running: page(MACHINE_PAGE), runner, PRI_GAME_MODE6
SSSMB Jackpot Lit: page(MACHINE_PAGE), runner, PRI_GAME_MODE7

ChaosMB Running: page(MACHINE_PAGE), runner, PRI_GAME_MODE6
Chaos Jackpot: page(MACHINE_PAGE), PRI_GAME_QUICK8

Animation Test: page(EFFECT_PAGE), PRI_GAME_MODE2

BG Flash: page(MACHINE_PAGE), PRI_GAME_MODE4

##########################################################################
# Lamp effects
##########################################################################
[leffs]

perlinclude machine/tz/leff.pl

Flasher Happy: PRI_LEFF1, page(MACHINE_PAGE)
GUMBALL STROBE: PRI_LEFF1, page(MACHINE_PAGE)
Left Ramp: PRI_LEFF2, page(MACHINE_PAGE)
CLOCK TARGET: PRI_LEFF2, GI(ALL), page(MACHINE_PAGE)
No GI: L_NORMAL, PRI_LEFF3, GI(ALL), page(MACHINE_PAGE)
SLOT KICKOUT: PRI_LEFF3, page(MACHINE_PAGE)
Amode: runner, PRI_LEFF1, LAMPS(AMODE_ALL), GI(ALL), page(MACHINE_PAGE)
FLASH ALL: PRI_LEFF5, LAMPS(AMODE_ALL), page(MACHINE_PAGE)
Bonus: runner, PRI_BONUS, LAMPS(ALL), GI(ALL), page(MACHINE_PAGE)
Jets Active: shared, PRI_LEFF5, LAMPS(JETS), page(MACHINE_PAGE)
GAME TIMEOUT: PRI_TILT, GI(ALL), page(MACHINE_PAGE)
CLOCK START: PRI_LEFF4, GI(ALL), c_decl(clock_round_started_leff), page(MACHINE_PAGE)
MB RUNNING: shared, PRI_LEFF2, LAMPS(DOOR_LOCKS_AND_GUMBALL), c_decl(multiball_running_leff), page(MACHINE_PAGE)
Strobe Up: PRI_LEFF2, LAMPS(ALL), page(MACHINE_PAGE)
Multi Strobe: PRI_LEFF2, LAMPS(ALL), page(MACHINE_PAGE)
Door Strobe: PRI_LEFF1, LAMPS(DOOR_PANELS), GI(ALL), page(MACHINE_PAGE)
Left Loop: PRI_LEFF1, LAMPS(SORT3), page(MACHINE_PAGE)
Right Loop: PRI_LEFF1, LAMPS(SORT4), page(MACHINE_PAGE)
Ball Save: shared, PRI_LEFF3, LAMPS(BALL_SAVE)
Color Cycle: PRI_LEFF3, LAMPS(AMODE_ALL), GI(ALL), page(MACHINE_PAGE)
Circle Out: PRI_LEFF3, LAMPS(CIRCLE_OUT), page(MACHINE_PAGE)
Lock: PRI_LEFF4, LAMPS(LOCK_TEST), page(MACHINE_PAGE)

##########################################################################
# Fonts used in this game.
##########################################################################
[fonts]
mono9:
times10:
misctype:
v5prc:
steel:
lithograph:
