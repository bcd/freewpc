
# Root directory for native mode compiler.
CPU := m6809
C := cpu/$(CPU)

CFLAGS =

# TODO : don't define CONFIG_LITTLE_ENDIAN on sparc, etc.

# Additional CFLAGS required for a native build
CFLAGS += -O -g -DCONFIG_NATIVE -DCONFIG_LITTLE_ENDIAN

ifeq ($(CONFIG_PTH),y)
PTH_CFLAGS := $(shell pth-config --cflags)
HOST_LIBS += -lpth
CFLAGS += $(PTH_CFLAGS)
endif

ifeq ($(CONFIG_NATIVE_PROFILE),y)
CFLAGS += -pg
HOST_LFLAGS += -pg
endif

ifeq ($(CONFIG_CURSES), y)
CFLAGS += -DCONFIG_UI -DCURSES
HOST_LIBS += -lncurses
NATIVE_OBJS += $(D)/ui_curses.o
endif
