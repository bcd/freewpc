
# Root directory for the platform files
P=platform/wpc

CPU := m6809
$(eval $(call have,CONFIG_PERIODIC_FIRQ))
CONFIG_TASK := y
include cpu/$(CPU)/Makefile

# Subplatform support
CONFIG_WHITESTAR ?= y
CONFIG_FONT ?= $(CONFIG_DMD)
CONFIG_AC ?= n
CONFIG_ANIMATION ?= $(CONFIG_DMD)
$(eval $(call have,CONFIG_GI))
$(eval $(call nohave,CONFIG_AC))

# Additional CFLAGS required on the WPC platform
CFLAGS += -mdirect -DCONFIG_PLATFORM_WHITESTAR -fno-builtin -DCONFIG_PINMAME -mcode-section=.text -mdata-section=.text -mbss-section=ram
EXTRA_ASFLAGS += -DCONFIG_PLATFORM_WHITESTAR

# Don't check format strings, because we define those differently than ANSI C.
CFLAGS += -Wno-format

# Optional CFLAGS
CFLAGS += -mint8

### NOTE:  -mwpc is totally not acceptable, as Whitestar doesn't have
# the shifter hardware.  But gcc6809 is crashing on some bit shift
# statements, so for _now_, keep this here until the compiler can be
# fixed, so we can fix up other problems.
CFLAGS += -mwpc

# Optimization flags
CFLAGS += -O2 -fomit-frame-pointer -fstrength-reduce -frerun-loop-opt -Wunknown-pragmas -foptimize-sibling-calls -fstrict-aliasing -fregmove

KERNEL_SW_OBJS += $(P)/main.o
KERNEL_ASM_OBJS += $(P)/start.o
