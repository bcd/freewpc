
#
# Game specific Makefile extensions
#

MACHINE_FILE := tz.md

MACHINE_SCHEDULE := $(M)/tz.sched

MACHINE_SCHED_FLAGS := -i tz/driver.h

# Define the name of the GAME_ROM.  This is
# optional; you can delete it and a suitable
# default will be taken.
GAME_ROM_PREFIX = ftz

# Set ROM_PAGE_COUNT to the number of 16KB pages
# that should be present in the output image.
# The hardware supports a maximum of 64 pages;
# you can't exceed this.
#
# The 'system' will always reside in the uppermost
# two pages.  If the ROM is less than a full 64
# pages, then the pages are numbered from [N..64]
# anyway.
#
# 128KB.....8 pages
# 256KB.....16 pages
# 512KB.....32 pages
# 1MB...... 64 pages
#
ROM_PAGE_COUNT = 32

#################################################
#
# Simulation options.
#
# These options control the way that the game
# ROM is made available to pinmame for testing.
# They are only used during installation into
# the pinmame roms directory.
#
#################################################

MACHINE_MAJOR = 0
MACHINE_MINOR = 85

PINMAME_MACHINE = tz_92

PINMAME_GAME_ROM = tzone9_2.rom

PINMAME_OTHER_ROMS = tzu*.rom

#
# Object files exported by this game
#
# GAME_OBJS go into the system page, which should be a last resort or for common
# functions, or for those which have to for some reason.  All feature tests can
# go into GAME_TEST_OBJS.  Preferably, game functions go into GAME_PAGED_OBJS.
# This works safely for any file that only uses CALLSET_ENTRY functions, deffs,
# and leffs.  RTTs must go into GAME_OBJS.
#

SCHED_HEADERS += build/left_sling.h build/right_sling.h build/left_jet.h build/right_jet.h build/lower_jet.h

GAME_OBJS = clock.o magnet.o sling.o jets.o autofire.o gumball.o \
	rampdiv.o lock.o door.o

GAME_TEST_OBJS = clocktest.o gumball_test.o magtest.o powerball_test.o

GAME_INCLUDES =

GAME_PAGED_OBJS = lanes.o leftramp.o skill.o deadend.o camera.o \
	config.o rightramp.o hitch.o piano.o rocket.o slot.o greed.o \
	mpf.o powerball.o clocktarget.o vmode.o deffs.o bonus.o star.o \
	multiball.o loop.o sssmb.o chaosmb.o leffs.o amode.o

GAME_FSMS = shot.fsm

FIF_SRCS += images/cow.fif images/spiral.fif images/mborder.fif

FIF_SRCS += $(M)/ballborder.fif $(M)/brianhead.fif $(M)/city.fif \
	$(M)/coinstack.fif $(M)/hitcher.fif $(M)/oldcar.fif \
	$(M)/pyramid.fif $(M)/robot.fif

IMAGE_MAP += $(M)/tz.ild

SCHED_HEADERS += build/clock_mech.h build/bridge_open.h build/shooter_div.h \
	build/ramp_div.h build/gumball_div.h


