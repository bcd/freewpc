
SR := $(D)/srec2bin
TOOLS += $(SR)
HOST_OBJS += $(D)/srec2bin.o

$(D)/srec2bin : $(D)/srec2bin.o

# vim: set filetype=make:
