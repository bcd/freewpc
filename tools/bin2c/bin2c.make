BIN2C := $(D)/bin2c
TOOLS += $(BIN2C)
OBJS := $(D)/bin2c.o
# $(OBJS) : TOOL_CFLAGS=-Itools/pgmlib -Iinclude -DNO_MAIN
HOST_OBJS += $(OBJS) 
$(BIN2C) : $(OBJS)
# vim: set filetype=make:
