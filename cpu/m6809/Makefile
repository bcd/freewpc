
# Root directory for the CPU files.
CPU := m6809
C := cpu/$(CPU)

# Path to the compiler
GCC_ROOT ?= /usr/local/bin
CC := $(GCC_ROOT)/m6809-unknown-none-gcc-$(GCC_VERSION)
AS := $(CC) -xassembler-with-cpp
LD := $(GCC_ROOT)/m6809-unknown-none-ld
REQUIRED += $(CC) $(LD)

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
CFLAGS += -O2 -fstrength-reduce -frerun-loop-opt -Wunknown-pragmas -foptimize-sibling-calls -fstrict-aliasing -fregmove -fgcse-after-reload -fgcse-sm -fgcse-las -ffreestanding
CFLAGS += -DCONFIG_BIG_ENDIAN
# TODO : should -funroll-loops be given?
CFLAGS += $(if $(CONFIG_MALLOC), -DCONFIG_MALLOC)
ifeq ($(CONFIG_PROFILE),y)
CFLAGS += -pg
else
CFLAGS += -fomit-frame-pointer
endif


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
KERNEL_ASM_OBJS += $(if $(CONFIG_TEST), $(C)/irqload.o)
KERNEL_ASM_OBJS += $(C)/div32.o
KERNEL_ASM_OBJS += $(if $(CONFIG_PROFILE), $(C)/mcount.o)
KERNEL_ASM_OBJS += $(if $(CONFIG_BPT), $(C)/breakpoint.o)
KERNEL_ASM_OBJS += $(C)/section.o

.PHONY : run
run:
	# Start pinmame up and let it run indefinitely.
	$(PINMAME) $(PINMAME_MACHINE) $(PINMAME_FLAGS) &

.PHONY : debug
debug:
	# Start pinmame up and let it run indefinitely.
	$(PINMAME) $(PINMAME_MACHINE) -d $(PINMAME_FLAGS) &

.PHONY : rund
rund: run $(DBCON)
	xterm -e "$(DBCON) -1" &

.PHONY : rund-file
rund-file: run $(DBCON)
	xterm -e "$(DBCON) -1 > $(DBCON_FILE) 2>&1" &

.PHONY : endrun
endrun :
	kill $(shell pidof $(PINMAME))

.PHONY : timedrun
timedrun:
	$(MAKE) rund-file DBCON_FILE="test.log"
	sleep 30
	$(MAKE) endrun

.PHONY : runcon
runcon: $(BLDDIR)/$(GAME_ROM)
	wpc-run $(BLDDIR)/$(GAME_ROM)

