
# Root directory for native mode compiler.
CPU := native
C := cpu/$(CPU)

CFLAGS =

# TODO : don't define CONFIG_LITTLE_ENDIAN on sparc, etc.

# Additional CFLAGS required for a native build
CFLAGS += -O -g -DCONFIG_NATIVE -DCONFIG_LITTLE_ENDIAN

HOST_LIBS += -lm

ifeq ($(CONFIG_PTH),y)
PTH_CFLAGS := $(shell pth-config --cflags)
HOST_LIBS += -lpth
CFLAGS += $(PTH_CFLAGS)
NATIVE_OBJS += $(C)/task_pth.o
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

ifeq ($(CONFIG_SOFT_REALTIME), y)
NATIVE_OBJS += $(C)/realtime.o
endif

NATIVE_OBJS += $(C)/bits.o
