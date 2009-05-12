# The configuration for the WPC platform

CONFIG_GAME_ROM := y

# Root directory for the platform files
P := platform/wpc

# Common 6809 configuration
CPU := m6809
CONFIG_TASK := y
CONFIG_MALLOC := y
include cpu/$(CPU)/Makefile

# Subplatform support
# Eventually, this section will contain conditionals for building on the
# different WPC variations.  For now, all of these are mandatory though.
CONFIG_WPC ?= y
CONFIG_FONT ?= $(CONFIG_DMD)
CONFIG_AC ?= y
CONFIG_ANIMATION ?= $(CONFIG_DMD)

# The default CFLAGS required on the WPC platform
# A few notes:
# 1. -Wno-format is to not check format strings, because we define those
# differently than ANSI C does (e.g. look at sprintf()).
#
# 2. -mint8 is used for the 6809 compiler to define int as an 8-bit value.
# This is non-standard but it generates much better code.  It should be
# possible to build without this, though.
#
INT_CFLAGS = -mint8
# TODO : Don't hardcode code/data sections here like this!
CFLAGS += $(INT_CFLAGS) -DHAVE_NVRAM -mdirect -DCONFIG_PLATFORM_WPC -mwpc -fno-builtin -mcode-section=.text -mdata-section=.text -mbss-section=ram -Wno-format
EXTRA_ASFLAGS += -DCONFIG_PLATFORM_WPC

# Say which source files need to be compiled in for the WPC platform only.
# Use KERNEL_HW_OBJS for C sources, and KERNEL_ASM_OBJS for assembler files.
KERNEL_HW_OBJS += $(P)/main.o
KERNEL_HW_OBJS += $(P)/interrupt.o
KERNEL_HW_OBJS += $(P)/init-wpc.o
KERNEL_ASM_OBJS += $(if $(CONFIG_DMD), $(P)/bitmap.o)
KERNEL_ASM_OBJS += $(if $(CONFIG_DMD), $(P)/dmd.o)
KERNEL_ASM_OBJS += $(if $(CONFIG_DMD), $(P)/dot.o)
KERNEL_ASM_OBJS += $(if $(CONFIG_DMD), $(P)/shadow.o)
KERNEL_ASM_OBJS += $(P)/start.o

# On PIC games, we need to do some complex 32-bit math.  The standard
# library functions in the 6809 toolchain do not work, so there is a
# replacement in math.c.  However, since this is only needed at
# initialization, put it into the COMMON_PAGE.
COMMON_BASIC_OBJS += $(if $(CONFIG_PIC), $(P)/math.o)

# Import images that are common to all games.
IMAGE_MAP += $(P)/wpc.ild

BOTTOM_BANK=$(shell echo $$((62 - $(NUM_PAGED_SECTIONS) )))

FIRST_BANK=$(shell echo $$((64 - $(ROM_PAGE_COUNT) )))

# The math code must be compiled in 16-bit mode, since it interfaces
# to the system libraries which expect this.
$(P)/math.o : INT_CFLAGS=-mint16

# The startup code needs to know the number of paged regions in the ROM
# to checksum.  The pages between BOTTOM_BANK and TOP_BANK are paged in
# and contribute to the total checksum.  TOP_BANK is fixed but
# BOTTOM_BANK varies according to the machine-specific ROM usage.
$(P)/start.o : EXTRA_ASFLAGS += -DTOP_BANK=61 -DBOTTOM_BANK=$(BOTTOM_BANK)
