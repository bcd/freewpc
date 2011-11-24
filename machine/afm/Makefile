
#
# Game specific Makefile extensions
#

MACHINE_FILE = afm.md

# Define the name of the GAME_ROM.  This is
# optional; you can delete it and a suitable
# default will be taken.
GAME_ROM_PREFIX = fafm

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
ROM_PAGE_COUNT = 64

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
MACHINE_MINOR = 32

PINMAME_MACHINE = afm_113

PINMAME_GAME_ROM = afm_1_13.bin

PINMAME_OTHER_ROMS = afm_s*.l1

CONFIG_FLEX := y

#
# Object files exported by this game
#

GAME_PAGED_OBJS = afm_leff.o misc.o shot.o rules.o deffs.o \
	ebflex.o

GAME_INCLUDES =

IMAGE_MAP += $(M)/afm.ild

