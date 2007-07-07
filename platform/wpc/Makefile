
# Root directory for the platform files
P=platform/wpc

# Additional CFLAGS required on the WPC platform
CFLAGS += -DHAVE_FASTRAM_ATTRIBUTE -mdirect -DCONFIG_PLATFORM_WPC -mwpc -fno-builtin -DCONFIG_PINMAME -DCONFIG_BIG_ENDIAN -mcode-section=.text -mdata-section=.text -mbss-section=ram 

ifdef CONFIG_16BIT
CFLAGS += -fshort-enums
else
CFLAGS += -mint8
endif

# Optimization flags
CFLAGS += -O2 -fomit-frame-pointer -fstrength-reduce -frerun-loop-opt -Wunknown-pragmas -foptimize-sibling-calls -fstrict-aliasing -fregmove

KERNEL_SW_OBJS += $(P)/div10.o $(P)/malloc.o $(P)/task.o $(P)/vector.o

KERNEL_ASM_OBJS = $(P)/farcall.o $(P)/task_6809.o $(P)/start.o

