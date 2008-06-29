
# Root directory for the platform files
P := platform/wpc-shell
CONFIG_BARE := y

CPU := m6809
include cpu/$(CPU)/Makefile

# A few notes:
# 1. -Wno-format is to not check format strings, because we define those
# differently than ANSI C does (e.g. look at sprintf()).
INT_CFLAGS = -mint8
CFLAGS = $(INT_CFLAGS) -DHAVE_NVRAM -mdirect -DCONFIG_PLATFORM_WPC -mwpc -fno-builtin -DCONFIG_PINMAME -mcode-section=.text -mdata-section=.text -mbss-section=ram -Wno-format

# Optional CFLAGS based on configuration options

# Optimization flags
CFLAGS += -O2 -fomit-frame-pointer -fstrength-reduce -frerun-loop-opt -Wunknown-pragmas -foptimize-sibling-calls -fstrict-aliasing -fregmove -fgcse-after-reload -fgcse-sm -fgcse-las -ffreestanding
# TODO : should -funroll-loops be given?

KERNEL_OBJS += $(P)/main.o
KERNEL_ASM_OBJS += $(P)/start.o

ROM_PAGE_COUNT = 32

PINMAME_MACHINE = tz_92
PINMAME_GAME_ROM = tzone9_2.rom
PINMAME_OTHER_ROMS = tzu*.rom
