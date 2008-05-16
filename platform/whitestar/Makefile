
# Root directory for the platform files
P=platform/wpc

CPU := m6809
CONFIG_TASK := y
include cpu/$(CPU)/Makefile

# Additional CFLAGS required on the WPC platform
CFLAGS += -mdirect -DCONFIG_PLATFORM_WHITESTAR -fno-builtin -DCONFIG_PINMAME -mcode-section=.text -mdata-section=.text -mbss-section=ram

# Don't check format strings, because we define those differently than ANSI C.
CFLAGS += -Wno-format

# Optional CFLAGS
CFLAGS += -mint8

# Optimization flags
CFLAGS += -O2 -fomit-frame-pointer -fstrength-reduce -frerun-loop-opt -Wunknown-pragmas -foptimize-sibling-calls -fstrict-aliasing -fregmove

KERNEL_SW_OBJS += $(P)/main.o
KERNEL_ASM_OBJS += $(P)/start.o
