#
# FreeWPC makefile
#

#######################################################################
###	Configuration
#######################################################################

# Set this to the name of the machine for which you are targetting.
MACHINE = tz
TARGET_MACHINE = tz92

# Set this to the path where the final ROM image should be installed
TARGET_ROMPATH = /home/bcd/eptools/mameroms

# Set this to the path where libc can be found
LIBC_PATH = ./libc

#######################################################################
###	Filenames
#######################################################################

# Where to write errors
ERR = err
TMPFILES += $(ERR)

# The linker command file (generated dynamically)
LINKCMD = system.lnk
BINFILES = system.bin
TMPFILES += $(LINKCMD)

# Preloaded macro files
DEFMACROS = m6809.m4 syscall.m4

#######################################################################
###	Programs
#######################################################################

# Name of the assembler to use
AS = ./sasm09
REQUIRED += $(AS)

CC = /usr/local/bin/gcc09
REQUIRED += $(CC)

# Name of the rommer to use
ROMMER = srec_cat
PATH_REQUIRED += $(ROMMER)

# Name of the blanker to use
BLANKER = dd
PATH_REQUIRED += $(BLANKER)

# Source files for the core OS
AS_OS_OBJS = system.o vector.o
SAS_OS_OBJS = switch.o task1.o dmd1.o segment1.o

OS_OBJS = div10.o init.o sysinfo.o task.o lamp.o sol.o dmd.o \
	switches.o sound.o coin.o service.o game.o test.o \
	segment.o device.o lampset.o score.o deff.o

OS_INCLUDES = include/freewpc.h


GAME_OBJS = config.o clock.o
AS_GAME_OBJS =

GAME_INCLUDES =

INCLUDES = $(OS_INCLUDES) $(GAME_INCLUDES)

ASMFLAGS = -I. -Iinclude -D__SASM__
ASMFLAGS += -N --save-temps

# Default CFLAGS
CFLAGS = -I. -Iinclude -I$(LIBC_PATH)/include

# Default optimizations.  These are the only optimizations that
# are known to work OK; using -O2 is almost guaranteed to fail.
CFLAGS += -O1 -fstrength-reduce -frerun-loop-opt -fomit-frame-pointer -Wunknown-pragmas -foptimize-sibling-calls

# Turn on compiler debug.  This will cause a bunch of compiler
# debug files to get written out during every phase of the build.
ifdef DEBUG_COMPILER
CFLAGS += -da
endif

# Please, turn on all warnings!
CFLAGS += -Wall

# I've been burned by not having a prototype for a function
# that takes a 'char' sized argument.  The compiler implicitly
# converts this to 'int', which is a different size, and bad
# things happen...
CFLAGS += -Werror-implicit-function-declaration

OBJS = $(OS_OBJS) $(patsubst %,$(MACHINE)/%,$(GAME_OBJS))
AS_OBJS = $(AS_OS_OBJS) $(AS_GAME_OBJS)
SAS_OBJS = $(SAS_OS_OBJS) $(SAS_GAME_OBJS)

DEPS = $(DEFMACROS) $(INCLUDES) Makefile

INSTALL_TARGET=install_$(TARGET_MACHINE)

#######################################################################
###	Begin Makefile Targets
#######################################################################

default_target : clean_err check_prereqs $(INSTALL_TARGET)

clean_err:
	rm -f $(ERR)

check_prereqs :

install_tz92 : freewpc.rom
	@echo Copying to mame directory ...; \
	cp -p freewpc.rom $(TARGET_ROMPATH)/tzone9_2.rom; \
	cd $(TARGET_ROMPATH); \
	rm -f tz_92.zip; \
	zip -9 tz_92.zip tzone9_2.rom tzu*.rom

freewpc.rom : blank256.bin blank128.bin blank64.bin blank32.bin system.bin
	@echo Padding ... && cat blank256.bin blank128.bin blank64.bin blank32.bin system.bin > $@

blank%.bin:
	@echo Creating blank file ... && dd if=/dev/zero of=$@ bs=1k count=$*

$(BINFILES) : %.bin : %.s19
	@echo Converting $< to binary ... && $(ROMMER) $< --motorola --output - --binary | dd of=$@ bs=1k skip=32

$(BINFILES:.bin=.s19) : %.s19 : $(LINKCMD) $(OBJS) $(AS_OBJS) $(SAS_OBJS)
	@echo Linking $@... && aslink -f $@ >> $(ERR) 2>&1

$(SAS_OBJS) : %.o : %.s $(AS) $(DEPS)
	$(AS) $(ASMFLAGS) $<

$(AS_OBJS) : %.o : %.s $(CC) $(DEPS)
	@echo Assembling $< ... && $(CC) -c -x assembler-with-cpp $<

$(OBJS) : %.o : %.c $(CC) $(DEPS)
	@echo Compiling $< ... && $(CC) -o $(@:.o=.S) -S $(CFLAGS) $<
	@$(CC) -o $@ -c $(CFLAGS) $< > /dev/null 2>&1

$(LINKCMD) : $(DEPS)
	@echo Creating linker command file...
	@rm -f $(LINKCMD)
	@echo "-mxswz" >> $(LINKCMD)
	@echo "-b fastram = 0x0" >> $(LINKCMD)
	@echo "-b ram = 0x100" >> $(LINKCMD)
	#@echo "-b _DATA = 0x800" >> $(LINKCMD)
	#@echo "-b rom = 0x4000" >> $(LINKCMD)
	@echo "-b sysrom = 0x8000" >> $(LINKCMD)
	@echo "-b vector = 0xFFF0" >> $(LINKCMD)
	@for f in `echo $(AS_OBJS) $(SAS_OBJS) $(OBJS)`; do echo $$f >> $(LINKCMD); done
	@echo "-k $(LIBC_PATH)/" >> $(LINKCMD)
	@echo "-l c.a" >> $(LINKCMD)
	@echo "-e" >> $(LINKCMD)

show_objs:
	echo $(OBJS)

clean:
	rm -f *.sp *.o *.rel $(LINKCMD) *.s19 *.map *.bin *.rom *.lst *.s1 *.s2 *.s3 *.s4 *.S *.c.[0-9]*.* *.lst *.out *.m41 $(ERR)
