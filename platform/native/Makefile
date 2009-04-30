# This Makefile is used to control a native build, where the code
# runs on the same machine as the build.  It is slightly different
# because of toolchain differences, and also simulation files need
# to be linked in.

# Subplatform support
# Eventually, this section will contain conditionals for building on the
# different WPC variations.  For now, all of these are mandatory though.
CONFIG_WPC=y
CONFIG_FONT ?= $(CONFIG_DMD)
CONFIG_AC=y
CONFIG_ANIMATION ?= $(CONFIG_DMD)

D := platform/native

# Additional CFLAGS required for a native build
# TODO : don't define CONFIG_LITTLE_ENDIAN on sparc, etc.
PTH_CFLAGS := $(shell pth-config --cflags)
CFLAGS += -O $(PTH_CFLAGS) -I$(D) -DCONFIG_NATIVE -DCONFIG_PLATFORM_WPC -g -DCONFIG_LITTLE_ENDIAN
HOST_LIBS += -lpth

ifeq ($(CONFIG_NATIVE_PROFILE),y)
CFLAGS += -pg
HOST_LFLAGS += -pg
endif

# Additional object files to be linked into the kernel region
NATIVE_OBJS := $(D)/main.o $(D)/switch.o $(D)/task.o $(D)/section.o \
	$(D)/bcd_string.o \
	$(D)/timing.o $(D)/watchdog.o $(D)/zerocross.o \
	$(D)/ball.o $(D)/signal.o $(D)/hwtimer.o $(D)/sound.o \
	$(D)/coil.o

# For ASCII DMD
NATIVE_OBJS += $(if $(CONFIG_DMD), $(D)/asciidmd.o, $(D)/segment.o)
NATIVE_OBJS += $(if $(CONFIG_DMD), tools/imglib/imglib.o)
$(D)/asciidmd.o : CFLAGS += -Itools/imglib

# When using the curses UI (default for now)
CFLAGS += -DCONFIG_UI -DCURSES
HOST_LIBS += -lncurses
NATIVE_OBJS += $(D)/ui_curses.o

$(NATIVE_OBJS) : CFLAGS += -DNATIVE_SYSTEM

# Add machine type flags
CFLAGS += $(if $(CONFIG_DMD), -DMACHINE_DMD=1)

CFLAGS += -DMACHINE_SHORTNAME=\"$(MACHINE)\"

IMAGE_MAP += platform/wpc/wpc.ild

# CALLSET_ENTRYs in the simulator directory work too!
CALLSET_FLAGS += -D $(D)

FIRST_BANK=0

# Invoke 'make attach' to start up a GDB session that attaches itself to
# a running instance of the native mode program.
attach:
	gdb -p `ps -ef | grep freewpc | head -n 1 | grep -o "[0-9].*" | awk '{print $$1}'` -x gdbmacros
