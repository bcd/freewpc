
SCHED := $(D)/sched
TOOLS += $(SCHED)
OBJS := $(D)/sched.o
HOST_OBJS += $(OBJS)
$(SCHED) : $(D)/sched.o

# vim: set filetype=make:
