# Subplatform support
# Eventually, this section will contain conditionals for building on the
# different WPC variations.  For now, all of these are mandatory though.
CONFIG_WPC=y
CONFIG_FONT=y
CONFIG_AC=y
CONFIG_ANIMATION=y

D := platform/native

# Additional CFLAGS required for a native build
# TODO : don't define CONFIG_LITTLE_ENDIAN on sparc, etc.
PTH_CFLAGS := $(shell pth-config --cflags)
CFLAGS += -O -I$(PTH_CFLAGS) -I$(D) -DCONFIG_NATIVE -DCONFIG_NATIVE -DCONFIG_PLATFORM_WPC -g -DCONFIG_LITTLE_ENDIAN -pg
HOST_LFLAGS += -pg
HOST_LIBS += -lpth

# Additional object files to be linked into the kernel region
KERNEL_OBJS += $(D)/main.o $(D)/task.o $(D)/section.o

# Machine-specific simulation files
#ifeq ($(MACHINE),tz)
#KERNEL_OBJS += $(D)/tz_sim.o
#endif

# When using the curses UI (default for now)
CFLAGS += -DCONFIG_UI -DCURSES
HOST_LIBS += -lncurses
KERNEL_OBJS += $(D)/ui_curses.o

# CALLSET_ENTRYs in the simulator directory work too!
CALLSET_FLAGS += -D $(D)

# Invoke 'make attach' to start up a GDB session that attaches itself to
# a running instance of the native mode program.
attach:
	gdb -p `ps -ef | grep freewpc | head -n 1 | grep -o "[0-9].*" | awk '{print $$1}'` -x gdbmacros
