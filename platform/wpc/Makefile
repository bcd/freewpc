
# Root directory for the platform files
P := platform/wpc

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
CFLAGS = $(INT_CFLAGS) -DHAVE_FASTRAM_ATTRIBUTE -DHAVE_NVRAM -mdirect -DCONFIG_PLATFORM_WPC -mwpc -fno-builtin -DCONFIG_PINMAME -DCONFIG_BIG_ENDIAN -mcode-section=.text -mdata-section=.text -mbss-section=ram -Wno-format
SOFTREG_OPTIONS := -msoft-reg-count=4 -DSOFT_REG_COUNT=4

# Optional CFLAGS based on configuration options

# Save assembler files if needed for debugging the 6809 compiler.
ifeq ($(SAVE_ASM),y)
CFLAGS += -save-temps
endif

# Turn on compiler debug.  This will cause a bunch of compiler
# debug files to get written out during every phase of the build.
# This is really only needed for debugging the 6809 compiler.
ifeq ($(DEBUG_COMPILER),y)
CFLAGS += -da -dA
endif

# Optimization flags
CFLAGS += -O2 -fomit-frame-pointer -fstrength-reduce -frerun-loop-opt -Wunknown-pragmas -foptimize-sibling-calls -fstrict-aliasing -fregmove -fgcse-after-reload -fgcse-sm -fgcse-las -ffreestanding
# TODO : should -funroll-loops be given?

KERNEL_HW_OBJS += $(P)/div10.o $(P)/main.o $(P)/task.o $(P)/vector.o

KERNEL_SW_OBJS += $(P)/malloc.o

KERNEL_ASM_OBJS += $(P)/dmd.o $(P)/farcall.o $(P)/task_6809.o $(P)/start.o $(P)/bitmap.o

COMMON_BASIC_OBJS += $(P)/math.o

CALLSET_FLAGS += -D platform/wpc

$(P)/math.o : INT_CFLAGS=-mint16
