
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

MACHINE_SCHEDULE := $(M)/wcs.sched

PINMAME_MACHINE = wcs_l2

PINMAME_GAME_ROM = wcup_lx2.rom

PINMAME_OTHER_ROMS = wcup_u*.rom

#
# Object files exported by this game
#

GAME_OBJS =

GAME_TEST_OBJS =

GAME_INCLUDES =

GAME_PAGED_OBJS = goalie_driver.o amode.o pitypoints.o shot.o

FIF_SRCS += $(M)/soccerball.fif $(M)/striker.fif $(M)/pgj.fif

SCHED_HEADERS += build/sling0.h build/sling1.h build/sling2.h build/sling3.h build/sling4.h build/spinner0.h
