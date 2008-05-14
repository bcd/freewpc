
# Root directory for the platform files
P := platform/wpcsound
CONFIG_BARE := y

# Common 6809 configuration
CPU := m6809
include cpu/$(CPU)/Makefile

# The default CFLAGS required on the WPC Sound Board
# A few notes:
# 1. -Wno-format is to not check format strings, because we define those
# differently than ANSI C does (e.g. look at sprintf()).
INT_CFLAGS = -mint8
CFLAGS += $(INT_CFLAGS) -DHAVE_FASTRAM_ATTRIBUTE -DHAVE_NVRAM -mdirect -DCONFIG_PLATFORM_WPCSOUND -mwpc -fno-builtin -DCONFIG_PINMAME -DCONFIG_BIG_ENDIAN -mcode-section=.text -mdata-section=.text -mbss-section=ram -Wno-format
EXTRA_ASFLAGS += -DCONFIG_PLATFORM_WPCSOUND

KERNEL_OBJS += $(P)/main.o $(P)/interrupt.o $(P)/volume.o $(P)/host.o \
	$(P)/dac.o $(P)/cvsd.o
KERNEL_ASM_OBJS += $(P)/start.o

