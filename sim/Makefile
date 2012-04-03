# This Makefile is used to control a simulation build, where the code
# runs on the same machine as the build.  It assumes that the normal,
# non-simulation platform makefile has already been included, then it
# redefines those things which are necessarily different for simulation.

D := sim
NATIVE_OBJS :=

# Common simulation CPU configuration
CPU ?= native
$(eval $(call have,CONFIG_SIM))
$(eval $(call have,CONFIG_PTH))
$(eval $(call have,CONFIG_CALLIO))
CONFIG_UI ?= curses
include cpu/$(CPU)/Makefile
include $(PMAKEFILE)

# Additional defines
CFLAGS += -I$(D) -DPAGE=0

# Additional object files to be linked into the kernel region
NATIVE_OBJS += $(D)/main.o $(D)/switch.o \
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
NATIVE_OBJS += $(if $(CONFIG_PLATFORM_MIN), $(D)/io_min.o)
NATIVE_OBJS += $(if $(CONFIG_PLATFORM_P2K), $(D)/io_p2k.o)
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

$(NATIVE_OBJS) : CFLAGS += -DNATIVE_SYSTEM $(UI_CFLAGS)

# Add machine type flags
CFLAGS += $(if $(CONFIG_DMD), -DMACHINE_DMD=1)

# CALLSET_ENTRYs in the simulator directory work too!
CALLSET_FLAGS += -D $(D)

