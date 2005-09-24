#
# FreeWPC makefile
#
# (C) Copyright 2005 by Brian Dominy.
#
# This Makefile can be used to build an entire, FreeWPC game ROM
# from source code.
#
# To build the product, just type "make"
#
# To build the compiler, type "make gcc".  You will need root
# privileges to install it.
#

#######################################################################
###	Configuration
#######################################################################

# Set this to the name of the machine for which you are targetting.
MACHINE ?= tz

# Set this to the path where the final ROM image should be installed
TARGET_ROMPATH = /home/bcd/eptools/mameroms

# Which version of the assembler tools to use
ASVER ?= 3.0.0

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
LINKCMD = freewpc.lnk
PAGED_LINKCMD = page62.lnk

SYSTEM_BINFILES = freewpc.bin
PAGED_BINFILES = page62.bin
BINFILES = $(SYSTEM_BINFILES) $(PAGED_BINFILES)
TMPFILES += $(LINKCMD)

TMPFILES = *.sp		# Intermediate sasm assmebler files
TMPFILES += *.o		# Intermediate object files
TMPFILES += *.lnk		# Linker command files
TMPFILES += *.s19 	# Motorola S-record files
TMPFILES += *.map 	# Linker map files
TMPFILES += *.bin		# Raw binary images
TMPFILES += *.rom		# Complete ROM files
TMPFILES += *.lst 	# Assembler listings
TMPFILES += *.s1 *.s2 *.s3 *.s4 *.S 
TMPFILES += *.c.[0-9]*.* 
TMPFILES += *.out
TMPFILES += *.m41 	# Old M4 macro output files
TMPFILES += page*.s	# Page header files
TMPFILES += $(ERR)

#######################################################################
###	Programs
#######################################################################

# Path to the compiler and linker
CC = /usr/local/bin/gcc09
# LD = /usr/local/bin/ld09
LD = /home/bcd/bin/aslink
REQUIRED += $(CC) $(LD)

# Name of the rommer to use
ROMMER = srec_cat
PATH_REQUIRED += $(ROMMER)

# Name of the blanker to use
BLANKER = dd
PATH_REQUIRED += $(BLANKER)


#######################################################################
###	Source and Binary Filenames
#######################################################################

GAME_ROM = freewpc.rom

PAGED_SECTIONS = font

FIXED_SECTION = sysrom

OS_OBJS = div10.o init.o sysinfo.o task.o lamp.o sol.o dmd.o \
	switches.o sound.o coin.o service.o game.o test.o \
	device.o lampset.o score.o deff.o triac.o paging.o db.o \
	trough.o font.o printf.o tilt.o vector.o

FONT_OBJS = fonts/mono5x5.o

XBM_OBJS = images/freewpc.o images/brian.o

OS_INCLUDES = include/freewpc.h include/wpc.h

INCLUDES = $(OS_INCLUDES) $(GAME_INCLUDES)

#######################################################################
###	Compiler / Assembler / Linker Flags
#######################################################################

# Default CFLAGS
CFLAGS = -I$(LIBC_DIR)/include -I$(INCLUDE_DIR) -I$(MACHINE_DIR)

# Default optimizations.  These are the only optimizations that
# are known to work OK; using -O2 is almost guaranteed to fail.
CFLAGS += -O1 -fstrength-reduce -frerun-loop-opt -fomit-frame-pointer -Wunknown-pragmas -foptimize-sibling-calls

# This didn't work before, but now it does!
# However, it is still disabled by default.
# This implies -fstrength-reduce and -frerun-cse-after-loop
ifdef UNROLL_LOOPS
CFLAGS += -funroll-loops
endif

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

#######################################################################
###	Object File Distribution
#######################################################################

# Because WPC uses ROM paging, the linking job is more
# difficult to get right.  We require that the programmer
# explicitly state which pages things should belong in.
#
# The process happens at two levels.  First, each source
# file can use "#pragma section ()" to place functions into
# different "sections", which are just named groups of code
# and data.  A single file can have a mixture of multiple
# sections in theory, but in reality there are some restrictions.
#
# The following variables group different object files into
# "pages", which are like sections...
#


KERNEL_OBJS = $(patsubst %,kernel/%,$(OS_OBJS))
MACHINE_OBJS = $(patsubst %,$(MACHINE)/%,$(GAME_OBJS))
SYSTEM_HEADER_OBJS =	freewpc.o


PAGE55_OBJS =
PAGE56_OBJS =
PAGE57_OBJS =
PAGE58_OBJS =
PAGE59_OBJS =
PAGE60_OBJS =
PAGE61_OBJS =
PAGE62_OBJS = $(FONT_OBJS)

PAGED_OBJS = $(PAGE61_OBJS) $(PAGE62_OBJS)

PAGE_HEADER_OBJS = page61.o page62.o

SYSTEM_OBJS = $(SYSTEM_HEADER_OBJS) $(KERNEL_OBJS) $(MACHINE_OBJS) $(XBM_OBJS)


AS_OBJS = $(SYSTEM_HEADER_OBJS)

C_OBJS = $(KERNEL_OBJS) $(MACHINE_OBJS) $(FONT_OBJS)



OBJS = $(C_OBJS) $(AS_OBJS) $(XBM_OBJS)

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

#
# Use 'make build' to build the ROM without installing it.
#
build : $(GAME_ROM)

#
# How to make a ROM image, which is the concatenation of each of the
# paged binaries, the system binary, and padding to fill out the length
# to that expected for the particular machine.
#
$(GAME_ROM) : blank256.bin blank128.bin blank64.bin blank16.bin $(BINFILES)
	@echo Padding ... && cat blank256.bin blank128.bin blank64.bin blank16.bin $(PAGED_BINFILES) $(SYSTEM_BINFILES) > $@

#
# How to make a blank file.  This creates an empty file of any desired size
# in multiples of 1KB.
#
blank%.bin:
	@echo Creating $*KB blank file ... && $(BLANKER) if=/dev/zero of=$@ bs=1k count=$* > /dev/null 2>&1

$(SYSTEM_BINFILES) : %.bin : %.s19
	@echo Converting $< to binary ... && $(ROMMER) $< --motorola --output - --binary | dd of=$@ bs=1k skip=32

ROMMER_FLAGS = --fill 0xff 0x4000 0x8000
$(PAGED_BINFILES) : %.bin : %.s19
	@echo Converting $< to binary ... && $(ROMMER) $< --motorola $(ROMMER_FLAGS) --output - --binary | dd of=$@ bs=1k skip=16

#
# General rule for linking a group of object files.  The linker produces
# a Motorola S-record file by default (S19).
#
$(BINFILES:.bin=.s19) : %.s19 : $(LD) $(OBJS) $(AS_OBJS) $(PAGE_HEADER_OBJS) %.lnk
	@echo Linking $@... && $(LD) -f $@ >> $(ERR) 2>&1

#
# How to make the linker command file for a paged section.
#
$(PAGED_LINKCMD) : $(DEPS)
	@echo Creating linker command file...
	@rm -f $(PAGED_LINKCMD)
	@echo "-mxswz" >> $(PAGED_LINKCMD)
	@echo "-b ram = 0x100" >> $(PAGED_LINKCMD)
	@for f in `echo $(PAGED_SECTIONS)`; do echo "-b $$f = 0x4000" >> $(PAGED_LINKCMD); done
	@echo "-b sysrom = 0x8000" >> $(PAGED_LINKCMD)
	@echo "-b vector = 0xFFF0" >> $(PAGED_LINKCMD)
	@echo page62.o >> $(PAGED_LINKCMD)
	@echo $(FONT_OBJS) >> $(PAGED_LINKCMD)
	@echo "-v" >> $(PAGED_LINKCMD)
	@for f in `echo $(SYSTEM_OBJS)`; do echo $$f >> $(PAGED_LINKCMD); done
	@for f in `echo $(filter-out $(FONT_OBJS), $(PAGE_HEADER_OBJS) $(PAGED_OBJS))`; do echo $$f >> $(PAGED_LINKCMD); done
	@echo "-k $(LIBC_DIR)/" >> $(PAGED_LINKCMD)
	@echo "-l c.a" >> $(PAGED_LINKCMD)
	@echo "-e" >> $(PAGED_LINKCMD)


#
# How to build a page header source file.
#
page%.s:
	@echo ".area page$*" > page$*.s

#
# How to make the linker command file for the system section.
#
$(LINKCMD) : $(DEPS)
	@echo Creating linker command file...
	@rm -f $(LINKCMD)
	@echo "-mxswz" >> $(LINKCMD)
	@echo "-b ram = 0x100" >> $(LINKCMD)
	@for f in `echo $(PAGED_SECTIONS)`; do echo "-b $$f = 0x4000" >> $(LINKCMD); done
	@echo "-b sysrom = 0x8000" >> $(LINKCMD)
	@echo "-b vector = 0xFFF0" >> $(LINKCMD)
	@for f in `echo $(SYSTEM_OBJS)`; do echo $$f >> $(LINKCMD); done
	@echo "-v" >> $(LINKCMD)
	@for f in `echo $(PAGED_OBJS)`; do echo $$f >> $(LINKCMD); done
	@echo "-k $(LIBC_DIR)/" >> $(LINKCMD)
	@echo "-l c.a" >> $(LINKCMD)
	@echo "-e" >> $(LINKCMD)

#
# General rule for how to build any assembler file.  We use gcc to do
# it rather than invoking the assembler directly.  This lets us embed
# #include, #define, etc. within the assembly code, since the C preprocessor
# is invoked first.
#
$(AS_OBJS) : %.o : %.s $(REQUIRED) $(DEPS)
	@echo Assembling $< ... && $(CC) -o $@ -c -x assembler-with-cpp $< 2>&1 | tee -a err

#
# General rule for how to build a page header, which is a special
# version of an assembly file.
#
$(PAGE_HEADER_OBJS) : page%.o : page%.s $(REQUIRED) $(DEPS)
	@echo Assembling page header $< ... && $(CC) -o $@ -c -x assembler-with-cpp $< 2>&1 | tee -a err

#
# General rule for how to build any C module.
#
$(C_OBJS) : %.o : %.c $(REQUIRED) $(DEPS)
	@echo Compiling $< ... && $(CC) -o $(@:.o=.S) -S $(CFLAGS) $<
	@$(CC) -o $@ -c $(CFLAGS) $< 2>&1 | tee -a err

#
# General rule for how to build any XBM bitmap image.
#
$(XBM_OBJS) : %.o : %.xbm
	@echo Compiling $< ... && $(CC) -Dstatic= -o $@ -x c -c $< 2>&1 | tee -a err

show_objs:
	@echo $(OBJS)

ptrindex:
	tools/ptrindex/ptrindex.pl -m deff -C kernel -C tz

#
# 'make gcc' will build the compiler.  It uses a homegrown script
# 'gccbuild' to control the entire link process.
#
gcc:
	cd gcc-build && ./gccbuild

gcc-%:
	cd gcc-build && ./gccbuild %

astools:
	cd as-$(ASVER) && make

astools-%:
	cd as-$(ASVER) && make %


kernel/switches.o : include/$(MACHINE)/switch.h

#
# 'make clean' does what you think.
#
clean:
	@for dir in `echo . kernel fonts images $(MACHINE)`;\
		do echo Removing files in $$dir... && \
		cd $$dir && rm -f $(TMPFILES) && cd -; done

