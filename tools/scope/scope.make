
SCOPE := $(D)/scope
TOOLS += $(SCOPE)
OBJS := $(D)/scope.o
$(OBJS) : TOOL_CFLAGS=-DSTANDALONE
HOST_OBJS += $(OBJS) 
$(SCOPE) : $(OBJS)

# vim: set filetype=make:
