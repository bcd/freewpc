
#
# Game specific Makefile extensions
#

MACHINE_FILE = funhouse.md

MACHINE_SCHEDULE := $(M)/funhouse.sched

# Define the name of the GAME_ROM.  This is
# optional; you can delete it and a suitable
# default will be taken.
GAME_ROM_PREFIX = ffh

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
ROM_PAGE_COUNT = 16  # L9 uses a larger ROM

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

PINMAME_MACHINE = fh_l9

PINMAME_GAME_ROM = funh_l9.rom

PINMAME_OTHER_ROMS = fh_u*

#
# Object files exported by this game
#

GAME_OBJS =

GAME_PAGED_OBJS = shot.o leffs.o rudy.o trivial.o mirror.o \
	mb.o clock.o steps.o quickmb.o superdog.o jets.o

GAME_INCLUDES =

SCHED_HEADERS += build/left_sling.h build/right_sling.h \
	build/left_jet.h build/right_jet.h build/lower_jet.h \
	build/ramp_div.h build/eye_direction.h build/steps_gate.h

