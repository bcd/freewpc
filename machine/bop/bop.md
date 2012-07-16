#--------------------------------------------------------------------------
# Machine description for The Machine: Bride of Pinbot
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
Title: Bride of Pinbot
include platform/wpc/wpc89.md

#Pinmame-Zip:
#Pinmame-ROM:
#Lamp-Matrix-Width:
#Lamp-Matrix-Height:

##########################################################################
# Use 'define' to emit a plain #define for anything not covered by
# some other means.
##########################################################################
#define MACHINE_SYS11_SOUND
#define MACHINE_SCORE_DIGITS
#define MACHINE_MUSIC_GAME
#define MACHINE_MUSIC_PLUNGER
#define MACHINE_REPLAY_SCORE_CHOICES       10
#define MACHINE_REPLAY_START_CHOICE        5
define MACHINE_BALL_SAVE_TIME             0
define MACHINE_MAX_BALLS                  2

#define MACHINE_CUSTOM_AMODE
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
11: Left Outlane
12: Left Inlane
13: Right Inlane
14: Right Outlane
15: Left Standup
16: Right Top Standup
17: Right Bottom Standup
18: Shoot Again, shoot-again
21: SW Lite Jackpot
22: BW Lite Billion
23: BW Extra Ball
24: SW Lite Extra Ball
25: SW 50K
26: SW 100K
27: Values x2
28: Spin SW
31: SW 250K
32: BW 10M
33: BW 50M
34: BW Special
35: BW 5M
36: BW 1M
37: Space Shuttle
38: Launch Pad
41: Skill Shot 50K
42: Skill Shot 75K
43: Skill Shot 100K
44: Skill Shot 200K
45: Skill Shot 25K
46: Left Eye
47: Right Eye
48: Mouth
51: Left Loop 500K
52: Left Loop 100K
53: Left Loop 50K
54: Left Loop 25K
55: Right Loop 500K
56: Right Loop 100K
57: Right Loop 50K
58: Right Loop 25K
61: Centre Ramp 100K
62: Centre Ramp 500K
63: Centre Ramp 1M
64: Wire ball lock
65: Jet 500K
66: Jet 100K
67: Jet 50K
68: Jet 25K
71: Jackpot 8M
72: Jackpot 7M
73: Jackpot 6M
74: Jackpot 5M
75: Jackpot 4M
76: Jackpot 3M
77: Jackpot 2M
78: Jackpot 1M
81: Backglass Hip
82: Backglass Middle Leg
83: Backglass Knee
84: Backglass Foot
85: Backglass Shoulder
86: MPF 100K
87: MPF 200K
88: MPF 300K


##########################################################################
# Switch Description
# The key is in column/row format.  The first parameter must be the switch
# name.  Options can be given in any order:
#    ingame - only service the switch during a game
#    intest - also service the switch in test mode
#    novalid - tripping this switch does NOT mark ball in play
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
14: Plumb bob tilt, c_decl(sw_tilt), cabinet, tilt, ingame, novalid
15: Left Outlane
16: Left Inlane
17: Right Inlane
18: Right Outlane
21: Slam Tilt, slam-tilt, ingame, cabinet
23: Ticket Opto, cabinet, opto, novalid
25: Trough Right, noscore
26: Trough Center, noscore
27: Trough Left, noscore
28: Left Standup, standup
31: Skill Shot 50K
32: Skill Shot 75K
33: Skill Shot 100K
34: Skill Shot 200K
35: Skill Shot 25K
36: Right Top Standup, standup
37: Right Bottom Standup, standup
38: Outhole, outhole, noscore
41: Centre Ramp Made
43: Left Loop, ingame
44: Right Loop Top, ingame
45: Right Loop Bottom, ingame
46: Under Playfield Kickback
47: Enter Head
51: Spinner, ingame
52: Shooter, shooter, edge, novalid, debounce(TIME_200MS)
53: UR Jet, ingame, c_decl(jet_hit)
54: UL Jet, ingame, c_decl(jet_hit)
55: Lower Jet, ingame, c_decl(jet_hit)
56: Jet Sling, ingame, c_decl(jet_hit)
57: Left Sling, ingame
58: Right Sling, ingame
63: Left Eye, intest
64: Right Eye, intest
65: Mouth, intest
67: Face Position, edge, intest
71: Wireform Top
72: Wireform Bottom
73: Enter MPF
74: MPF Exit Left
75: MPF Exit Right
76: Left Ramp Enter
77: Centre Ramp Enter

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
H2: Trough Release, ballserve
H3: UPF Kicker
H4: MPF Gate
H5: SShot Kicker
H6: Wire Post
H7: Knocker, knocker
H8: Mouth, nosearch

L1: UL Jet
L2: Left Sling
L3: UR Jet
L4: Right Sling
L5: Lower Jet
L6: Jet Sling
L7: Left Eye, nosearch
L8: Right Eye, nosearch

G1: Billion, flash
G2: Left Ramp, flash
G3: Jackpot, flash
G4: Skill Shot, flash
G5: Left Helmet, flash
G6: Right Helmet, flash
G7: Jets Enter, flash
G8: Left Loop, flash

A1: Helmet Data, nosearch
A2: Helmet Clock, nosearch
A3: Head Motor Relay, nosearch
A4: Head Motor, motor, nosearch

##########################################################################
# General Illumination
##########################################################################
[gi]
0: Backglass and body
1: Helmet
2: Rear playfield
3: Backglass only
4: Front Playfield
7: Flippers

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
Playfield: PF:all
Lanes: Left Outlane..Right Outlane
Left loops: Left Loop 500K..Left Loop 25K
Right loops: Right Loop 500K..Right Loop 25K
Skillshot: Skill Shot 50K..Skill Shot 25K

[containers]
Trough: trough, Trough Release, Trough Right, Trough Center, Trough Left, init_max_count(2)

Head: Wire Post, init_max_count(0), Enter Head, Wireform Top, Wireform Bottom

UPFKicker: UPF Kicker, init_max_count(0), Under Playfield Kickback

SShotKicker: SShot Kicker, init_max_count(0), Skill Shot 50K

#------------------------------------------------------------------------
# The remaining sections describe software aspects, and not the physical
# machine.
#------------------------------------------------------------------------

##########################################################################
# Items for the Feature Adjustments menu.  Parameters indicate the
# type of adjustment and the default value.
##########################################################################
[adjustments]

##########################################################################
# Items for the Feature Audits menu.
##########################################################################
[audits]

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
Start Ball: MUS_MAIN1_PLUNGER
Ball In Play: MUS_MAIN1_1
End Game: MUS_GAME_OVER
Volume Change: MUS_SECRET_FANFARE

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
Skillshot enabled:

##########################################################################
# Display effects
##########################################################################
[deffs]
Skillshot: page(MACHINE_PAGE), PRI_GAME_QUICK6, D_QUEUED+D_PAUSE+D_SCORE
Jet Hit: page(MACHINE_PAGE), PRI_GAME_QUICK2, D_RESTARTABLE
Jets Level Up: page(MACHINE_PAGE), PRI_GAME_MODE3, D_QUEUED
Loop: page(MACHINE_PAGE), PRI_GAME_QUICK4, D_RESTARTABLE+D_SCORE
Centre Ramp: page(MACHINE_PAGE), PRI_GAME_QUICK5, D_RESTARTABLE
Shuttle Launch: page(MACHINE_PAGE), PRI_GAME_QUICK5, D_RESTARTABLE
Rollover Completed: page(MACHINE_PAGE), PRI_GAME_QUICK2, D_RESTARTABLE

##########################################################################
# Lamp effects
##########################################################################
[leffs]
Amode: runner, PRI_LEFF1, LAMPS(PLAYFIELD), GI(ALL), page(MACHINE_PAGE)
#Circle Out: PRI_LEFF3, LAMPS(CIRCLE_OUT), page(MACHINE_PAGE)
#Shooter: PRI_LEFF2, page(MACHINE_PAGE)
#Skillshot: PRI_LEFF2, page(MACHINE_PAGE), LAMPS(SKILLSHOT)

[timers]

[templates]

Left Sling: driver(spsol), sw=SW_LEFT_SLING, sol=SOL_LEFT_SLING, ontime=4, offtime=20

Right Sling: driver(spsol), sw=SW_RIGHT_SLING, sol=SOL_RIGHT_SLING, ontime=4, offtime=20

Jet Sling: driver(spsol), sw=SW_JET_SLING, sol=SOL_JET_SLING, ontime=4, offtime=20

UL Jet: driver(spsol), sw=SW_UL_JET, sol=SOL_UL_JET, ontime=4, offtime=20

UR Jet: driver(spsol), sw=SW_UR_JET, sol=SOL_UR_JET, ontime=4, offtime=20

Lower Jet: driver(spsol), sw=SW_LOWER_JET, sol=SOL_LOWER_JET, ontime=4, offtime=20

Spinner: driver(spinner), sw_event=sw_spinner, sw_number=SW_SPINNER

Gate: driver(duty), sol=SOL_MPF_GATE, ontime=TIME_300MS, duty_ontime=TIME_33MS, duty_offtime=TIME_16MS, timeout=60

Head Motor: driver(duty), sol=SOL_HEAD_MOTOR, ontime=0, duty_ontime=TIME_33MS, duty_offtime=TIME_16MS, timeout=20
Head Motor Relay: driver(duty), sol=SOL_HEAD_MOTOR_RELAY, ontime=0, duty_ontime=TIME_33MS, duty_offtime=TIME_16MS, timeout=20

Outhole: driver(outhole), sol=SOL_OUTHOLE, swno=SW_OUTHOLE, swevent=sw_outhole
