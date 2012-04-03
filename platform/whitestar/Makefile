
# Root directory for the platform files
P=platform/whitestar

ifndef CPU
CPU := m6809
$(eval $(call have,CONFIG_PERIODIC_FIRQ))
CONFIG_TASK := y
include cpu/$(CPU)/Makefile
$(eval $(call have,CONFIG_MMIO))
endif

# Subplatform support
$(eval $(call have,CONFIG_PLATFORM_WHITESTAR))
$(eval $(call nohave,CONFIG_AC))
$(eval $(call have,CONFIG_GI))
$(eval $(call have,CONFIG_NO_DEFFS))

# ROM bank switching
CONFIG_ROM_BANK_SIZE := 16
CONFIG_FIXED_PAGE_COUNT := 2
CONFIG_MAX_ROM_PAGES := 8
CONFIG_CODE_PAGE_LIST := 56 57 58 59 60 61
CONFIG_SYSTEM_CODE_PAGE := 62

# Additional CFLAGS required on the WPC platform
ifeq ($(CPU),m6809)
CFLAGS += -mint8 -mdirect -fno-builtin -mcode-section=.text -mdata-section=.text -mbss-section=ram
endif

# Don't check format strings, because we define those differently than ANSI C.
CFLAGS += -Wno-format

# Optimization flags
CFLAGS += -O2 -fomit-frame-pointer -fstrength-reduce -frerun-loop-opt -Wunknown-pragmas -foptimize-sibling-calls -fstrict-aliasing -fregmove

KERNEL_SW_OBJS += $(P)/ws_hw.o
ifeq ($(CPU),m6809)
KERNEL_ASM_OBJS += platform/wpc/start.o
KERNEL_SW_OBJS += $(P)/main.o
endif

platform/wpc/start.o : EXTRA_ASFLAGS += -DTOP_BANK=$(TOP_BANK) -DBOTTOM_BANK=$(BOTTOM_BANK)
