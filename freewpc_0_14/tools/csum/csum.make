
CSUM := $(D)/csum
TOOLS += $(CSUM)
OBJS := $(D)/csum.o
HOST_OBJS += $(OBJS)
$(CSUM) : $(D)/csum.o

# vim: set filetype=make:
