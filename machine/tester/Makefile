
#
# Game specific Makefile extensions
#

MACHINE_FILE = tester.md

ifeq ($(hw),)
hw=WPC_TEST_DMD
endif

EXTRA_CFLAGS += -D$(hw)

CONFIG_PLAYABLE := n

# Define the name of the GAME_ROM.  This is
# optional; you can delete it and a suitable
# default will be taken.
GAME_ROM_PREFIX = wpct

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
MACHINE_MINOR = 31

PINMAME_MACHINE = tz_92

PINMAME_GAME_ROM = tzone9_2.rom

PINMAME_OTHER_ROMS = tzu*.rom

#
# Object files exported by this game
#

GAME_OBJS = 

GAME_TEST_OBJS = 

GAME_INCLUDES =

GAME_PAGED_OBJS =
