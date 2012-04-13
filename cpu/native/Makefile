ifndef NATIVE_OBJS

# Root directory for native mode compiler.
CPU := native
C := cpu/$(CPU)

GCC_ROOT ?= /usr/bin
GCC_CROSS ?=
CC := $(GCC_ROOT)/$(GCC_CROSS)gcc
AS := $(CC) -xassembler-with-cpp
LD := $(GCC_ROOT)/$(GCC_CROSS)gcc
REQUIRED += $(CC) $(LD)

GCC_VERSION = NATIVE
CFLAGS =
HOST_LFLAGS += -Wl,-Map=build/freewpc.map
HOST_LFLAGS += -Wl,--section-start=.globalflag=0x8100000
HOST_LFLAGS += -Wl,--section-start=.flag=0x8100100
HOST_LFLAGS += -Wl,--section-start=.freetimer=0x8100200

# TODO : don't define CONFIG_LITTLE_ENDIAN on sparc, etc.

# Additional CFLAGS required for a native build
CFLAGS += -O -g -DCONFIG_NATIVE -DCONFIG_LITTLE_ENDIAN -DMACHINE_SHORTNAME=\"$(MACHINE)\"

HOST_LIBS += -lm

ifeq ($(CONFIG_PTH),y)
PTH_CFLAGS := $(shell pth-config --cflags)
HOST_LIBS += -lpth
CFLAGS += $(PTH_CFLAGS)
NATIVE_OBJS += $(C)/task_pth.o
endif

ifeq ($(CONFIG_PTHREADS),y)
HOST_LIBS += -lpthread
NATIVE_OBJS += $(C)/task_pthread.o
endif

ifeq ($(CONFIG_NATIVE_PROFILE),y)
CFLAGS += -pg
HOST_LFLAGS += -pg
endif

ifeq ($(CONFIG_NATIVE_COVERAGE),y)
CFLAGS += -fprofile-arcs -ftest-coverage
HOST_LIBS += -lgcov
endif

ifeq ($(CONFIG_UI), curses)
CFLAGS += -DCONFIG_UI -DCURSES
CFLAGS += -I$(shell ncurses5-config --includedir)
HOST_LIBS += -lncurses
NATIVE_OBJS += $(D)/ui_curses.o
endif

ifeq ($(CONFIG_UI), gtk)
CFLAGS += -DCONFIG_UI -DCONFIG_GTK
UI_CFLAGS += $(shell pkg-config --cflags gtk+-2.0)
HOST_LIBS += $(shell pkg-config --libs gtk+-2.0) -export-dynamic
NATIVE_OBJS += $(D)/ui_gtk.o
endif

ifeq ($(CONFIG_UI), console)
CFLAGS += -DCONFIG_UI -DCONFIG_UI_CONSOLE
NATIVE_OBJS += $(D)/ui_console.o
endif

ifeq ($(CONFIG_UI), remote)
CFLAGS += -DCONFIG_UI -DCONFIG_UI_REMOTE
NATIVE_OBJS += $(D)/ui_remote.o
endif

ifeq ($(CONFIG_UI), sdl)
CFLAGS += -DCONFIG_UI -DCONFIG_UI_SDL
UI_CFLAGS += $(shell sdl-config --cflags)
HOST_LIBS += $(shell sdl-config --libs) -lSDL_ttf
NATIVE_OBJS += $(D)/ui_sdl.o
endif

ifeq ($(CONFIG_LINUX_GPIO),y)
NATIVE_OBJS += $(C)/gpio.o
endif

ifeq ($(CONFIG_LINUX_INPUT),y)
NATIVE_OBJS += $(C)/input.o
endif

NATIVE_OBJS += $(C)/bits.o
NATIVE_OBJS += $(C)/bcd_string.o
NATIVE_OBJS += $(C)/log.o
NATIVE_OBJS += $(C)/main.o
NATIVE_OBJS += $(C)/realtime.o
NATIVE_OBJS += $(C)/section.o

# For Ubuntu 8.10 and higher: The default compiler flags will try to
# detect buffer overflows, but we are doing ugly things to read/write
# persistent memory.  We need to disable this 'feature' for this file
# only.
$(C)/section.o : CFLAGS += -U_FORTIFY_SOURCE

.PHONY : run
run: $(NATIVE_PROG)
	$(NATIVE_PROG)

# Invoke 'make attach' to start up a GDB session that attaches itself to
# a running instance of the native mode program.
.PHONY : attach
attach:
	gdb -p `ps -ef | grep $(NATIVE_PROG) | head -n 1 | grep -o "[0-9].*" | awk '{print $$1}'` -x gdbmacros

endif
