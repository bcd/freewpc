# The configuration for the WPC platform

CONFIG_GAME_ROM := y

# Root directory for the platform files
P := platform/wpc

# Common 6809 configuration
CPU := m6809
$(eval $(call have,CONFIG_PERIODIC_IRQ))
CONFIG_TASK := y
include cpu/$(CPU)/Makefile

# Subplatform support
# Eventually, this section will contain conditionals for building on the
# different WPC variations.  For now, all of these are mandatory though.
$(eval $(call have,CONFIG_PLATFORM_WPC))
$(eval $(call have,CONFIG_MMIO))
CONFIG_FONT ?= $(CONFIG_DMD)
$(eval $(call have,CONFIG_AC))
$(eval $(call have,CONFIG_GI))
$(eval $(call have,CONFIG_TRIAC))
$(eval $(call have,CONFIG_RTC))
$(eval $(call have,CONFIG_BUYIN))
CONFIG_SCORE_RANK := y

# ROM bank switching
CONFIG_ROM_BANK_SIZE := 16
CONFIG_FIXED_PAGE_COUNT := 2
CONFIG_MAX_ROM_PAGES := 64
# Alphanumeric games like Funhouse are restricted to 128KB total,
# so only 6 non-system pages.  DMD games are allowed to use more.
ifeq ($(CONFIG_DMD),y)
CONFIG_CODE_PAGE_LIST := 52 53 54 55
else
CONFIG_CODE_PAGE_LIST :=
endif
CONFIG_CODE_PAGE_LIST += 56 57 58 59 60 61
CONFIG_SYSTEM_CODE_PAGE := 62

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
CFLAGS += $(INT_CFLAGS) -DHAVE_NVRAM -mdirect -mwpc -fno-builtin -mcode-section=.text -mdata-section=.text -mbss-section=ram -Wno-format -Werror=implicit-function-declaration
EXTRA_ASFLAGS += -D__int8__

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

# Import images that are common to all games.
ifeq ($(CONFIG_DMD),y)
IMAGE_MAP += $(P)/wpc.ild
endif

# The startup code needs to know the number of paged regions in the ROM
# to checksum.  The pages between BOTTOM_BANK and TOP_BANK are paged in
# and contribute to the total checksum.  TOP_BANK is fixed but
# BOTTOM_BANK varies according to the machine-specific ROM usage.
$(P)/start.o : EXTRA_ASFLAGS += -DTOP_BANK=$(TOP_BANK) -DBOTTOM_BANK=$(BOTTOM_BANK)
