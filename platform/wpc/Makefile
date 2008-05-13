
# Root directory for the platform files
P := platform/wpc

# Common 6809 configuration
CPU := m6809
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
INT_CFLAGS = -mint8
CFLAGS += $(INT_CFLAGS) -DHAVE_FASTRAM_ATTRIBUTE -DHAVE_NVRAM -mdirect -DCONFIG_PLATFORM_WPC -mwpc -fno-builtin -DCONFIG_PINMAME -DCONFIG_BIG_ENDIAN -mcode-section=.text -mdata-section=.text -mbss-section=ram -Wno-format

KERNEL_HW_OBJS += $(P)/main.o

KERNEL_ASM_OBJS += $(if $(CONFIG_DMD), $(P)/bitmap.o)
KERNEL_ASM_OBJS += $(if $(CONFIG_DMD), $(P)/dmd.o)
KERNEL_ASM_OBJS += $(P)/dot.o
KERNEL_ASM_OBJS += $(if $(CONFIG_DMD), $(P)/shadow.o)
KERNEL_ASM_OBJS += $(P)/start.o

COMMON_BASIC_OBJS += $(if $(CONFIG_PIC), $(P)/math.o)

CALLSET_FLAGS += -D platform/wpc

# The math code must be compiled in 16-bit mode, since it interfaces
# to the system libraries which expect this.
$(P)/math.o : INT_CFLAGS=-mint16
