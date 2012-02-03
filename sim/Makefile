# This Makefile is used to control a simulation build, where the code
# runs on the same machine as the build.  It assumes that the normal,
# non-simulation platform makefile has already been included, then it
# redefines those things which are necessarily different for simulation.

D := sim
NATIVE_OBJS :=

# Common simulation CPU configuration
CPU := native
$(eval $(call have,CONFIG_PTH))
CONFIG_UI ?= curses
$(eval $(call have,CONFIG_SOFT_REALTIME))
include cpu/$(CPU)/Makefile
include $(PMAKEFILE)

# Additional defines
# TODO : Is CONFIG_PLATFORM_WPC needed?
CFLAGS += -I$(D) -DCONFIG_PLATFORM_WPC -DPAGE=0

# Additional object files to be linked into the kernel region
NATIVE_OBJS += $(D)/main.o $(D)/switch.o $(D)/section.o \
	$(D)/timing.o $(D)/watchdog.o \
	$(D)/signal.o $(D)/hwtimer.o $(D)/sound.o

NATIVE_OBJS += $(if $(CONFIG_AC), $(D)/zerocross.o)
NATIVE_OBJS += $(D)/coil.o
NATIVE_OBJS += $(D)/script.o
NATIVE_OBJS += $(D)/conf.o
NATIVE_OBJS += $(D)/node.o
NATIVE_OBJS += $(D)/io.o
NATIVE_OBJS += $(D)/keyboard.o
NATIVE_OBJS += $(if $(CONFIG_PLATFORM_WPC), $(D)/io_wpc.o)
NATIVE_OBJS += $(if $(CONFIG_PIC), $(D)/wpc-pic.o)
ifeq ($(MACHINE),tz)
$(eval $(call have,CONFIG_MACHINE_SIM))
NATIVE_OBJS += $(D)/tz_sim.o
endif

# For ASCII DMD
NATIVE_OBJS += $(if $(CONFIG_DMD), $(D)/asciidmd.o)
NATIVE_OBJS += $(if $(CONFIG_ALPHA), $(D)/segment.o)
NATIVE_OBJS += $(if $(CONFIG_DMD), tools/imglib/imglib.o)
NATIVE_OBJS += $(if $(CONFIG_DMD), cpu/native/dot.o)
$(D)/asciidmd.o : CFLAGS += -Itools/imglib

# For Ubuntu 8.10 and higher: The default compiler flags will try to
# detect buffer overflows, but we are doing ugly things to read/write
# persistent memory.  We need to disable this 'feature' for this file
# only.
$(D)/section.o : CFLAGS += -U_FORTIFY_SOURCE

$(NATIVE_OBJS) : CFLAGS += -DNATIVE_SYSTEM $(UI_CFLAGS)

# Add machine type flags
CFLAGS += $(if $(CONFIG_DMD), -DMACHINE_DMD=1)

CFLAGS += -DMACHINE_SHORTNAME=\"$(MACHINE)\"

# CALLSET_ENTRYs in the simulator directory work too!
CALLSET_FLAGS += -D $(D)

# Invoke 'make attach' to start up a GDB session that attaches itself to
# a running instance of the native mode program.
attach:
	gdb -p `ps -ef | grep $(NATIVE_PROG) | head -n 1 | grep -o "[0-9].*" | awk '{print $$1}'` -x gdbmacros
