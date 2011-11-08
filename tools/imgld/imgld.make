
IMGLD := $(D)/imgld
TOOLS += $(IMGLD)
OBJS := $(D)/imgld.o tools/imglib/imglib.o
$(OBJS) : TOOL_CFLAGS=-Itools/imglib -Iinclude -DNO_MAIN -DCONFIG_NATIVE -DWPC_DMD_LOW_PAGE=0 -DWPC_DMD_HIGH_PAGE=0
HOST_OBJS += $(OBJS)
$(IMGLD) : $(OBJS)

# vim: set filetype=make:
