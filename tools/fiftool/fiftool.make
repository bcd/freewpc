
FIFTOOL := $(D)/fiftool
TOOLS += $(FIFTOOL)
OBJS := $(D)/fiftool.o tools/pgmlib/pgmlib.o
$(OBJS) : TOOL_CFLAGS=-Itools/pgmlib -Iinclude -DNO_MAIN
HOST_OBJS += $(OBJS) 
$(FIFTOOL) : $(OBJS)

# vim: set filetype=make:
