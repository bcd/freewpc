
#
# Game specific Makefile extensions
#

# Define the name of the GAME_ROM.  This is
# optional; you can delete it and a suitable
# default will be taken.
GAME_ROM_PREFIX = fwcs

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

MACHINE_FILE = wcs.md

MACHINE_MAJOR = 0
MACHINE_MINOR = 62

IMAGE_MAP += $(M)/wcs.ild

MACHINE_SCHEDULE := $(M)/wcs.sched

PINMAME_MACHINE = wcs_l2

PINMAME_GAME_ROM = wcup_lx2.rom

PINMAME_OTHER_ROMS = wcup_u*.rom

#
# Object files exported by this game
#

GAME_OBJS =

GAME_TEST_OBJS = goalie_test.o

GAME_INCLUDES =

GAME_PAGED_OBJS = goalie_driver.o amode.o pitypoints.o \
	shot.o lock.o driver.o kickback.o freekick.o \
	toplane.o deffs.o jets.o goal_round.o \
	combo.o multiball.o tv.o leffs.o mode.o ultra.o \
	effect.o skill.o rampdiv.o magna_goalie.o penalty.o \
	quickmb.o city.o final.o multiplier.o ball_driver.o

