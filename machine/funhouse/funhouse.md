#--------------------------------------------------------------------------
# Machine description for Funhouse
# (C) Copyright 2007, 2009, 2010, 2011 by Brian Dominy <brian@oddchange.com>
#
# See tools/genmachine for more information about the format of this file.
#--------------------------------------------------------------------------

# This file describes many characteristics of a pinball machine,
# mostly physical stuff, although some rules can be coded here also.

##########################################################################
# General section (before a [section] header is given.
# Miscellaneous parameters are specified here.
##########################################################################
Title: Fun House
include platform/wpc/wpc89.md

#Pinmame-Zip:
#Pinmame-ROM:
#Lamp-Matrix-Width:
#Lamp-Matrix-Height:

##########################################################################
# Use 'define' to emit a plain #define for anything not covered by
# some other means.
##########################################################################
# define MACHINE_SYS11_SOUND
#define MACHINE_SCORE_DIGITS
#define MACHINE_MUSIC_GAME
#define MACHINE_MUSIC_PLUNGER
define MACHINE_BALL_SAVE_TIME 0

# Define replay values
# Replay ranges from 3M-20M in 500K steps
# Replay boost ranges from 500K-5M in 500K steps
define REPLAY_TEN_THOUSANDS
define REPLAY_SCORE_MIN 300
define REPLAY_SCORE_STEP 50
define REPLAY_SCORE_MAX 2000
define REPLAY_SCORE_DEFAULT 500
define REPLAY_BOOST_MIN 50
define REPLAY_BOOST_STEP 50
define REPLAY_BOOST_MAX 500
define REPLAY_BOOST_DEFAULT 100

define MACHINE_CUSTOM_AMODE
define MACHINE_GRAND_CHAMPION_INITIALS { 'L', 'E', 'D' }
define MACHINE_GRAND_CHAMPION_SCORE { 0x00, 0x15, 0x00, 0x00, 0x00 }
define MACHINE_HIGH_SCORE_INITIALS { 'B', 'C', 'D' }, { 'Q', 'Q', 'Q' }, { 'D', 'E', 'H' }, { 'J', 'N', 'D' }
define MACHINE_HIGH_SCORES { 0x00, 0x10, 0x00, 0x00, 0x00 }, { 0x00, 0x09, 0x00, 0x00, 0x00 }, { 0x00, 0x08, 0x00, 0x00, 0x00 }, { 0x00, 0x07, 0x00, 0x00, 0x00 }

##########################################################################
# Lamp Description
# The key is given in column/row format.  The first parameter must be
# the lamp name.  Optionally you can specify a color, and x() and y()
# for the location on the playfield.  Only a subset of lamp colors are
# recognized; see tools/genmachine for details.  Lamp location is
# given in terms of the Lamp-Matrix-Width and Lamp-Matrix-Height.
##########################################################################
[lamps]
11: Gangway 75K
12: Gangway 100K
13: Gangway 150K
14: Gangway 200K
15: Gangway 250K
16: Gangway Ex. Ball
17: Shoot Again, shoot-again
18: Steps Gate Open
21: Clock 45 Min.
22: Clock Hour 8
23: Clock Hour 6
24: Clock 25 Min.
25: Clock 15 Min.
26: Clock 10 Min.
27: Clock Hour 12
28: Clock 0 Min.
31: Clock 40 Min.
32: Clock 35 Min.
33: Clock 30 Min.
34: Clock 20 Min.
35: Clock Hour 3
36: Clock Hour 1
37: Clock Hour 11
38: Clock 50 Min.
41: Clock Hour 9
42: Clock Hour 7
43: Clock Hour 5
44: Clock Hour 4
45: Clock Hour 2
46: Clock 5 Min.
47: Clock 55 Min.
48: Clock Hour 10
51: Lower Jet
52: Upper Left Jet
53: Super Dog
54: Steps Frenzy
55: Steps E.B.
56: Steps 500K
57: Ramp Entrance
58: Right Gangway
61: Flipper Lanes
62: Step S
63: Trap Door Bonus
64: Ramp 250K
65: Step T
66: Left Gangway
67: Extra Ball, extra-ball
68: Lock
71: Mirror Ex. Ball
72: Upper Right Jet
73: Step P
74: Mirror Million
75: Mirror Bumpers
76: Mirror Super Dog
77: Mirror Open Gate
78: Mirror Quick MB
81: Million Plus
82: Specials
83: Frenzy
84: Ramp Steps
85: Mirror Value
86: Step E
87: Million
88: Start Button, start, cabinet

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
11: Right Flipper, button, intest, c_decl(sw_right_button)
12: Left Flipper, button, intest, c_decl(sw_left_button)
13: Start Button, start-button, intest
14: Plumb Bob Tilt, c_decl(sw_tilt), cabinet, tilt, ingame, noplay
15: Steps Frenzy
16: Upper Ramp
17: Step S, lamp(LM_STEP_S)
18: Upper Left Jet
21: Slam Tilt, slam-tilt, ingame, cabinet
23: Ticket Opto, cabinet, opto, noplay
25: Lock Right
26: Step Extra Ball
27: Lock Center
28: Lock Left
31: Step P, lamp(LM_STEP_P)
32: Superdog High, lamp(LM_SUPER_DOG)
33: Left Gangway
34: Superdog Low, lamp(LM_SUPER_DOG)
35: Step Track Lower
36: Step 500K
37: Superdog Center, lamp(LM_SUPER_DOG)
38: Step Track Upper
41: Left Slingshot
42: Left Inlane
43: Left Outlane
44: Wind Tunnel Hole
45: Trap Door
46: Rudys Hideout
47: Left Plunger, edge, debounce(TIME_200MS)
48: Ramp Exit
51: Dummy Jaw, opto
52: Right Outlane, noplay
53: Right Slingshot
54: Step T, lamp(LM_STEP_T)
55: Step Exit, opto
56: Ramp Enter
57: Jet Lane, noplay
58: Tunnel Kickout
61: Inner Right Inlane
62: Right Plunger, shooter, edge, noplay, debounce(TIME_200MS)
63: Right Trough, trough, noscore, noplay
64: Step E, lamp(LM_STEP_E)
65: Dummy Eject Hole
66: Right Gangway
67: Lower Right Hole
68: Lower Jet
71: Outer Right Inlane
72: Left Trough, trough, noscore, noplay
73: Outhole, outhole, noscore
74: Center Trough, trough, noscore, noplay
75: Upper Loop
76: Trap Door Closed, noplay, service
77: Upper Right Jet

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
H1: Outhole
H2: Ramp Diverter
H3: Rudys Hideout
H4: Tunnel Kickout
H5: Trap Door Open
H6: Trap Door Close
H7: Knocker, knocker
H8: Lock Release

L1: Upper Left Jet
L2: Upper Right Jet
L3: Lower Jet
L4: Left Slingshot
L5: Right Slingshot
L6: Steps Gate, nosearch
L7: Ball Release, ballserve
L8: Dummy Eject Hole

G1: Blue Flashers, flash
G2: Dummy Flasher, flash
G3: Clock Flashers, flash
G4: Super Dog Flash., flash
G5: Mouth Motor, motor
G6: Motor Direction, motor
G7: Red Flashers, flash
G8: Clear Flashers, flash

A1: Eyes Right, nosearch
A2: Eyelids Open, nosearch
A3: Eyelids Closed, nosearch
A4: Eyes Left, nosearch

##########################################################################
# General Illumination
##########################################################################
[gi]
0: Backglass Upper
1: Rudy Backglass
2: Rear Playfield
3: Backglass Center
4: Front Playfield
5: All Illumination

##########################################################################
# Tests
# These are additional test items that should appear in the TESTS menu.
##########################################################################
[tests]


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
# should be used to select and sort the lamps.  Some functions are
# builtin to genmachine; others can be defined by you and included
# via the 'perlinclude' directive.
#
# Note that a lampset may contain only one lamp.  The lampset is the
# unit of 'allocation' for a lamp effect.
##########################################################################
[lamplists]
Gangways: Gangway 75K, Gangway 100K, Gangway 150K, Gangway 200K, Gangway 250K, Gangway Ex. Ball
Clock Minutes: Clock 0 Min., Clock 5 Min., Clock 10 Min., Clock 15 Min., Clock 20 Min., Clock 25 Min., Clock 30 Min., Clock 35 Min., Clock 40 Min., Clock 45 Min., Clock 50 Min., Clock 55 Min.
Clock Hours: Clock Hour 12, Clock Hour 1, Clock Hour 2, Clock Hour 3, Clock Hour 4, Clock Hour 5, Clock Hour 6, Clock Hour 7, Clock Hour 8, Clock Hour 9, Clock Hour 10, Clock Hour 11
Mirror Awards: Mirror Ex. Ball, Mirror Million, Mirror Bumpers, Mirror Super Dog, Mirror Open Gate, Mirror Quick MB
Jets: Upper Left Jet, Upper Right Jet, Lower Jet
Trap Door Lamps: Frenzy, Trap Door Bonus, Million Plus
Top Loop: Lock, Extra Ball
Steps Awards: Steps Frenzy, Steps E.B., Steps 500K
Step Targets: Step S, Step T, Step E, Step P
Ramp Awards: Ramp Entrance, Ramp Steps, Ramp 250K
Lower Lanes: Specials, Flipper Lanes
Circle Out: PF:lamp_sort_circle_out
Strobe Up: PF:lamp_sort_bottom_to_top
Strobe Down: PF:lamp_sort_top_to_bottom
Strobe Left: PF:lamp_sort_right_to_left
Strobe Right: PF:lamp_sort_left_to_right
Clock Hours Minutes: Clock Hours, Clock Minutes
Major PF: Gangways, Clock Minutes, Clock Hours, Mirror Awards, Ramp Awards, Steps Awards
Playfield: PF:all

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
Trough: Ball Release, trough, init_max_count(3), \
	Left Trough, Center Trough, Right Trough
Lock: Lock Release, init_max_count(0), \
	Lock Right, Lock Center, Lock Left
Tunnel: Tunnel Kickout, init_max_count(0), Tunnel Kickout
Rudy: Dummy Eject Hole, init_max_count(0), Dummy Eject Hole
Hideout: Rudys Hideout, init_max_count(0), Rudys Hideout

#------------------------------------------------------------------------
# The remaining sections describe software aspects, and not the physical
# machine.
#------------------------------------------------------------------------

##########################################################################
# Items for the Feature Adjustments menu.  Parameters indicate the
# type of adjustment and the default value.
##########################################################################
[adjustments]
Clock 1st Start: integer, 8
Clock 2nd Start: integer, 5
Disable Dummy: yes_no, NO
Disable Trapdoor: yes_no, NO
Disable Gate: yes_no, NO

##########################################################################
# Items for the Feature Audits menu.
##########################################################################
[audits]
Rudy Hits: INT
Rudy Gulps: INT
Mirror Awards: INT
Multiball: INT
Million Plus: INT
Million Shots: INT
Ramp Shots: INT
Skill Shots: INT
Frenzy: INT
Super Frenzy: INT
Trap Door Opened: INT
Trap Door Made: INT
3 Way Combo: INT
4 Way Combo: INT
Special Lit: INT

##########################################################################
# Sound calls for well-known events
##########################################################################
[system_sounds]
Add Coin: SND_COIN
Tilt Warning: SND_TILT_WARNING
Tilt: SND_TILT

##########################################################################
# Music calls for well-known events
##########################################################################
[system_music]
Start Ball: MUS_MAIN_PLUNGER
Ball In Play: MUS_MAIN
End Game: MUS_MAIN
Volume Change: MUS_RESTART_RUNNING

##########################################################################
# A list of all scores needed by the game rules.
##########################################################################
[scores]
110:
1K:
2570:
5K:
5130:
10K:
25K:
50K:
75K:
100K:
125K:
150K:
175K:
200K:
225K:
250K:
275K:
300K:
500K:
750K:
1M:


##########################################################################
# Bit flags.
##########################################################################
[flags]
Mirror Lit:
Million Lit:
Multiball Lit:
Multiball Running:
Quick MB Running:
Jackpot Lit:
Frenzy Lit:
Super Frenzy Lit:
Super Frenzy Running:
Jackpot This Ball:
Ball At Steps:
Mirror Complete:
Steps Ramp Lit:
Steps Open:
Outlanes Lit:

##########################################################################
# Display effects
##########################################################################
[deffs]
Mirror Collect: page(MACHINE_PAGE), PRI_GAME_QUICK6, D_QUEUED+D_PAUSE
Bonus: page(MACHINE_PAGE), PRI_BONUS
Gangway Collect: page(MACHINE_PAGE), PRI_GAME_QUICK3, D_SCORE
Gangway EB Lit: page(MACHINE_PAGE), PRI_GAME_QUICK5
QuickMB Started: page(MACHINE_PAGE), PRI_GAME_QUICK4, D_QUEUED+D_PAUSE
QuickMB Running: runner, page(MACHINE_PAGE), PRI_GAME_MODE3
QuickMB Score: page(MACHINE_PAGE), PRI_GAME_QUICK6, D_SCORE
MB 1130: page(MACHINE_PAGE), PRI_GAME_QUICK4, D_PAUSE
MB 1145: page(MACHINE_PAGE), PRI_GAME_QUICK4
MB Start: page(MACHINE_PAGE), PRI_GAME_QUICK6, D_PAUSE
MB Running: runner, page(MACHINE_PAGE), PRI_GAME_MODE6
Superdog Running: page(MACHINE_PAGE), PRI_GAME_MODE2
SuperDog Score: page(MACHINE_PAGE), PRI_GAME_QUICK2, D_SCORE
Frenzy Running: page(MACHINE_PAGE), PRI_GAME_MODE5
Rules: page(MACHINE_PAGE), PRI_EGG1

##########################################################################
# Lamp effects
##########################################################################
[leffs]
Amode: runner, PRI_LEFF1, LAMPS(PLAYFIELD), GI(ALL), page(MACHINE_PAGE)
Circle Out: PRI_LEFF3, LAMPS(CIRCLE_OUT), page(MACHINE_PAGE)
Clock Vibrate: PRI_LEFF4, LAMPS(CLOCK_HOURS_MINUTES), page(MACHINE_PAGE)
Gangway Strobe: PRI_LEFF4, LAMPS(GANGWAYS), page(MACHINE_PAGE)
Jackpot: PRI_LEFF8, LAMPS(CIRCLE_OUT), page(MACHINE_PAGE)
Shooter: PRI_LEFF2, page(MACHINE_PAGE)
Superdog Score: PRI_LEFF3, page(MACHINE_PAGE)
Midnight: PRI_LEFF4, LAMPS(PLAYFIELD), GI(ALL), page(MACHINE_PAGE)
Rudy Hit: PRI_LEFF2, page(MACHINE_PAGE)
High Score: runner, PRI_LEFF4, LAMPS(PLAYFIELD), GI(ALL), page(MACHINE_PAGE)

[timers]
Tunnel Entered:
Ignore Jaw:
Ramp Just Entered:
Left Loop Started:
Right Loop Started:

[templates]

Left Sling: driver(sling), sw=SW_LEFT_SLINGSHOT, sol=SOL_LEFT_SLINGSHOT
Right Sling: driver(sling), sw=SW_RIGHT_SLINGSHOT, sol=SOL_RIGHT_SLINGSHOT
Left Jet: driver(jet), sw=SW_UPPER_LEFT_JET, sol=SOL_UPPER_LEFT_JET
Right Jet: driver(jet), sw=SW_UPPER_RIGHT_JET, sol=SOL_UPPER_RIGHT_JET
Lower Jet: driver(jet), sw=SW_LOWER_JET, sol=SOL_LOWER_JET

Ramp Div: driver(duty),
	sol=SOL_RAMP_DIVERTER,
	ontime=TIME_100MS, duty_ontime=TIME_16MS, duty_offtime=TIME_50MS, \
	timeout=TIME_3S

Steps Gate: driver(duty),
	sol=SOL_STEPS_GATE,
	ontime=TIME_100MS, duty_ontime=TIME_16MS, duty_offtime=TIME_50MS, \
	timeout=0

Eye Direction: driver(bivar),
	forward_sol=SOL_EYES_RIGHT, reverse_sol=SOL_EYES_LEFT

Outhole: driver(outhole), sol=SOL_OUTHOLE, swno=SW_OUTHOLE, swevent=sw_outhole
