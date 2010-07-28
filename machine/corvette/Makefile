
#
# Game specific Makefile extensions
#

# Define the name of the GAME_ROM.  This is
# optional; you can delete it and a suitable
# default will be taken.
GAME_ROM_PREFIX = fcorv

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

MACHINE_FILE = corvette.md

MACHINE_MAJOR = 0
MACHINE_MINOR = 61

IMAGE_MAP += $(M)/corvette.ild

MACHINE_SCHEDULE := $(M)/corvette.sched

PINMAME_MACHINE = corv_21

PINMAME_GAME_ROM = corv_2_1.rom

PINMAME_OTHER_ROMS = corvsnd*

#
# Object files exported by this game
#

GAME_OBJS =

GAME_TEST_OBJS = zr1_test.o

GAME_INCLUDES =

GAME_PAGED_OBJS = simple.o driver.o kickback.o \
	jets.o trivial.o sling.o zr_1_multiball.o shots.o \
	leffs.o deffs.o skill.o zr1.o

SCHED_HEADERS += build/kickback_driver.h build/spinner.h build/loop_gate.h \
	build/zr_1_up_rev_gate.h build/zr_1_low_rev_gate.h

