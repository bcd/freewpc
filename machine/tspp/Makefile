
#
# Game specific Makefile extensions
#

PLATFORM := whitestar

MACHINE_FILE := tspp.md

# Define the name of the GAME_ROM.  This is
# optional; you can delete it and a suitable
# default will be taken.
GAME_ROM_PREFIX = fspp

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
ROM_PAGE_COUNT = 8

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

PINMAME_MACHINE = simpprty
PINMAME_GAME_ROM = spp-cpu.500
PINMAME_OTHER_ROMS = spp1* sppdispa.500

#
# Object files exported by this game
#
# GAME_OBJS go into the system page, which should be a last resort or for common
# functions, or for those which have to for some reason.  All feature tests can
# go into GAME_TEST_OBJS.  Preferably, game functions go into GAME_PAGED_OBJS.
# This works safely for any file that only uses CALLSET_ENTRY functions, deffs,
# and leffs.  RTTs must go into GAME_OBJS.
#

GAME_OBJS = 

GAME_TEST_OBJS = 

GAME_INCLUDES =

GAME_PAGED_OBJS = 

