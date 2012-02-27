
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
CFLAGS += -mdirect -fno-builtin -mcode-section=.text -mdata-section=.text -mbss-section=ram

# Don't check format strings, because we define those differently than ANSI C.
CFLAGS += -Wno-format

# Optional CFLAGS
CFLAGS += -mint8

### TODO:  -mwpc is totally not acceptable, as Whitestar doesn't have
# the shifter hardware.  But gcc6809 is crashing on some bit shift
# statements, so for _now_, keep this here until the compiler can be
# fixed, so we can fix up other problems.
CFLAGS += -mwpc

# Optimization flags
CFLAGS += -O2 -fomit-frame-pointer -fstrength-reduce -frerun-loop-opt -Wunknown-pragmas -foptimize-sibling-calls -fstrict-aliasing -fregmove

KERNEL_SW_OBJS += $(P)/main.o
KERNEL_SW_OBJS += $(P)/ws_hw.o
KERNEL_ASM_OBJS += platform/wpc/start.o

platform/wpc/start.o : EXTRA_ASFLAGS += -DTOP_BANK=$(TOP_BANK) -DBOTTOM_BANK=$(BOTTOM_BANK)
