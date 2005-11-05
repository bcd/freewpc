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

# DEBUG_COMPILER=y

#######################################################################
###	Configuration
#######################################################################

# Set this to the name of the machine for which you are targetting.
MACHINE ?= tz

# Set this to the path where the final ROM image should be installed
# This is left blank on purpose: set this in your user.make file
TARGET_ROMPATH =

# Which version of the assembler tools to use
ASVER ?= 3.0.0


#######################################################################
###	Directories
#######################################################################

LIBC_DIR = ./libc
INCLUDE_DIR = ./include
MACHINE_DIR = ./$(MACHINE)
ASTOOLS_DIR = as-$(ASVER)/asxmak/linux/build


#######################################################################
###	Filenames
#######################################################################

# Where to write errors
ERR = err
TMPFILES += $(ERR)

# The linker command file (generated dynamically)
LINKCMD = freewpc.lnk
PAGED_LINKCMD = page55.lnk page56.lnk page57.lnk page58.lnk page59.lnk \
					 page60.lnk page61.lnk page62.lnk

# The XBM prototype header file
XBM_H = images/xbmproto.h

SYSTEM_BINFILES = freewpc.bin
PAGED_BINFILES = page55.bin page56.bin page57.bin page58.bin \
					  page59.bin page60.bin page61.bin page62.bin
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
GCC_ROOT = /usr/local/m6809/bin
CC = $(GCC_ROOT)/gcc
LD = $(GCC_ROOT)/ld
AS = $(GCC_ROOT)/as
REQUIRED += $(CC) $(LD) $(AS)

# Name of the S-record converter
SR = tools/srec2bin/srec2bin
PATH_REQUIRED += $(SR)

# Name of the blanker to use
BLANKER = dd
PATH_REQUIRED += $(BLANKER)

# The XBM prototype generator
XBMPROTO = tools/xbmproto

#######################################################################
###	Source and Binary Filenames
#######################################################################

GAME_ROM = freewpc.rom

FIXED_SECTION = sysrom

OS_OBJS = div10.o init.o sysinfo.o dmd.o \
	switches.o flip.o sound.o coin.o service.o game.o test.o \
	device.o lampset.o score.o deff.o leff.o triac.o paging.o db.o \
	trough.o font.o printf.o tilt.o vector.o reset.o player.o \
	task.o lamp.o sol.o

FONT_OBJS = fonts/mono5x5.o fonts/mono9x6.o

XBM_OBJS = images/freewpc.o images/brian.o

OS_INCLUDES = include/freewpc.h include/wpc.h

INCLUDES = $(OS_INCLUDES) $(GAME_INCLUDES)


XBM_SRCS = $(patsubst %.o,%.xbm,$(XBM_OBJS))

#######################################################################
###	Compiler / Assembler / Linker Flags
#######################################################################

# Default CFLAGS
CFLAGS = -I$(LIBC_DIR)/include -I$(INCLUDE_DIR) -I$(MACHINE_DIR)

# Default optimizations.  These are the only optimizations that
# are known to work OK; using -O2 is almost guaranteed to fail.
CFLAGS += -O1 -fstrength-reduce -frerun-loop-opt -fomit-frame-pointer -Wunknown-pragmas -foptimize-sibling-calls -fstrict-aliasing

# Default machine flags.  To keep code size small, turn on short
# branches by default.  Some files may need to override this option
# if they include a long function, which might need to branch longer
# distances.  Those files can use "#pragma long_branch" to revert to
# the safer inefficient form.
CFLAGS += -mshort_branch

# This didn't work before, but now it does!
# However, it is still disabled by default.
# This implies -fstrength-reduce and -frerun-cse-after-loop
ifdef UNROLL_LOOPS
CFLAGS += -funroll-loops
endif

# Throw some extra information in the assembler logs
# (disabled because it doesn't really help much)
# CFLAGS += -fverbose-asm

# Turn on compiler debug.  This will cause a bunch of compiler
# debug files to get written out during every phase of the build.
ifdef DEBUG_COMPILER
CFLAGS += -da
endif

# Please, turn on all warnings!  But don't check format strings,
# because we define those differently than ANSI C.
CFLAGS += -Wall -Wno-format


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

page55_SECTIONS =
page56_SECTIONS =
page57_SECTIONS =
page58_SECTIONS =
page59_SECTIONS =
page60_SECTIONS =
page61_SECTIONS =
page62_SECTIONS = font

PAGED_SECTIONS = $(page55_SECTIONS) $(page56_SECTIONS) $(page57_SECTIONS) \
					  $(page58_SECTIONS) $(page59_SECTIONS) $(page60_SECTIONS) \
					  $(page61_SECTIONS) $(page62_SECTIONS)

RAM_ADDR = 0x0
PAGED_ROM_ADDR = 0x4000
FIXED_ROM_ADDR = 0x8000
VECTOR_ROM_ADDR = 0xFFF0


KERNEL_OBJS = $(patsubst %,kernel/%,$(OS_OBJS))
MACHINE_OBJS = $(patsubst %,$(MACHINE)/%,$(GAME_OBJS))
SYSTEM_HEADER_OBJS =	freewpc.o


page55_OBJS =
page56_OBJS =
page57_OBJS =
page58_OBJS =
page59_OBJS =
page60_OBJS =
page61_OBJS =
page62_OBJS = $(FONT_OBJS)

PAGED_OBJS = $(page55_OBJS) $(page56_OBJS) $(page57_OBJS) \
				 $(page58_OBJS) $(page59_OBJS) $(page60_OBJS) \
				 $(page61_OBJS) $(page62_OBJS)

PAGE_HEADER_OBJS = page55.o page56.o page57.o page58.o page59.o \
						 page60.o page61.o page62.o

SYSTEM_OBJS = $(SYSTEM_HEADER_OBJS) $(KERNEL_OBJS) $(MACHINE_OBJS) $(XBM_OBJS)


AS_OBJS = $(SYSTEM_HEADER_OBJS)

C_OBJS = $(KERNEL_OBJS) $(MACHINE_OBJS) $(FONT_OBJS)



OBJS = $(C_OBJS) $(AS_OBJS) $(XBM_OBJS)

DEPS = $(DEFMACROS) $(INCLUDES) Makefile $(XBM_H) $(MACHINE)/Makefile

GENDEFINES = \
	include/gendefine_gid.h \
	include/gendefine_deff.h \
	include/gendefine_leff.h \
	include/gendefine_lampset.h \

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
	cd $(TARGET_ROMPATH) && \
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
$(GAME_ROM) : blank256.bin blank64.bin blank32.bin $(BINFILES)
	@echo Padding ... && cat blank256.bin blank64.bin blank32.bin $(PAGED_BINFILES) $(SYSTEM_BINFILES) > $@

#
# How to make a blank file.  This creates an empty file of any desired size
# in multiples of 1KB.
#
blank%.bin:
	@echo Creating $*KB blank file ... && $(BLANKER) if=/dev/zero of=$@ bs=1k count=$* > /dev/null 2>&1

$(SYSTEM_BINFILES) : %.bin : %.s19 $(SR)
	@echo Converting $< to binary ... && $(SR) $< $@ system

$(PAGED_BINFILES) : %.bin : %.s19 $(SR)
	@echo Converting $< to binary ... && $(SR) $< $@ paged

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
	@rm -f $@
	@echo "-mxswz" >> $@
	@echo "-b ram = 0x100" >> $@
	@for f in `echo $($(@:.lnk=_SECTIONS))`; do echo "-b $$f = 0x4000" >> $@; done
	@echo "-b sysrom = 0x8000" >> $@
	@echo $(@:.lnk=.o) >> $@
	@echo $($(@:.lnk=_OBJS)) >> $@
	@echo "-v" >> $@
	@for f in `echo $(SYSTEM_OBJS)`; do echo $$f >> $@; done
	@for f in `echo $(filter-out $($(@:.lnk=_OBJS)) $(@:.lnk=.o), $(PAGE_HEADER_OBJS) $(PAGED_OBJS))`; do echo $$f >> $@; done
	@echo "-k $(LIBC_DIR)/" >> $@
	@echo "-l c.a" >> $@
	@echo "-e" >> $@

	#@echo "-b vector = 0xFFF0" >> $@

#
# How to build a page header source file.
#
page%.s:
	@echo ".area sysrom" > page$*.s
	@echo ".db 0" >> page$*.s
	@echo ".area page$*" >> page$*.s
	@echo ".db 0" >> page$*.s
	@echo ".area ram" >> page$*.s
	@echo ".db 0" >> page$*.s

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
	@echo Assembling $< ... && $(AS) $< 2>&1 | tee -a err

#
# General rule for how to build a page header, which is a special
# version of an assembly file.
#
$(PAGE_HEADER_OBJS) : page%.o : page%.s $(REQUIRED) $(DEPS)
	@echo Assembling page header $< ... && $(AS) $< 2>&1 | tee -a err

#
# General rule for how to build any C module.
#
$(C_OBJS) : %.o : %.c $(REQUIRED) $(DEPS) $(GENDEFINES)
	@echo Compiling $< ... && $(CC) -o $(@:.o=.S) -S $(CFLAGS) $<
	@echo Assembling $(@:.o=.S) ... && $(AS) $(@:.o=.S)

#! @$(CC) -o $@ -c $(CFLAGS) $< 2>&1 | tee -a err

#
# General rule for how to build any XBM bitmap image.
#
$(XBM_OBJS) : %.o : %.xbm
	@echo Compiling $< ... && $(CC) -Dstatic= -o $(@:.o=.S) -x c -S $<
	@echo Assembling $(@:.o=.S) ... && $(AS) $(@:.o=.S)

ptrindex:
	tools/ptrindex/ptrindex.pl -m deff -C kernel -C tz


#######################################################################
###	Header File Targets
#######################################################################

#
# How to make the XBM prototypes
#

xbmprotos: $(XBM_H)

$(XBM_H) : $(XBM_SRCS) $(XBMPROTO)
	@echo Generating XBM prototypes... && $(XBMPROTO) -o $(XBM_H) -D images


#
# How to automake files of #defines
#
gendefines: $(GENDEFINES)

include/gendefine_gid.h :
	@echo Autogenerating task IDs... && tools/gendefine -p GID_ > include/gendefine_gid.h

include/gendefine_deff.h :
	@echo Autogenerating display effect IDs... && tools/gendefine -p DEFF_ > include/gendefine_deff.h

include/gendefine_leff.h :
	@echo Autogenerating lamp effect IDs... && tools/gendefine -p LEFF_ > include/gendefine_leff.h

include/gendefine_lampset.h :
	@echo Autogenerating lampset IDs... && tools/gendefine -p LAMPSET_ > include/gendefine_lampset.h

clean_gendefines:
	@echo Deleting autogenerated files... && rm -f $(GENDEFINES)

gendefines_again: clean_gendefines gendefines

#######################################################################
###	Tools
#######################################################################

#
# 'make gcc' will build the compiler.  It uses a homegrown script
# 'gccbuild' to control the entire link process.
#
# 'make gcc-install' will install it into the local bin directory.
#
# 'make gcc-anythingelse' will run gcc's 'anythingelse' target.
#
gcc-install:
	cp -p $(GCC) /usr/local/bin/gcc09

gcc:
	cd gcc-build && ./gccbuild make

gcc-%:
	cd gcc-build && ./gccbuild %

#
# 'make astools' will build the assembler, linker, and library
# manager.  As with gcc, -install will also install it and any
# other suffix will pass those options to the astools makefile.
#
astools: astools-build astools-install

astools-install:
	cp -p $(ASTOOLS_DIR)/aslink $(LD)

astools-build:
	cd asm-thomson && make && make install
	cd $(ASTOOLS_DIR) && make

astools-%:
	cd $(ASTOOLS_DIR) && make $*

#
# How to build the srec2bin utility, which is a simple S-record to
# binary converter suitable for what we need.
#
$(SR) : $(SR).c
	cd tools/srec2bin && make srec2bin

kernel/switches.o : include/$(MACHINE)/switch.h

#
# Install to the web server
#
WEBDIR := /home/bcd/oddchange/freewpc

web : webdocs webroms

webdocs : webdir
	cp -p doc/* $(WEBDIR)

webroms : webdir
	cp -p $(GAME_ROM) $(WEBDIR)/releases

webdir : $(WEBDIR)

$(WEBDIR):
	mkdir -p $(WEBDIR)
	mkdir -p $(WEBDIR)/releases


#
# 'make clean' does what you think.
#
clean: clean_derived clean_gendefines
	@for dir in `echo . kernel fonts images $(MACHINE)`;\
		do echo Removing files in \'$$dir\' ... && \
		cd $$dir && rm -f $(TMPFILES) && cd -; done

clean_derived:
	@for file in `echo $(XBM_H)` ;\
		do echo "Removing derived file $$file..." && \
		rm -f $$file; done

show_objs:
	@echo $(OBJS)

