
# Additional CFLAGS required for a native build
# TODO : don't define CONFIG_LITTLE_ENDIAN on sparc, etc.
CFLAGS += -O -I`pth-config --cflags` -Iplatform/linux -DCONFIG_NATIVE -DCONFIG_NATIVE -DCONFIG_PLATFORM_WPC -g -DCONFIG_LITTLE_ENDIAN -pg
HOST_LFLAGS += -pg
HOST_LIBS += -lpth

# Additional object files to be linked into the kernel region
KERNEL_OBJS += platform/linux/main.o platform/linux/task.o platform/linux/section.o

# When using the curses UI (default for now)
CFLAGS += -DCONFIG_UI -DCURSES
HOST_LIBS += -lncurses
KERNEL_OBJS += platform/linux/ui_curses.o

# CALLSET_ENTRYs in the simulator directory work too!
CALLSET_FLAGS += -D platform/linux

attach:
	gdb -p `ps -ef | grep freewpc | head -n 1 | awk '{print $$2}'` -x gdbmacros
