
SCHED := $(D)/sched
TOOLS += $(SCHED)
OBJS := $(D)/sched.o
$(OBJS) : TOOL_CFLAGS=-save-temps
HOST_OBJS += $(OBJS)
$(SCHED) : $(D)/sched.o

# vim: set filetype=make:
