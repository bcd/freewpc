
SOFTSCOPE := $(D)/softscope
TOOLS += $(SOFTSCOPE)
OBJS := $(D)/softscope.o
$(OBJS) : TOOL_CFLAGS=-DSTANDALONE
HOST_OBJS += $(OBJS) 
$(SOFTSCOPE) : $(OBJS)

# vim: set filetype=make:
