
DBCON := $(D)/wpcdebug
TOOLS += $(DBCON)
OBJS := $(D)/wpcdebug.o $(D)/input.o
$(OBJS) : TOOL_CFLAGS=-DSTANDALONE
HOST_OBJS += $(OBJS)
$(DBCON) : $(OBJS)

# vim: set filetype=make:
