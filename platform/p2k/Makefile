
D := sim
P := platform/p2k
NATIVE_OBJS :=

# Common native mode CPU configuration
PLATFORM := p2k
CPU := native
CONFIG_PTH := y
# CONFIG_CURSES := y
CONFIG_SOFT_REALTIME := y
include cpu/$(CPU)/Makefile

# FreeWPC module configuration
$(eval $(call have,CONFIG_DMD))
$(eval $(call have,CONFIG_DMD_STUB))
$(eval $(call have,CONFIG_FONT))
$(eval $(call have,CONFIG_AC))
$(eval $(call nohave,CONFIG_GI))
$(eval $(call have,CONFIG_ANIMATION))

# Platform dependent files
#KERNEL_HW_OBJS += $(P)/interrupt.o

# Additional defines
CFLAGS += -I$(D) -DCONFIG_PLATFORM_P2K -DPAGE=0

# Additional object files to be linked into the kernel region
NATIVE_OBJS += $(P)/main.o
# NATIVE_OBJS += $(D)/task.o
NATIVE_OBJS += $(D)/bcd_string.o

#
#NATIVE_OBJS += $(D)/switch.o $(D)/section.o \
#	$(D)/timing.o $(D)/watchdog.o $(D)/zerocross.o \
#	$(D)/ball.o $(D)/signal.o $(D)/hwtimer.o $(D)/sound.o \
#	$(D)/coil.o $(D)/script.o $(D)/conf.o

CFLAGS += -DMACHINE_SHORTNAME=\"$(MACHINE)\"

# CALLSET_ENTRYs in the simulator directory work too!
CALLSET_FLAGS += -D $(P)

FIRST_BANK=0

# Invoke 'make attach' to start up a GDB session that attaches itself to
# a running instance of the native mode program.
attach:
	gdb -p `ps -ef | grep $(NATIVE_PROG) | head -n 1 | grep -o "[0-9].*" | awk '{print $$1}'` -x gdbmacros
