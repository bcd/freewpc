#--------------------------------------------------------------------------
# TZ machine description for FreeWPC
# (C) Copyright 2006, 2007, 2009 by Brian Dominy <brian@oddchange.com>
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

include platform/wpc/wpc-fliptronic.md

Pinmame-Zip: tz_92.zip
Pinmame-ROM: tzone9_2.rom
Lamp-Matrix-Width: 39
Lamp-Matrix-Height: 29


##########################################################################
# Use 'define' to emit a plain #define for anything not covered by
# some other means.
##########################################################################
define MACHINE_TZ
define MACHINE_INCLUDE_FLAGS
define MACHINE_SOL_EXTBOARD1
define MACHINE_CUSTOM_AMODE
define MACHINE_SCORE_DIGITS               10
define MACHINE_MUSIC_GAME                 MUS_FASTLOCK_BANZAI_RUN
define MACHINE_MUSIC_PLUNGER              MUS_MULTIBALL_LIT_PLUNGER
define MACHINE_REPLAY_CODE_TO_SCORE       replay_code_to_score
define MACHINE_REPLAY_SCORE_CHOICES       10
define MACHINE_REPLAY_START_CHOICE        5
define MACHINE_AMODE_EFFECTS
define MACHINE_HAS_UPPER_LEFT_FLIPPER
define MACHINE_HAS_UPPER_RIGHT_FLIPPER
define MACHINE_AMODE_FLIPPER_SOUND_CODE   SND_THUD
define CONFIG_TZONE_IP y
# Allow only one high score entry per player
#define CONFIG_ONE_HS_PER_PLAYER 

#########################################################################
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
47: Shoot Again, red , shoot-again, x(36), y(13)
48: Right Inlane, white ,x(26), y(21)
51: Bonus X, white ,x( 5), y(11)
52: Multiball, white ,x( 7), y(12)
53: Super Skill, orange ,x( 9), y(13)
54: Left Powerball, white ,x( 6), y( 9)
55: Camera, white ,x(14), y(11)
56: Ramp Battle, orange ,x( 6), y(17)
57: Lock EB, red , extra-ball, x( 3), y(19)
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
11: Right Inlane, ingame, sound(SND_INSIDE_LEFT_INLANE)
12: Right Outlane, ingame, sound(SND_DRAIN)
13: Start Button, start-button, intest
14: Tilt, tilt, ingame, noplay, cabinet
15: Right Trough, noscore
16: Center Trough, noscore
17: Left Trough, noscore
18: Outhole, outhole, service, noplay, intest
21: Slam Tilt, slam-tilt, ingame, cabinet
23: Buyin Button, buyin-button
25: Far Left Trough, noscore
26: Trough Proximity, edge, noscore
27: Shooter, edge, shooter, noscore, debounce(TIME_200MS)
28: Rocket Kicker
31: Left Jet, ingame, c_decl(sw_jet), noplay, lamp(LM_LEFT_JET)
32: Right Jet, ingame, c_decl(sw_jet), noplay, lamp(LM_RIGHT_JET)
33: Bottom Jet, ingame, c_decl(sw_jet), noplay, lamp(LM_LOWER_JET)
34: Left Sling, ingame, c_decl(sw_sling), noplay
35: Right Sling, ingame, c_decl(sw_sling), noplay
36: Left Outlane, ingame, sound(SND_DRAIN)
37: Left Inlane 1, ingame, sound(SND_INSIDE_LEFT_INLANE)
38: Left Inlane 2, ingame, sound(SND_INSIDE_LEFT_INLANE)
41: Dead End, ingame
42: Camera, ingame
43: Piano
44: MPF Enter, ingame
45: MPF Left, ingame
46: MPF Right, ingame
47: Clock Target, standup, ingame
48: Standup 1, standup, ingame, lamp(LM_LL_5M)
51: Gumball Lane, intest
52: Hitchhiker, ingame
53: Left Ramp Enter, ingame, sound(SND_LEFT_RAMP_ENTER)
54: Left Ramp Exit, ingame, sound(SND_LEFT_RAMP_MADE)
55: Gumball Geneva, noscore, edge
56: Gumball Exit, noscore
57: Slot Proximity, edge, noscore
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
H1: Slot, duty(SOL_DUTY_100), time(TIME_200MS)
H2: Rocket Kicker, duty(SOL_DUTY_75), time(TIME_200MS)
H3: Autofire, nosearch, launch, duty(SOL_DUTY_100), time(TIME_200MS)
H4: Popper, time(TIME_100MS)
H5: Right Ramp Div, duty(SOL_DUTY_50), time(TIME_100MS)
H6: Gumball Div, duty(SOL_DUTY_50), time(TIME_133MS)
H7: Knocker, knocker, time(TIME_16MS)
H8: Outhole, duty(SOL_DUTY_50), time(TIME_133MS)

L1: Ball Serve, ballserve, duty(SOL_DUTY_25), time(TIME_133MS)
L2: Right Sling, duty(SOL_DUTY_100)
L3: Left Sling, duty(SOL_DUTY_100)
L4: Lower Jet, duty(SOL_DUTY_100)
L5: Left Jet, duty(SOL_DUTY_100)
L6: Right Jet, duty(SOL_DUTY_100)
L7: Lock Release, duty(SOL_DUTY_75), time(TIME_133MS)
L8: Shooter Div, nosearch

G1: Jets, flash
G2: Ramp3 Power Payoff, flash
G3: Powerfield, flash
G4: Ramp1, flash
G5: Left Magnet, nosearch
G6: Upper Right Magnet, nosearch
G7: Right Magnet, nosearch
G8: Gumball Release, motor, nosearch

A1: MPF Left Magnet, nosearch
A2: MPF Right Magnet, nosearch
A3: Ramp Divertor, duty(SOL_DUTY_25), time(TIME_100MS)
A4: Clock Target, flash

F5: U.R. Flip Power, time(TIME_33MS)
F6: U.R. Flip Hold, time(TIME_100MS)
F7: U.L. Flip Power, time(TIME_33MS)
F8: U.L. Flip Hold, time(TIME_100MS)

X1: UR Flipper, flash
X2: Gumball High, flash
X3: Gumball Mid, flash
X4: Gumball Low, flash
X5: Ramp2, flash
X6: Clock Reverse, motor, nosearch
X7: Clock Forward, motor, nosearch


[templates]

Left Sling: driver(spsol), sw=SW_LEFT_SLING, sol=SOL_LEFT_SLING, ontime=3, offtime=16
Right Sling: driver(spsol), sw=SW_RIGHT_SLING, sol=SOL_RIGHT_SLING, ontime=3, offtime=16
Left Jet: driver(spsol), sw=SW_LEFT_JET, sol=SOL_LEFT_JET, ontime=3, offtime=16
Right Jet: driver(spsol), sw=SW_RIGHT_JET, sol=SOL_RIGHT_JET, ontime=3, offtime=16
Lower Jet: driver(spsol), sw=SW_BOTTOM_JET, sol=SOL_LOWER_JET, ontime=3, offtime=16
Left mpf: driver(mpfmag), sw=SW_LEFT_BUTTON, sol=SOL_MPF_LEFT_MAGNET, ontime=3, offtime=3
Right mpf: driver(mpfmag), sw=SW_RIGHT_BUTTON, sol=SOL_MPF_RIGHT_MAGNET, ontime=3, offtime=3

Clock Mech: driver(bivar),
	forward_sol=SOL_CLOCK_FORWARD,
	reverse_sol=SOL_CLOCK_REVERSE

Bridge Open: driver(duty2),
	sol=SOL_RIGHT_RAMP_DIV, timeout=TIME_4S, ontime=TIME_33MS, duty_mask=DUTY_MASK_50

Shooter Div: driver(duty2),
	sol=SOL_SHOOTER_DIV, timeout=TIME_4S, ontime=TIME_700MS, duty_mask=DUTY_MASK_50

Ramp Div: driver(duty2),
	sol=SOL_RAMP_DIVERTOR, timeout=TIME_3S, ontime=TIME_33MS, duty_mask=DUTY_MASK_50

Gumball Div: driver(duty2),
	sol=SOL_GUMBALL_DIV, timeout=TIME_2S, ontime=TIME_33MS, duty_mask=DUTY_MASK_50

### old duty driver ###
#Ramp Div: driver(duty),
#	sol=SOL_RAMP_DIVERTOR,
#	ontime=TIME_33MS, duty_ontime=TIME_16MS, duty_offtime=TIME_16MS, timeout=TIME_3S
#Shooter Div: driver(duty),
#	sol=SOL_SHOOTER_DIV, timeout=TIME_4S, ontime=TIME_200MS, duty_ontime=TIME_16MS, duty_offtime=TIME_50MS
#Bridge Open: driver(duty),
#	sol=SOL_RIGHT_RAMP_DIV, timeout=TIME_4S, ontime=TIME_16MS, duty_ontime=TIME_16MS, duty_offtime=TIME_50MS
#Gumball Div: driver(duty),
#	sol=SOL_GUMBALL_DIV, timeout=TIME_3S, ontime=TIME_16MS, duty_ontime=TIME_16MS, duty_offtime=TIME_50MS

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

Lock: Lock Release, init_max_count(0), \
	Lock Upper, Lock Center, Lock Lower

Rocket: Rocket Kicker, init_max_count(0), \
	Rocket Kicker

Slot: Slot, init_max_count(0), \
	Slot

Popper: Popper, \
	Gumball Popper

#Gumball: Gumball Release, init_max_count(3), Gumball Enter, Gumball Exit

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
ac_detect:

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
Red Lamps: COLOR:red
White Lamps: COLOR:white
Orange Lamps: COLOR:orange
Yellow Lamps: COLOR:yellow
Green Lamps: COLOR:green
Blue Lamps: COLOR:blue
Amber Lamps: COLOR:amber
Unlit shots: Dead End, Slot Machine, Piano Panel, Ramp Battle, Camera
Inlanes: Left Inlane1, Left Inlane2, Right Inlane
Chaosmb Jackpots: Multiball, Ramp Battle, Piano Jackpot, Camera, Power Payoff, Dead End

Select mode: Door Locks, Door Gumball, Spiral Awards, Lower Lanes, Jets, Powerfield Values, Left Spiral, Left Powerball, Left Ramp Awards, Ramp Battle, Lock Awards, Piano Awards, Right Spiral, Right Powerball, Greed Targets, Dead End, Slot Machine, Camera

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
Mute/Pause: yes_no, YES
TZ Flipcodes: yes_no, YES
TZ Mag Helpers: yes_no, YES
Snake Wrap:yes_no, YES


# Hit Slot/Piano to relight if < 8 door_panels
Easy lite door: yes_no, YES

# Used by the autofire, turn on if autofire opto fails 
Fire empty: yes_no, NO

# No lock needed to relight Gumball
Easy Lite Gumball: yes_no, YES

# Do not grab the ball on the right magnet if gumball is lit
No grab gumball: yes_no, NO

# Show advanced info during bonus
Adv bonus info:yes_no, YES

# Tweak the drop time for the throw automatically
# Doesnt currenly work
Auto SWAG:yes_no, NO

# Dont allow throw, as the drag may cause damage to the table
Mag Throw:yes_no, YES

# Dont allow cradling during multiball
Dixon anti cradle:yes_no, NO

# Only allow one high score entry per player (Not working)
One HS entry:yes_no, NO

# Set the highest oddchange random award, 20 = 500K
Oddchange Level:integer, 18

##########################################################################
# Items for the Feature Audits menu.
##########################################################################
[audits]
3 Panel Games:
6 Panel Games:
9 Panel Games:
12 Panel Games:
Clock Errors:
LITZ Started:
Door Panels:

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
Start Ball: MUS_CLOCK_MILLIONS
Ball in Play: MUS_FASTLOCK_BANZAI_RUN 
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
150K:
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
100M:
200M:
300M:
400M:
500M:

##########################################################################
# The default high scores.  Use GC to indicate the grand champion.
# The parameters are the initials followed by the score value.  Periods
# may optionally be used to group digits, but they are not necessary.
# Commas _cannot_ be used for this purpose since they separate parameters.
##########################################################################
[highscores]
GC: BCD, 500.000.000
1: HRI, 400.000.000
2: FEK, 350.000.000
3: DOM, 300.000.000
4: MLC, 250.000.000


##########################################################################
# Bit flags.
##########################################################################

# These should be player specific stuff
[flags]
PIANO_DOOR_LIT:
SLOT_DOOR_LIT:
SNAKE_READY:
# Awarded for doing 88mph during fastlock
MARTY_BONUS:

# These are for non-player specific stuff
[globalflags]
POWERBALL_IN_PLAY:
SSSMB_RED_JACKPOT:
SSSMB_ORANGE_JACKPOT:
SSSMB_YELLOW_JACKPOT:
MB_JACKPOT_LIT:
QUICK_MB_RUNNING:
SSSMB_RUNNING:
CHAOSMB_RUNNING:
MULTIBALL_RUNNING:
SUPER_MB_RUNNING:
BTTZ_RUNNING:
Clock Working:
Clock Home:
Sharpe running:

##########################################################################
# Display effects
##########################################################################
[deffs]
Cow: page(MACHINE_PAGE), PRI_EGG1
TZ Flipcode entry: page(MACHINE3_PAGE), PRI_REPLAY
TZ Flipcode entered: page(MACHINE3_PAGE), PRI_DEBUGGER, D_PAUSE+D_QUEUED

Loop master Entry: page(MACHINE3_PAGE), PRI_HSENTRY
Loop master Exit: page(MACHINE3_PAGE), PRI_HSENTRY
Combo master Entry: page(MACHINE3_PAGE), PRI_HSENTRY
Combo master Exit: page(MACHINE3_PAGE), PRI_HSENTRY

Extra Ball: page(MACHINE_PAGE), PRI_EB, D_PAUSE+D_QUEUED
Shoot Again: page(MACHINE3_PAGE), PRI_SHOOT_AGAIN, D_PAUSE+D_QUEUED

Greed Mode: page(MACHINE3_PAGE), runner, PRI_GAME_MODE2, D_QUEUED+D_TIMEOUT
Greed Mode total: page(MACHINE3_PAGE), PRI_GAME_MODE3, D_QUEUED+D_PAUSE+D_ABORTABLE
Skill Shot Ready: page(MACHINE_PAGE), runner, PRI_GAME_MODE5, D_QUEUED+D_TIMEOUT

#These are in order of how they get triggered
Skill Shot Made: page(MACHINE_PAGE), PRI_GAME_QUICK6, D_PAUSE+D_QUEUED
Rocket: page(MACHINE2_PAGE), PRI_GAME_QUICK5, D_PAUSE+D_QUEUED+D_TIMEOUT
Hitchhiker: page(MACHINE2_PAGE), PRI_GAME_QUICK6, D_RESTARTABLE+D_QUEUED+D_TIMEOUT
Jets Hit: page(MACHINE_PAGE), PRI_GAME_QUICK3, D_RESTARTABLE
Jets Level Up: page(MACHINE_PAGE), PRI_GAME_QUICK4, D_QUEUED+D_TIMEOUT

Gumball: page(MACHINE_PAGE), PRI_GAME_MODE7, D_PAUSE+D_QUEUED+D_TIMEOUT
SSSMB Jackpot Collected: page(MACHINE_PAGE), PRI_JACKPOT, D_PAUSE+D_QUEUED
SSSMB Running: page(MACHINE_PAGE), runner, PRI_GAME_MODE6
SSSMB Jackpot Lit: page(MACHINE_PAGE), PRI_GAME_MODE8, D_PAUSE+D_QUEUED+D_RESTARTABLE
SSlot Mode: page(MACHINE_PAGE), runner, PRI_GAME_MODE4
SSlot Award: page(MACHINE_PAGE), PRI_JACKPOT, D_PAUSE
TSM Mode: page(MACHINE_PAGE), runner, PRI_GAME_MODE4
TSM Mode total: page(MACHINE_PAGE), PRI_GAME_MODE4, D_QUEUED+D_PAUSE+D_ABORTABLE
Spiral Mode: page(MACHINE3_PAGE), runner, PRI_GAME_MODE5
Spiral Mode total: page(MACHINE3_PAGE), PRI_GAME_MODE5, D_QUEUED+D_PAUSE+D_ABORTABLE
Spiral Loop: page(MACHINE3_PAGE), PRI_GAME_QUICK8, D_SCORE+D_QUEUED
Fastlock Mode: page(MACHINE_PAGE), runner, PRI_GAME_MODE3
Fastlock Award: page(MACHINE_PAGE), PRI_JACKPOT
Hitch Mode: page(MACHINE2_PAGE), runner, PRI_GAME_MODE3

Clock Millions Mode: page(MACHINE3_PAGE), runner, PRI_GAME_MODE3
# D_PAUSE so it will stop the clock
Clock Millions Hit: page(MACHINE3_PAGE), PRI_GAME_QUICK7, D_RESTARTABLE+D_PAUSE
Clock Millions Explode: page(MACHINE3_PAGE), PRI_GAME_QUICK8, D_QUEUED+D_TIMEOUT+D_RESTARTABLE
Clock Millions Mode Total: page(MACHINE3_PAGE), PRI_GAME_MODE5, D_QUEUED+D_PAUSE+D_ABORTABLE

MPF Mode: page(MACHINE_PAGE), runner, PRI_GAME_MODE8, D_QUEUED+D_TIMEOUT+D_PAUSE
MPF Award: page(MACHINE_PAGE), PRI_JACKPOT, D_PAUSE+D_QUEUED+D_TIMEOUT

Animation Test: page(EFFECT_PAGE), PRI_GAME_MODE2
BG Flash: page(MACHINE_PAGE), PRI_GAME_MODE4
Left Ramp: page(MACHINE_PAGE), PRI_GAME_QUICK2, D_RESTARTABLE
Dead End: page(MACHINE2_PAGE), PRI_GAME_QUICK2, D_PAUSE+D_QUEUED+D_RESTARTABLE+D_TIMEOUT
Inlane lights Dead End: page(MACHINE2_PAGE), PRI_GAME_QUICK2, D_PAUSE+D_QUEUED+D_RESTARTABLE+D_TIMEOUT
Left ramp lights camera: page(MACHINE2_PAGE), PRI_GAME_QUICK2, D_PAUSE+D_RESTARTABLE+D_TIMEOUT
Shoot Hitch: page(MACHINE_PAGE), PRI_GAME_QUICK2, D_PAUSE+D_QUEUED+D_RESTARTABLE+D_TIMEOUT
TV Static: page(MACHINE_PAGE), PRI_GAME_QUICK3, D_QUEUED+D_TIMEOUT
Text Color Flash: page(MACHINE_PAGE), PRI_GAME_QUICK3, D_QUEUED+D_TIMEOUT
Two Color Flash: page(MACHINE_PAGE), PRI_GAME_QUICK3, D_QUEUED+D_TIMEOUT
Spell Test: page(MACHINE_PAGE), PRI_GAME_QUICK3, D_QUEUED+D_TIMEOUT

Door Award: page(MACHINE_PAGE), PRI_GAME_QUICK6, D_QUEUED+D_PAUSE
Select mode: page(MACHINE2_PAGE), PRI_GAME_QUICK7, D_QUEUED+D_PAUSE

PB Detect: page(MACHINE_PAGE), PRI_GAME_QUICK8, D_QUEUED+D_TIMEOUT+D_PAUSE
PB Loop: page(MACHINE_PAGE), PRI_GAME_LOW8, D_QUEUED+D_TIMEOUT
Loop: page(MACHINE_PAGE), PRI_GAME_LOW4, D_RESTARTABLE+D_SCORE

Lock Lit: page(MACHINE_PAGE), PRI_GAME_QUICK8, D_QUEUED+D_TIMEOUT
Ball From Lock: page(MACHINE_PAGE), PRI_JACKPOT, D_PAUSE+D_QUEUED

#Multiball deffs
MB Lit: page(MACHINE_PAGE), PRI_GAME_MODE8, D_QUEUED+D_TIMEOUT
MB Start: page(MACHINE_PAGE), PRI_GAME_QUICK8, D_PAUSE+D_QUEUED
MB Running: page(MACHINE_PAGE), runner, PRI_GAME_MODE7, D_QUEUED+D_TIMEOUT
Jackpot Relit: page(MACHINE_PAGE), PRI_GAME_QUICK8, D_ABORTABLE
MBall Restart: page(MACHINE_PAGE), runner, PRI_GAME_LOW3, D_QUEUED+D_TIMEOUT
MB Jackpot Collected: page(MACHINE_PAGE), PRI_JACKPOT, D_PAUSE+D_QUEUED
MB Ten Million Added: page(MACHINE_PAGE), PRI_JACKPOT, D_PAUSE+D_QUEUED+D_RESTARTABLE

#I prefer the Jackpot animation to be shown over the replay, hence the oddness
Replay: page(MACHINE_PAGE), PRI_JACKPOT, D_PAUSE+D_QUEUED+D_TIMEOUT
Jackpot: page(MACHINE_PAGE), PRI_REPLAY, D_PAUSE+D_QUEUED
Special: page(MACHINE_PAGE), PRI_SPECIAL

ChaosMB Running: page(MACHINE_PAGE), runner, PRI_GAME_MODE6
Chaos Jackpot: page(MACHINE_PAGE), PRI_GAME_QUICK8, D_PAUSE+D_QUEUED

TBC: page(MACHINE3_PAGE), PRI_JACKPOT, D_QUEUED+D_PAUSE
BTTZ Running: page(MACHINE3_PAGE), runner, PRI_GAME_MODE7, D_QUEUED+D_TIMEOUT
BTTZ End: page(MACHINE3_PAGE), PRI_TILT, D_QUEUED

Rollover Completed: page(MACHINE3_PAGE), PRI_GAME_LOW3
Ball Drain Outlane: page(MACHINE_PAGE), PRI_BALLSAVE, D_RESTARTABLE
Ball Explode: page(MACHINE_PAGE), PRI_JACKPOT, D_RESTARTABLE
TZ Ball Save: page(MACHINE_PAGE), PRI_BALLSAVE, D_RESTARTABLE
Two Way Combo: page(MACHINE_PAGE), PRI_GAME_QUICK7, D_PAUSE+D_QUEUED
Three Way Combo: page(MACHINE_PAGE), PRI_GAME_QUICK7, D_PAUSE+D_QUEUED
In the lead: page(MACHINE_PAGE), PRI_JACKPOT, D_PAUSE+D_QUEUED
Home and Dry: page(MACHINE_PAGE), PRI_JACKPOT, D_PAUSE+D_QUEUED
PB Jackpot: page(MACHINE_PAGE), PRI_JACKPOT, D_PAUSE+D_QUEUED

Lucky Bounce: page(MACHINE_PAGE), PRI_GAME_QUICK6, D_PAUSE+D_QUEUED
Shoot Camera: page(MACHINE3_PAGE), PRI_GAME_QUICK8, D_RESTARTABLE
Shoot Jackpot: page(MACHINE_PAGE), PRI_GAME_QUICK5, D_RESTARTABLE+D_TIMEOUT+D_QUEUED
Button Masher: page(MACHINE_PAGE), PRI_JACKPOT, D_PAUSE+D_QUEUED

Shoot Power Payoff: page(MACHINE3_PAGE), PRI_GAME_QUICK5, D_RESTARTABLE+D_TIMEOUT+D_QUEUED

#Doink mode
Get ready to Doink: page(MACHINE_PAGE), PRI_JACKPOT, D_QUEUED+D_TIMEOUT
TNF: page(MACHINE4_PAGE), PRI_JACKPOT, D_QUEUED+D_PAUSE
TNF exit: page(MACHINE4_PAGE), PRI_GAME_QUICK8, D_QUEUED+D_PAUSE
Shoot Right Loop: page(MACHINE_PAGE), PRI_GAME_QUICK3

Bonus: page(MACHINE4_PAGE), runner, PRI_BONUS
Score to beat: page(MACHINE4_PAGE), PRI_JACKPOT, D_PAUSE+D_QUEUED

Backdoor Award: page(MACHINE2_PAGE), PRI_JACKPOT, D_QUEUED+D_PAUSE
Spiralaward Collected: page(MACHINE2_PAGE), PRI_GAME_LOW5, D_QUEUED+D_PAUSE+D_TIMEOUT
Camera Award: page(MACHINE2_PAGE), PRI_GAME_QUICK8, D_QUEUED+D_TIMEOUT+D_PAUSE

Rules: page(MACHINE3_PAGE), PRI_EGG1

Pinwheel: page(MACHINE_PAGE), PRI_GAME_QUICK3
Snake: page(MACHINE3_PAGE), PRI_GAME_VMODE, D_PAUSE+D_QUEUED
Snake Ready: page(MACHINE3_PAGE), PRI_GAME_VMODE, D_PAUSE+D_QUEUED
Driver: page(MACHINE_PAGE), PRI_GAME_QUICK7
Explosion: page(MACHINE_PAGE), PRI_EGG1

Paused: page(MACHINE2_PAGE), PRI_STATUS, D_PAUSE+D_QUEUED
Alarm: page(MACHINE2_PAGE), PRI_SLAMTILT, D_PAUSE+D_QUEUED
Anti Cradle: page(MACHINE_PAGE), PRI_TILT, D_PAUSE+D_QUEUED

Hurryup Mode: page(MACHINE3_PAGE), runner, PRI_GAME_MODE7, D_QUEUED+D_TIMEOUT
Hurryup awarded: page(MACHINE3_PAGE), PRI_JACKPOT, D_PAUSE+D_QUEUED

#Match loop: page(MACHINE3_PAGE), PRI_MATCH, D_PAUSE+D_QUEUED

Oddchange grows: page(MACHINE2_PAGE), PRI_GAME_QUICK3, D_RESTARTABLE
Oddchange collected: page(MACHINE2_PAGE), PRI_GAME_QUICK4, D_QUEUED+D_TIMEOUT+D_SCORE
Bitmap test: page(MACHINE2_PAGE), PRI_GAME_QUICK5

SDSS Ready: page(MACHINE4_PAGE), PRI_GAME_MODE6, D_PAUSE
SDSS Awarded: page(MACHINE4_PAGE), PRI_JACKPOT, D_PAUSE

##########################################################################
# Lamp effects
##########################################################################
[leffs]

perlinclude machine/tz/leff.pl

Bonus: runner, PRI_BONUS, LAMPS(ALL), GI(ALL), page(MACHINE2_PAGE)
Select Mode: runner, PRI_LEFF7, LAMPS(SELECT_MODE), GI(ALL), page(MACHINE2_PAGE)
GI Cycle: PRI_LEFF3, GI(ALL), page(MACHINE2_PAGE)
Flasher Happy: shared, PRI_LEFF1, page(MACHINE2_PAGE)
Left Ramp: shared, PRI_LEFF2, page(MACHINE2_PAGE)
No GI: PRI_LEFF1, GI(ALL), page(MACHINE2_PAGE)
Piano Jackpot Collected: PRI_LEFF1, LAMPS(ALL), GI(ALL), page(MACHINE2_PAGE)
Flash GI: PRI_LEFF2, GI(ALL), page(MACHINE2_PAGE)
Flash GI2: PRI_LEFF2, GI(ALL), page(MACHINE2_PAGE)
Flash All: PRI_LEFF5, LAMPS(AMODE_ALL), page(MACHINE2_PAGE)
Slot Kickout: PRI_LEFF1, GI(ALL), page(MACHINE2_PAGE)
Gumball Strobe: PRI_LEFF2, LAMPS(ALL), GI(ALL), page(MACHINE2_PAGE)
Clock Target: PRI_LEFF1, GI(ALL), page(MACHINE2_PAGE)
Game Timeout: PRI_TILT, GI(ALL), page(MACHINE2_PAGE)
Clock Start: PRI_LEFF4, GI(ALL), c_decl(clock_round_started_leff), page(MACHINE2_PAGE)
MB Running: shared, PRI_LEFF2, LAMPS(DOOR_LOCKS_AND_GUMBALL), c_decl(multiball_running_leff), page(MACHINE2_PAGE)
Strobe Up: PRI_LEFF2, LAMPS(ALL), , GI(ALL), page(MACHINE2_PAGE)
Strobe Down: PRI_LEFF2, LAMPS(ALL), GI(ALL), page(MACHINE2_PAGE)
Multi Strobe: PRI_LEFF2, LAMPS(ALL), page(MACHINE2_PAGE)
Door Strobe: PRI_LEFF3, LAMPS(DOOR_PANELS), GI(ALL), page(MACHINE2_PAGE)
Right Loop: PRI_LEFF1, LAMPS(SORT4), page(MACHINE2_PAGE)
Left Loop: PRI_LEFF1, LAMPS(SORT3), page(MACHINE2_PAGE)
Jets Active: shared, PRI_LEFF3, LAMPS(JETS), page(MACHINE2_PAGE)
Circle Out: PRI_LEFF3, LAMPS(CIRCLE_OUT), page(MACHINE2_PAGE)
Color Cycle: PRI_LEFF3, LAMPS(AMODE_ALL), GI(ALL), page(MACHINE2_PAGE)
Lock: PRI_LEFF4, LAMPS(LOCK_TEST), page(MACHINE2_PAGE)
MPF Active: shared, PRI_LEFF4, LAMPS(POWERFIELD_VALUES), page(MACHINE2_PAGE)
MPF Hit: PRI_LEFF5, LAMPS(ALL), GI(ALL), page(MACHINE2_PAGE)
Rocket: PRI_LEFF2, LAMPS(ALL), GI(ALL), page(MACHINE2_PAGE)
Powerball Announce: PRI_LEFF4, LAMPS(ALL), GI(ALL), page(MACHINE2_PAGE)
Amode: runner, PRI_LEFF1, LAMPS(AMODE_ALL), GI(ALL), page(MACHINE2_PAGE)
Spiralaward: shared, PRI_LEFF5, LAMPS(SPIRAL_AWARDS), page(MACHINE2_PAGE)
Rules: runner, PRI_TILT, LAMPS(ALL), GI(ALL), page(MACHINE2_PAGE)

##########################################################################
# Fonts used in this game.
##########################################################################
[fonts]
times10:
twizone:
steel:

[timers]
Rocket Flipper:
R Ramp to MPF:
MPF Top to Camera:

