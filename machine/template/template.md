#--------------------------------------------------------------------------
# Machine description for FreeWPC
# (C) Copyright 2007, 2009 by Brian Dominy <brian@oddchange.com>
#
# See tools/genmachine for more information about the format of this file.
#--------------------------------------------------------------------------

# This file describes many characteristics of a pinball machine,
# mostly physical stuff, although some rules can be coded here also.

##########################################################################
# General section (before a [section] header is given.
# Miscellaneous parameters are specified here.
##########################################################################
Title: xyz
DMD: Yes/No
Fliptronic: Yes/No
DCS: Yes/No
WPC95: Yes/No

# Uncomment and set the following if you know the filename for this
# machine used by PinMAME.
#Pinmame-Zip:
#Pinmame-ROM:
#Lamp-Matrix-Width:
#Lamp-Matrix-Height:

##########################################################################
# Include standard definitions that apply to all WPC games.
# This will set some defaults for things you leave out here.
##########################################################################
# Choose ONE of the following:
#include platform/wpc/wpc89.md
#include platform/wpc/wpc-dmd.md
#include platform/wpc/wpc-fliptronic.md
#include platform/wpc/wpc-dcs.md
#include platform/wpc/wpc-security.md
#include platform/wpc/wpc95.md

##########################################################################
# Use 'define' to emit a plain #define for anything not covered by
# some other means.
##########################################################################
#define MACHINE_SOL_EXTBOARD1
#define MACHINE_CUSTOM_AMODE
#define MACHINE_SCORE_DIGITS
#define MACHINE_MUSIC_GAME
#define MACHINE_MUSIC_PLUNGER
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


##########################################################################
# General Illumination
##########################################################################
[gi]


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

##########################################################################
# Per-player bit flags.
##########################################################################
[flags]

##########################################################################
# System-wide bit flags.
##########################################################################
[globalflags]

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
