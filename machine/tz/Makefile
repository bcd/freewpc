
#
# Game specific Makefile extensions
#

MACHINE_FILE := tz.md
MACHINE_SCHEDULE := $(M)/tz.sched

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

MACHINE_MAJOR = 1
MACHINE_MINOR = 0

PINMAME_MACHINE = tz_92

PINMAME_GAME_ROM = tzone9_2.rom

PINMAME_OTHER_ROMS = tzu*.rom

# Enable optional features of the OS which we always want.
$(eval $(call have,CONFIG_MUTE_PAUSE))


#
# Object files exported by this game
#
# GAME_OBJS go into the system page, which should be a last resort or for common
# functions, or for those which have to for some reason.  All feature tests can
# go into GAME_TEST_OBJS.  Preferably, game functions go into GAME_PAGED_OBJS.
# This works safely for any file that only uses CALLSET_ENTRY functions, deffs,
# and leffs.  RTTs must go into GAME_OBJS.
#

GAME_OBJS = jets.o autofire.o gumball.o \
	config.o loop.o rampdiv.o magnet.o \
	maghelper.o clock.o

GAME_TEST_OBJS = gumball_test.o powerball_test.o clocktest.o magtest.o ac_detect.o

GAME_INCLUDES =

GAME_PAGED_OBJS = leftramp.o rightramp.o skill.o \
	fastlock.o door.o slot.o sling.o \
	mpf.o chaosmb.o powerball.o \
	multiball.o sssmb.o deffs.o

GAME2_OBJS = unlit.o spiralaward.o leffs.o star.o rocket.o tz_amode.o \
	outhole.o camera.o deadend.o hitch.o birthday.o alarm.o dixon.o \
	music.o oddchange.o shots.o mode_select.o bitmap_test.o

GAME3_OBJS = tz_flipcode.o rules.o greed.o clockmillions.o \
	spiral.o bttz.o factoids.o master.o snake.o \
	lock.o lanes.o combo.o deffs2.o hurryup.o

GAME4_OBJS = bonus.o dollar.o bar.o tnf.o sdss.o

IMAGE_MAP += $(M)/tz.ild

