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
# This is left blank on purpose: set this in your user.make file.
# There is no default value.
TARGET_ROMPATH =

# Which version of the assembler tools to use
ASVER ?= 1.5.2
#ASVER ?= 3.0.0

# Which version of the compiler to use
GCC_VERSION ?= 3.3.6

# Set to 'y' if you want to use the direct page (not working yet)
USE_DIRECT_PAGE=n

# Uncomment this if you want extra debug notes from the compiler.
# Normally, you do not want to turn this on.
# DEBUG_COMPILER=y

#######################################################################
###	Include User Settings
#######################################################################
-include user.make

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
PAGED_LINKCMD = page56.lnk page57.lnk page58.lnk \
	page59.lnk page60.lnk page61.lnk

# The XBM prototype header file
XBM_H = images/xbmproto.h

SYSTEM_BINFILES = freewpc.bin
PAGED_BINFILES = page56.bin page57.bin \
					  page58.bin page59.bin page60.bin page61.bin
BINFILES = $(SYSTEM_BINFILES) $(PAGED_BINFILES)
TMPFILES += $(LINKCMD)

TMPFILES = *.sp		# Intermediate sasm assmebler files
TMPFILES += *.o		# Intermediate object files
TMPFILES += *.rel		# Intermediate object files (as 3.0.0)
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
TMPFILES += *.callset # Callset files
TMPFILES += $(ERR)

#######################################################################
###	Programs
#######################################################################

# Path to the compiler and linker
GCC_ROOT = /usr/local/m6809/bin
ifdef GCC_VERSION
CC = $(GCC_ROOT)/gcc-$(GCC_VERSION)
else
CC = $(GCC_ROOT)/gcc
endif
CC_MODE ?= -S
# CC_MODE = -E
LD = $(GCC_ROOT)/ld
ifdef NEWAS
AS = $(GCC_ROOT)/as-$(ASVER)
else
AS = $(GCC_ROOT)/as
endif
REQUIRED += $(CC) $(LD) $(AS)

# Name of the S-record converter
SR = tools/srec2bin/srec2bin
PATH_REQUIRED += $(SR)

# Name of the blanker to use
BLANKER = dd
PATH_REQUIRED += $(BLANKER)

# The XBM prototype generator
XBMPROTO = tools/xbmproto

# The Unix calculator
BC = bc

#######################################################################
###	Source and Binary Filenames
#######################################################################

GAME_ROM = freewpc.rom

FIXED_SECTION = sysrom

OS_OBJS = div10.o init.o adj.o sysinfo.o dmd.o \
	switches.o flip.o sound.o coin.o service.o game.o test.o \
	device.o lampset.o score.o deff.o leff.o triac.o paging.o db.o \
	trough.o font.o printf.o tilt.o vector.o reset.o player.o \
	task.o lamp.o sol.o flasher.o ac.o

FONT_OBJS = fonts/mono5x5.o fonts/mono9x6.o

XBM_OBJS = images/freewpc.o images/brian.o

OS_INCLUDES = include/freewpc.h include/wpc.h

INCLUDES = $(OS_INCLUDES) $(GAME_INCLUDES)


XBM_SRCS = $(patsubst %.o,%.xbm,$(XBM_OBJS))

#######################################################################
###	Compiler / Assembler / Linker Flags
#######################################################################
# Default include directories
CFLAGS = -I$(LIBC_DIR)/include -I$(INCLUDE_DIR) -I$(MACHINE_DIR) -Icallset

# Additional defines
ifdef GCC_VERSION
CFLAGS += -DGCC_VERSION=$(GCC_VERSION)
else
CFLAGS += -DGCC_VERSION=3.1.1
endif

ifdef ASVER
CFLAGS += -DAS_VERSION=$(ASVER)
else
CFLAGS += -DAS_VERSION=1.5.2
endif

# Default optimizations.  These are the only optimizations that
# are known to work OK; using -O2 is almost guaranteed to fail.
CFLAGS += -O1 -fstrength-reduce -frerun-loop-opt -fomit-frame-pointer -Wunknown-pragmas -foptimize-sibling-calls -fstrict-aliasing -fregmove

# Default machine flags.  To keep code size small, turn on short
# branches by default.  Some files may need to override this option
# if they include a long function, which might need to branch longer
# distances.  Those files can use "#pragma long_branch" to revert to
# the safer inefficient form.
CFLAGS += -mshort-branch

# This didn't work before, but now it does!
# However, it is still disabled by default.
# This implies -fstrength-reduce and -frerun-cse-after-loop
ifdef UNROLL_LOOPS
CFLAGS += -funroll-loops
endif

CFLAGS += -fno-builtin

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

ifeq ($(FREEWPC_DEBUGGER),y)
CFLAGS += -DDEBUGGER
endif
ifdef USER_MAJOR
CFLAGS += -DFREEWPC_MAJOR_VERSION=$(USER_MAJOR)
endif
ifdef USER_MINOR
CFLAGS += -DFREEWPC_MINOR_VERSION=$(USER_MAJOR)
endif
ifdef USER_TAG
CFLAGS += -DUSER_TAG=$(USER_TAG)
endif

#
# Newer versions of the assembler require these flags be passed.
#
ifeq ($(ASVER),1.5.2)
ASFLAGS =
else
ASFLAGS = -log
endif

#######################################################################
###	Include Machine Extensions
#######################################################################
include $(MACHINE)/Makefile

#######################################################################
###	Object File Distribution
#######################################################################

# Because WPC uses ROM paging, the linking job is more
# difficult to get right.  We require that the programmer
# explicitly state which pages things should belong in.
# TBD

#PAGED_SECTIONS = page54 page55 page56 page57 page58 page59 page60 page61
PAGED_SECTIONS = page56 page57 page58 page59 page60 page61

NUM_PAGED_SECTIONS := 6 

NUM_BLANK_PAGES := \
	$(shell echo $(ROM_PAGE_COUNT) - 2 - $(NUM_PAGED_SECTIONS) | $(BC))

BLANK_SIZE := $(shell echo $(NUM_BLANK_PAGES) \* 16 | $(BC))

#
# The WPC physical memory map is divided into four sections.
# The build procedure can control which section is used for a
# particular function.
#

ifeq ($(USE_DIRECT_PAGE),y)
DIRECT_AREA = 0x0
RAM_AREA = 0x100
DIRECT_LNK_CMD = "-b direct = $(DIRECT_AREA)"
else
RAM_AREA = 0x0
DIRECT_LNK_CMD = "-x"
endif
NVRAM_AREA = 0x1800
PAGED_AREA = 0x4000
FIXED_AREA = 0x8000
VECTOR_AREA = 0xFFF0


KERNEL_OBJS = $(patsubst %,kernel/%,$(OS_OBJS))
MACHINE_OBJS = $(patsubst %,$(MACHINE)/%,$(GAME_OBJS))
SYSTEM_HEADER_OBJS =	freewpc.o

#
# Define a mapping between object files and page numbers in
# which they should be placed.  This information must be
# provided in both directions.
#
page56_OBJS = page56.o
page57_OBJS = page57.o
page58_OBJS = page58.o
page59_OBJS = page59.o
page60_OBJS = page60.o $(XBM_OBJS)
page61_OBJS = page61.o $(FONT_OBJS)

$(XBM_OBJS) : PAGE=60
$(FONT_OBJS) : PAGE=61

PAGE_DEFINES := -DSYS_PAGE=59 -DXBM_PAGE=60 -DFONT_PAGE=61
CFLAGS += $(PAGE_DEFINES)

PAGED_OBJS = $(page56_OBJS) $(page57_OBJS) \
				 $(page58_OBJS) $(page59_OBJS) $(page60_OBJS) $(page61_OBJS)


PAGE_HEADER_OBJS = page56.o page57.o page58.o page59.o \
						 page60.o page61.o

SYSTEM_OBJS = $(SYSTEM_HEADER_OBJS) $(KERNEL_OBJS) $(MACHINE_OBJS)

$(SYSTEM_OBJS) : PAGE=59

AS_OBJS = $(SYSTEM_HEADER_OBJS)

C_OBJS = $(KERNEL_OBJS) $(MACHINE_OBJS) $(FONT_OBJS)


OBJS = $(C_OBJS) $(AS_OBJS) $(XBM_OBJS)

MACH_LINKS = mach include/mach

DEPS = $(DEFMACROS) $(INCLUDES) Makefile $(XBM_H) $(MACHINE)/Makefile $(MACH_LINKS)


GENDEFINES = \
	include/gendefine_gid.h \
	include/gendefine_deff.h \
	include/gendefine_leff.h \
	include/gendefine_lampset.h \

#######################################################################
###	Set Default Target
#######################################################################
default_target : clean_err check_prereqs install


#######################################################################
###	Begin Makefile Targets
#######################################################################

clean_err:
	rm -f $(ERR)

check_prereqs :

# TODO : change zip to do a replace of the existing ROM.  Also make
# a backup of the existing zip so we can run the real game again :-)
install : $(TARGET_ROMPATH)/$(PINMAME_GAME_ROM)
	@echo Installing to MAME directory '$(TARGET_ROMPATH)' ...; \
	cd $(TARGET_ROMPATH); \
	if [ ! -f $(PINMAME_MACHINE).zip.original ]; then \
		echo Saving original MAME roms...; \
		mv $(PINMAME_MACHINE).zip $(PINMAME_MACHINE).zip.original; \
	fi; \
	rm -f $(PINMAME_MACHINE).zip; \
	zip -9 $(PINMAME_MACHINE).zip $(PINMAME_GAME_ROM) $(PINMAME_OTHER_ROMS)

uninstall :
	@cd $(TARGET_ROMPATH) && \
	if [ -f $(PINMAME_MACHINE).zip.original ]; then \
		if [ -f $(PINMAME_MACHINE).zip ]; then \
			echo Restoring original $(MACHINE) ROM in $(TARGET_ROMPATH)... ; \
			rm -f $(PINMAME_MACHINE).zip && \
			mv $(PINMAME_MACHINE).zip.original $(PINMAME_MACHINE).zip; \
		fi; \
	fi


#
# PinMAME will want the ROM file to be named differently...
#
$(TARGET_ROMPATH)/$(PINMAME_GAME_ROM) : $(GAME_ROM)
	cp -p $(GAME_ROM) $(TARGET_ROMPATH)/$(PINMAME_GAME_ROM)

#
# Use 'make build' to build the ROM without installing it.
#
build : $(GAME_ROM)

#
# How to make a ROM image, which is the concatenation of each of the
# paged binaries, the system binary, and padding to fill out the length
# to that expected for the particular machine.
#
$(GAME_ROM) : blank$(BLANK_SIZE).bin $(BINFILES)
	@echo Padding ... && \
		cat blank$(BLANK_SIZE).bin $(PAGED_BINFILES) $(SYSTEM_BINFILES) > $@

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
	@echo Creating linker command file $@ ...
	@rm -f $@
	@echo "-mxswz" >> $@
	@echo $(DIRECT_LNK_CMD) >> $@
	@echo "-b ram = $(RAM_AREA)" >> $@
	@for f in `echo $(PAGED_SECTIONS)`; \
		do echo "-b $$f = $(PAGED_AREA)" >> $@; done
	@echo "-b sysrom = $(FIXED_AREA)" >> $@
	@echo $($(@:.lnk=_OBJS)) >> $@
	@echo "-v" >> $@
	@for f in `echo $(SYSTEM_OBJS)`; do echo $$f >> $@; done
	@for f in `echo $(filter-out $($(@:.lnk=_OBJS)) $(@:.lnk=.o), $(PAGE_HEADER_OBJS) $(PAGED_OBJS))`; do echo $$f >> $@; done
	@echo "-k $(LIBC_DIR)/" >> $@
	@echo "-l c.a" >> $@
	@echo "-e" >> $@

#
# How to build the system page header source file.
#
freewpc.s:
	@echo ".area sysrom" > $@
	@echo ".db 0" >> $@


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
	@echo Creating linker command file $@ ...
	@rm -f $(LINKCMD)
	@echo "-mxswz" >> $(LINKCMD)
	@echo $(DIRECT_LNK_CMD) >> $(LINKCMD)
	@echo "-b ram = $(RAM_AREA)" >> $(LINKCMD)
	@for f in `echo $(PAGED_SECTIONS)`; \
		do echo "-b $$f = $(PAGED_AREA)" >> $(LINKCMD); done
	@echo "-b sysrom = $(FIXED_AREA)" >> $(LINKCMD)
	@echo "-b vector = $(VECTOR_AREA)" >> $(LINKCMD)
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
	@echo Assembling $< ... && $(AS) $(ASFLAGS) $< 2>&1 | tee -a err
ifneq ($(ASVER), 1.5.2)
	@mv $*.rel $*.o
endif

#
# General rule for how to build a page header, which is a special
# version of an assembly file.
#
$(PAGE_HEADER_OBJS) : page%.o : page%.s $(REQUIRED) $(DEPS)
	@echo Assembling page header $< ... && $(AS) $(ASFLAGS) $< 2>&1 | tee -a err
ifneq ($(ASVER), 1.5.2)
	@mv $(@:.o=.rel) $@
endif

#
# General rule for how to build any C++ module.
#
#tz/cpptest.o : %.o : %.cpp $(REQUIRED) $(DEPS) $(GENDEFINES)
#	@echo Compiling $< ... && $(CC) -o $(@:.o=.S) -S $(CFLAGS) $<
#	@echo Assembling $(@:.o=.S) ... && $(AS) $(@:.o=.S)
#

#
# General rule for how to build any C or XBM module.
# The basic rule is the same, but with a few differences that are
# handled through some extra variables:
#
$(C_OBJS) : PAGEFLAGS="-DDECLARE_PAGED=__attribute__((section(\"page$(PAGE)\")))" 
$(XBM_OBJS) : PAGEFLAGS="-Dstatic=__attribute__((section(\"page$(PAGE)\")))"

$(C_OBJS) : GCC_LANG=
$(XBM_OBJS) : GCC_LANG=-x c

$(C_OBJS) : %.o : %.c $(REQUIRED) $(DEPS) $(GENDEFINES)
$(XBM_OBJS) : %.o : %.xbm

$(C_OBJS) $(XBM_OBJS) :
	@echo Compiling $< \(in page $(PAGE)\) ... && $(CC) -o $(@:.o=.S) $(CFLAGS) $(CC_MODE) $(PAGEFLAGS) $(GCC_LANG) $<
	@echo Assembling $(@:.o=.S) ... && $(AS) $(ASFLAGS) $(@:.o=.S)
ifneq ($(ASVER), 1.5.2)
	@mv $(@:.o=.rel) $@
endif

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
gendefines: $(GENDEFINES) $(MACH_LINKS)

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

#
# How to automake callsets
#
$(MACHINE)/config.c : callset

callset :
	@echo Generating callsets ... && mkdir -p callset && tools/gencallset

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
ifdef GCC_VERSION
ifeq ($(GCC_VERSION),3.1.1)
GCC_BUILD_DIR = gcc-build
else
GCC_BUILD_DIR = gcc-$(GCC_VERSION)-build
endif
else
GCC_BUILD_DIR = gcc-build
endif

gcc:
	cd $(GCC_BUILD_DIR) && ./gccbuild make

gcc-%:
	cd $(GCC_BUILD_DIR) && ./gccbuild $*


#
# 'make astools' will build the assembler, linker, and library
# manager.  As with gcc, -install will also install it and any
# other suffix will pass those options to the astools makefile.
#
astools: astools-build astools-install

astools-install:
	cp -p $(ASTOOLS_DIR)/aslink $(LD)

astools-build:
	cd asm-thomson && $(MAKE) && $(MAKE) install
	cd $(ASTOOLS_DIR) && $(MAKE)

astools-%:
	cd $(ASTOOLS_DIR) && $(MAKE) $*

#
# How to build the srec2bin utility, which is a simple S-record to
# binary converter suitable for what we need.
#
$(SR) : $(SR).c
	cd tools/srec2bin && $(MAKE) srec2bin

kernel/switches.o : include/$(MACHINE)/switch.h


#
# Symbolic links to the machine code.  Once set, code can reference
# 'mach' and 'include/mach' without knowing the specific machine type.
#
mach:
	@echo Setting symbolic link for machine source code &&\
		ln -s $(MACHINE) mach

include/mach:
	@echo Setting symbolic link for machine include files &&\
		cd include && ln -s $(MACHINE) mach

Makefile : user.make


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
# For debugging the makefile settings
#
info:
	@echo "Machine : $(MACHINE)"
	@echo "GCC_VERSION = $(GCC_VERSION)"
	@echo "CC = $(CC)"
	-@$(CC) -v
	@echo "AS = $(AS)"
	-@$(AS)
	@echo "CFLAGS = $(CFLAGS)"
	@echo "ASFLAGS = $(ASFLAGS)"
	@echo "BLANK_SIZE = $(BLANK_SIZE)"

#
# 'make clean' does what you think.
#
clean: clean_derived clean_gendefines
	@for dir in `echo . callset kernel fonts images $(MACHINE)`;\
		do echo Removing files in \'$$dir\' ... && \
		cd $$dir && rm -f $(TMPFILES) && cd -; done

clean_derived:
	@for file in `echo $(XBM_H) mach include/mach` ;\
		do echo "Removing derived file $$file..." && \
		rm -f $$file; done && \
		rm -rf callset

show_objs:
	@echo $(OBJS)

