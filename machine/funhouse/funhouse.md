#--------------------------------------------------------------------------
# Machine description for Funhouse
# (C) Copyright 2007 by Brian Dominy <brian@oddchange.com>
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
DMD: No
Fliptronic: No
DCS: No
WPC95: No
#Pinmame-Zip:
#Pinmame-ROM:
#Lamp-Matrix-Width:
#Lamp-Matrix-Height:

##########################################################################
# Include standard definitions that apply to all WPC games.
# This will set some defaults for things you leave out here.
##########################################################################
include platform/wpc/wpc.md

##########################################################################
# Use 'define' to emit a plain #define for anything not covered by
# some other means.
##########################################################################
define MACHINE_LACKS_ALL_INCLUDES
# define MACHINE_SYS11_SOUND

#define MACHINE_INCLUDE_FLAGS
#define MACHINE_SOL_EXTBOARD1
#define MACHINE_CUSTOM_AMODE
#define MACHINE_SCORE_DIGITS
#define MACHINE_MUSIC_GAME
#define MACHINE_MUSIC_PLUNGER
#define MACHINE_REPLAY_CODE_TO_SCORE
#define MACHINE_DEBUGGER_HOOK
#define MACHINE_REPLAY_SCORE_CHOICES 10
#define MACHINE_OUTHOLE_KICK_HOOK

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
17: Mirror Quick MB
18: Mirror Open Gate
21: Mirror Super Dog
22: Mirror Bumpers
23: Mirror Million
24: Mirror Ex. Ball
25: Extra Ball
26: Super Dog
27: Step S
28: Step P

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
14: Plumb Bob Tilt, c_decl(sw_tilt), tilt, ingame, noplay
21: Slam Tilt, slam-tilt, ingame, cabinet
23: Ticket Opto, opto, noplay
25: Lock Right
27: Lock Center
28: Lock Left
41: Left Slingshot
44: Wind Tunnel Hole
45: Trap Door
46: Rudys Hideout
51: Dummy Jaw, opto
53: Right Slingshot
58: Tunnel Kickout
62: Right Plunger, shooter
63: Right Trough
65: Dummy Eject Hole
67: Lower Right Hole
68: Lower Jet
72: Left Trough
73: Outhole, outhole, noscore
74: Center Trough
76: Trap Door Closed, noplay
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
H7: Knocker
H8: Lock Release
L1: Upper Left Jet
L2: Upper Right Jet
L3: Lower Jet
L4: Left Slingshot
L5: Right Slingshot
L6: Steps Gate
L7: Ball Release
L8: Dummy Eject Hole

G1: Blue Flashers
G2: Dummy Flasher
G3: Clock Flashers
G4: Super Dog Flash.
G5: Mouth Motor
G6: Motor Direction
G7: Red Flashers
G8: Clear Flashers

A1: Eyes Right
A2: Eyelids Open
A3: Eyelids Closed
A4: Eyes Left

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

[targetbanks]

[shots]

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

##########################################################################
# Music calls for well-known events
##########################################################################
[system_music]

##########################################################################
# A list of all scores needed by the game rules.
##########################################################################
[scores]

##########################################################################
# The default high scores.  Use GC to indicate the grand champion.
# The parameters are the initials followed by the score value.  Periods
# may optionally be used to group digits, but they are not necessary.
# Commas _cannot_ be used for this purpose since they separate parameters.
##########################################################################
[highscores]
GC: LED, 15.000.000
1: BCD, 10.000.000
2: QQQ, 9.000.000
3: DEH, 8.000.000
4: JND, 7.000.000

##########################################################################
# Bit flags.
##########################################################################
[flags]

##########################################################################
# Display effects
##########################################################################
[deffs]

##########################################################################
# Lamp effects
##########################################################################
[leffs]

##########################################################################
# Fonts used in this game.
##########################################################################
[fonts]
