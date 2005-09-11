#
# FreeWPC makefile
#

#######################################################################
###	Configuration
#######################################################################

# Set this to the name of the machine for which you are targetting.
MACHINE = tz

# Set this to the path where the final ROM image should be installed
TARGET_ROMPATH = /home/bcd/eptools/mameroms

#######################################################################
###	Directories
#######################################################################

LIBC_DIR = ./libc
INCLUDE_DIR = ./include
MACHINE_DIR = ./$(MACHINE)

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
LD = /usr/local/bin/ld09
REQUIRED += $(CC) $(LD)

# Name of the rommer to use
ROMMER = srec_cat
PATH_REQUIRED += $(ROMMER)

# Name of the blanker to use
BLANKER = dd
PATH_REQUIRED += $(BLANKER)




GAME_ROM = freewpc.rom

# Source files for the core OS
AS_OS_OBJS = system.o vector.o
SAS_OS_OBJS = switch.o task1.o dmd1.o

OS_OBJS = div10.o init.o sysinfo.o task.o lamp.o sol.o dmd.o \
	switches.o sound.o coin.o service.o game.o test.o \
	device.o lampset.o score.o deff.o triac.o paging.o db.o \
	trough.o font.o printf.o

FONT_OBJS = 5x5.o

OS_INCLUDES = include/freewpc.h wpc.h


INCLUDES = $(OS_INCLUDES) $(GAME_INCLUDES)

ASMFLAGS = -I. -Iinclude -D__SASM__
ASMFLAGS += -N --save-temps

# Default CFLAGS
# TODO - remove -I. and put all includes into subdirectories
CFLAGS = -I$(LIBC_DIR)/include -I. -I$(INCLUDE_DIR) -I$(MACHINE_DIR)

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

# I'd like to use this sometimes, but some things don't compile with it...
# CFLAGS += -fno-defer-pop

OBJS = $(patsubst %,kernel/%,$(OS_OBJS)) \
	$(patsubst %,$(MACHINE)/%,$(GAME_OBJS)) \
	$(patsubst %,fonts/%,$(FONT_OBJS))
AS_OBJS = $(AS_OS_OBJS) $(AS_GAME_OBJS)
SAS_OBJS = $(SAS_OS_OBJS) $(SAS_GAME_OBJS)

DEPS = $(DEFMACROS) $(INCLUDES) Makefile

#######################################################################
###	Include User Settings
#######################################################################
-include user.make

#######################################################################
###	Set Default Target
#######################################################################
default_target : clean_err check_prereqs mame_install


#######################################################################
###	Include Machine Extensions
#######################################################################
include $(MACHINE)/Makefile


#######################################################################
###	Begin Makefile Targets
#######################################################################

clean_err:
	rm -f $(ERR)

check_prereqs :

# TODO : change zip to do a replace of the existing ROM.  Also make
# a backup of the existing zip so we can run the real game again :-)
mame_install : $(PINMAME_GAME_ROM)
	@echo Copying to mame directory ...; \
	cp -p $(PINMAME_GAME_ROM) $(TARGET_ROMPATH)/$(PINMAME_ROM); \
	cd $(TARGET_ROMPATH); \
	rm -f $(PINMAME_MACHINE).zip; \
	zip -9 $(PINMAME_MACHINE).zip $(PINMAME_GAME_ROM) $(PINMAME_OTHER_ROMS)

$(PINMAME_GAME_ROM) : $(GAME_ROM)
	cp -p $(GAME_ROM) $(PINMAME_GAME_ROM)

build : $(GAME_ROM)

$(GAME_ROM) : blank256.bin blank128.bin blank64.bin blank32.bin system.bin
	@echo Padding ... && cat blank256.bin blank128.bin blank64.bin blank32.bin system.bin > $@

blank%.bin:
	@echo Creating $*KB blank file ... && $(BLANKER) if=/dev/zero of=$@ bs=1k count=$* > /dev/null 2>&1

$(BINFILES) : %.bin : %.s19
	@echo Converting $< to binary ... && $(ROMMER) $< --motorola --output - --binary | dd of=$@ bs=1k skip=32

$(BINFILES:.bin=.s19) : %.s19 : $(LD) $(OBJS) $(AS_OBJS) $(SAS_OBJS) $(LINKCMD)
	@echo Linking $@... && aslink -f $@ >> $(ERR) 2>&1

new_link:
	@echo Linking $@... && $(LD) -mxs -b _DATA=0x800 -b sysrom=0x8000 -b vector=0xFFF0  -b fastram=0x0 -o $@ $(OBJS) $(AS_OBJS) $(SAS_OBJS) -L$(LIBC_DIR) -lc >> $(ERR) 2>&1

$(SAS_OBJS) : %.o : %.s $(AS) $(DEPS)
	$(AS) $(ASMFLAGS) $<

$(AS_OBJS) : %.o : %.s $(CC) $(DEPS)
	@echo Assembling $< ... && $(CC) -c -x assembler-with-cpp $<

$(OBJS) : %.o : %.c $(CC) $(DEPS)
	@echo Compiling $< ... && $(CC) -o $(@:.o=.S) -S $(CFLAGS) $<
	@$(CC) -o $@ -c $(CFLAGS) $< > err 2>&1

$(LINKCMD) : $(DEPS)
	@echo Creating linker command file...
	@rm -f $(LINKCMD)
	@echo "-mxswz" >> $(LINKCMD)
	@echo "-b fastram = 0x0" >> $(LINKCMD)
	@echo "-b ram = 0x100" >> $(LINKCMD)
	@#echo "-b _DATA = 0x800" >> $(LINKCMD)
	@#echo "-b rom = 0x4000" >> $(LINKCMD)
	@echo "-b sysrom = 0x8000" >> $(LINKCMD)
	@echo "-b vector = 0xFFF0" >> $(LINKCMD)
	@for f in `echo $(AS_OBJS) $(SAS_OBJS) $(OBJS)`; do echo $$f >> $(LINKCMD); done
	@echo "-k $(LIBC_DIR)/" >> $(LINKCMD)
	@echo "-l c.a" >> $(LINKCMD)
	@echo "-e" >> $(LINKCMD)

show_objs:
	echo $(OBJS)

clean:
	@echo Cleaning top-level directory ... && rm -f *.sp *.o *.rel $(LINKCMD) *.s19 *.map *.bin *.rom *.lst *.s1 *.s2 *.s3 *.s4 *.S *.c.[0-9]*.* *.lst *.out *.m41 $(ERR)
	@echo Cleaning 'kernel' ... && cd kernel && rm -f *.sp *.o *.rel $(LINKCMD) *.s19 *.map *.bin *.rom *.lst *.s1 *.s2 *.s3 *.s4 *.S *.c.[0-9]*.* *.lst *.out *.m41 $(ERR)
	@echo Cleaning $(MACHINE) ... && cd $(MACHINE) && rm -f *.sp *.o *.rel $(LINKCMD) *.s19 *.map *.bin *.rom *.lst *.s1 *.s2 *.s3 *.s4 *.S *.c.[0-9]*.* *.lst *.out *.m41 $(ERR)

