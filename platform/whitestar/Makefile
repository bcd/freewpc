
# Root directory for the platform files
P=platform/wpc

# Additional CFLAGS required on the WPC platform
CFLAGS += -DHAVE_FASTRAM_ATTRIBUTE -mdirect -DCONFIG_PLATFORM_WHITESTAR -fno-builtin -DCONFIG_PINMAME -DCONFIG_BIG_ENDIAN -mcode-section=.text -mdata-section=.text -mbss-section=ram 

# Don't check format strings, because we define those differently than ANSI C.
CFLAGS += -Wno-format

# Optional CFLAGS

# Save assembler files if needed for debugging the 6809 compiler.
ifeq ($(SAVE_ASM),y)
CFLAGS += -save-temps
endif

CFLAGS += -mint8

# Turn on compiler debug.  This will cause a bunch of compiler
# debug files to get written out during every phase of the build.
# This is really only needed for debugging the 6809 compiler.
ifeq ($(DEBUG_COMPILER),y)
CFLAGS += -da -dA
endif

# Optimization flags
CFLAGS += -O2 -fomit-frame-pointer -fstrength-reduce -frerun-loop-opt -Wunknown-pragmas -foptimize-sibling-calls -fstrict-aliasing -fregmove

KERNEL_SW_OBJS +=  \
	$(P)/div10.o  \
	$(P)/main.o \
	$(P)/malloc.o  \
	$(P)/task.o  \
	$(P)/vector.o \

KERNEL_ASM_OBJS = $(P)/farcall.o $(P)/task_6809.o $(P)/start.o

