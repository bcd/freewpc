
# Root directory for the CPU files.
CPU := m6809
C := cpu/$(CPU)

CFLAGS =

# Soft register support is enabled by -msoft-reg-count.
SOFTREG_OPTIONS := -msoft-reg-count=4 -DSOFT_REG_COUNT=4

# Save assembler files for debugging the 6809 compiler.
ifeq ($(SAVE_ASM),y)
CFLAGS += -save-temps
endif

# Turn on compiler debug.  This will cause a bunch of compiler
# debug files to get written out.
ifeq ($(DEBUG_COMPILER),y)
CFLAGS += -da -dA
endif

ifeq ($(CONFIG_BPT),y)
CFLAGS += -DCONFIG_BPT
endif

# Optimization flags
CFLAGS += -O2 -fomit-frame-pointer -fstrength-reduce -frerun-loop-opt -Wunknown-pragmas -foptimize-sibling-calls -fstrict-aliasing -fregmove -fgcse-after-reload -fgcse-sm -fgcse-las -ffreestanding
CFLAGS += -DCONFIG_BIG_ENDIAN -DHAVE_FASTRAM_ATTRIBUTE
# TODO : should -funroll-loops be given?
CFLAGS += $(if $(CONFIG_MALLOC), -DCONFIG_MALLOC)

# Which version of the 6809 compiler to use by default.
# This is the minimum available version.  More often than not,
# testing/releases are done with later builds than this.
GCC_VERSION ?= 4.3.4

KERNEL_OBJS     += $(C)/vector.o
KERNEL_OBJS     += $(C)/div10.o
KERNEL_ASM_OBJS += $(C)/farcall.o
KERNEL_OBJS     += $(if $(CONFIG_TASK), $(C)/task.o)
KERNEL_ASM_OBJS += $(if $(CONFIG_TASK), $(C)/task_6809.o)
KERNEL_BASIC_OBJS += $(if $(CONFIG_MALLOC), $(C)/malloc.o)
KERNEL_ASM_OBJS += $(C)/string.o
KERNEL_ASM_OBJS += $(C)/bcd_string.o
KERNEL_ASM_OBJS += $(C)/irqload.o
ifeq ($(FREEWPC_DEBUGGER),y)
KERNEL_ASM_OBJS += $(if $(CONFIG_BPT), $(C)/breakpoint.o)
endif
