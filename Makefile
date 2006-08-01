#
# FreeWPC makefile
#
# (C) Copyright 2005-2006 by Brian Dominy.
#
# This Makefile can be used to build an entire, FreeWPC game ROM
# from source code.
#
# To build the product, just type "make".
#
# To customize the build, create a file named "user.make".
# See user.make.example for an example of how this should look.
# The settings in user.make override any compiler defaults in the
# Configuration section below.
#
# By default, make will also install your game ROM into your pinmame
# ROMs directory.  The original MAME zip file will be saved into
# a file with the .original extension appended.  You can do a
# "make uninstall" to delete the FreeWPC zip and rename the original
# zip, so that things are back to the way that they started.
#

#######################################################################
###	Configuration
#######################################################################

# Set this to the name of the machine for which you are targetting.
# MACHINE ?= $(shell if [ -h mach ]; then stat -c "%N" mach | awk '{print $3}' | tr -d "\`\'"; fi)
MACHINE ?= tz

# Set this to the name of the platform.  Normally this should be set
# by the machine's Makefile.  Default to WPC for now.
PLATFORM ?= wpc
#PLATFORM = whitestar
#PLATFORM = linux

# Set the location for temporary build output files
BLD ?= build

# Set this to the path where the final ROM image should be installed
# This is left blank on purpose: set this in your user.make file.
# There is no default value.
TARGET_ROMPATH =

# Which version of the assembler tools to use
# Version 1.5.2 is no longer supported; it generates .rel files instead of .o
ASVER ?= 4.1.0
ifeq ($(ASVER),)
ASVER := 4.1.0
endif
ifeq ($(ASVER),1.5.2)
error astools version 1.5.2 no longer supported
endif

# Which version of the compiler to use
GCC_VERSION ?= 3.4.6

# Set to 'y' if you want to use the direct page
USE_DIRECT_PAGE := y

# Set to 'y' if you want to enable multiplayer games
USE_LOCALS ?= y

# Set to 'y' if you want to save the assembly sources
SAVE_ASM ?= n

# Set to 'y' if you want to link with libc.
USE_LIBC ?= n

# Build date (now)
BUILD_DATE = \"$(shell date +%m/%d/%y)\"

# Uncomment this if you want extra debug notes from the compiler.
# Normally, you do not want to turn this on.
DEBUG_COMPILER ?= n

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

#######################################################################
###	Filenames
#######################################################################

# Where to write errors
ERR = err
TMPFILES += $(ERR)

# The linker command file (generated dynamically)
LINKCMD = $(BLD)/freewpc.lnk
PAGED_LINKCMD = $(BLD)/page56.lnk $(BLD)/page57.lnk $(BLD)/page58.lnk \
	$(BLD)/page59.lnk $(BLD)/page60.lnk $(BLD)/page61.lnk

# The XBM prototype header file
XBM_H = images/xbmproto.h

SYSTEM_BINFILES = $(BLD)/freewpc.bin
PAGED_BINFILES = $(BLD)/page56.bin $(BLD)/page57.bin $(BLD)/page58.bin \
	$(BLD)/page59.bin $(BLD)/page60.bin $(BLD)/page61.bin

BINFILES = $(SYSTEM_BINFILES) $(PAGED_BINFILES)
TMPFILES += $(LINKCMD)

TMPFILES += *.o
TMPFILES += *.rom
TMPFILES += *.lst
TMPFILES += *.S
TMPFILES += *.i
TMPFILES += *.c.[0-9]*.* 
TMPFILES += *.fon.[0-9]*.* 
TMPFILES += *.xbm.[0-9]*.* 
TMPFILES += *.out
TMPFILES += $(ERR)
TMPFILES += $(BLD)/*

#######################################################################
###	Programs
#######################################################################

# Path to the compiler and linker
GCC_ROOT = /usr/local/m6809/bin
ifdef GCC_VERSION
CC := $(GCC_ROOT)/gcc-$(GCC_VERSION)
else
CC := $(GCC_ROOT)/gcc
endif

# We use the latest versions of astools, version 4.1.0
LD6809 = $(GCC_ROOT)/aslink
LD = $(GCC_ROOT)/ld
AS6809 = $(GCC_ROOT)/as6809
AS = $(GCC_ROOT)/as

REQUIRED += $(CC) $(LD6809) $(AS6809) $(LD) $(AS)

# Name of the S-record converter
SR = tools/srec2bin/srec2bin
REQUIRED += $(PWD)/$(SR)

# Name of the blanker to use
BLANKER = dd
PATH_REQUIRED += $(BLANKER)

# The XBM prototype generator
XBMPROTO = tools/xbmproto

# The gendefine script
GENDEFINE = tools/gendefine

# The Unix calculator
BC = bc
PATH_REQUIRED += $(BC)

#######################################################################
###	Source and Binary Filenames
#######################################################################

FIXED_SECTION = sysrom

KERNEL_OBJS = \
	build/callset.o \
	kernel/ac.o \
	kernel/adj.o \
	kernel/audit.o \
	kernel/coin.o \
	kernel/csum.o \
	kernel/db.o \
	kernel/deff.o \
	kernel/device.o \
	kernel/div10.o \
	kernel/dmd.o \
	kernel/flasher.o \
	kernel/flip.o \
	kernel/font.o \
	kernel/game.o \
	kernel/init.o \
	kernel/lamp.o \
	kernel/lampset.o \
	kernel/leff.o \
	kernel/misc.o \
	kernel/msg.o \
	kernel/player.o \
	kernel/printf.o \
	kernel/random.o \
	kernel/reset.o \
	kernel/score.o \
	kernel/service.o \
	kernel/sol.o \
	kernel/sound.o \
	kernel/switches.o \
	kernel/sysinfo.o \
	kernel/task.o \
	kernel/timer.o \
	kernel/triac.o \
	kernel/trough.o \
	kernel/tilt.o \
	kernel/vector.o

COMMON_OBJS = \
	common/buyin.o \
	common/eb.o \
	common/highscore.o \
	common/knocker.o \
	common/match.o \
	common/replay.o \
	common/rtc.o \
	common/search.o \
	common/status.o \


KERNEL_ASM_OBJS = \
	kernel/farcall.o \

ifdef TASK_LL_SUPPORT
KERNEL_ASM_OBJS += kernel/task_6809.o
endif

TEST_OBJS = test/window.o

TRANS_OBJS = kernel/dmdtrans.o

FONT_OBJS = fonts/mono5.o fonts/mono9.o

FON_OBJS = \
	fonts/fixed10.o \
	fonts/fixed6.o \
	fonts/lucida9.o \
	fonts/cu17.o \
	fonts/term6.o \
	fonts/times10.o \
	fonts/times8.o \
	fonts/helv8.o \
	fonts/utopia.o \
	fonts/schu.o \
	fonts/miscfixed.o \

XBM_OBJS = images/freewpc.o images/brianhead_1.o images/brianhead_2.o \
	images/freewpc_logo_1.o images/freewpc_logo_2.o

OS_INCLUDES = include/freewpc.h include/wpc.h

INCLUDES = $(OS_INCLUDES) $(GAME_INCLUDES)

XBM_SRCS = $(patsubst %.o,%.xbm,$(XBM_OBJS))

FON_SRCS = $(patsubst %.o,%.fon,$(FON_OBJS))
export FON_SRCS

#######################################################################
###	Compiler / Assembler / Linker Flags
#######################################################################

CFLAGS = $(EXTRA_CFLAGS)

ifeq ($(SAVE_ASM),y)
CFLAGS += -save-temps
endif

ifeq ($(USE_LOCALS),y)
CFLAGS += -DCONFIG_MULTIPLAYER
endif

ifdef TASK_LL_SUPPORT
CFLAGS += -DTASK_LL_SUPPORT
endif

# System include directories.  FreeWPC doesn't use libc currently.
ifeq ($(USE_LIBC),y)
CFLAGS += -I$(LIBC_DIR)/include
endif

# Program include directories
CFLAGS += -I$(INCLUDE_DIR) -I$(MACHINE_DIR)

# Additional defines
ifdef GCC_VERSION
CFLAGS += -DGCC_VERSION=$(GCC_VERSION)
else
CFLAGS += -DGCC_VERSION=3.3.6
endif

ifdef ASVER
CFLAGS += -DAS_VERSION=$(ASVER)
else
CFLAGS += -DAS_VERSION=1.5.2
endif

# Options to control the output section names
CFLAGS += -mcode-section=sysrom -mdata-section=sysrom -mbss-section=ram

# Use 8-bit integers by default for now.
CFLAGS += -mint8

# Default optimizations.  -O2 works OK for me, but hasn't always; you might
# want to fall back to -O1 if you have problems.
ifndef OPT
OPT = -O2
endif
CFLAGS += $(OPT) -fstrength-reduce -frerun-loop-opt -Wunknown-pragmas -foptimize-sibling-calls -fstrict-aliasing -fregmove

# Default machine flags.  We enable WPC extensions here.
CFLAGS += -mwpc

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
ifeq ($(DEBUG_COMPILER),y)
CFLAGS += -da -dA
endif

# Please, turn on all warnings!  But don't check format strings,
# because we define those differently than ANSI C.
CFLAGS += -Wall -Wno-format

# I'd like to use this sometimes, but some things don't compile with it...
# CFLAGS += -fno-defer-pop

#
# Define lots of other things based on make parameters
#
CFLAGS += -DBUILD_DATE=$(BUILD_DATE)
ifeq ($(FREEWPC_DEBUGGER),y)
CFLAGS += -DDEBUGGER
ifeq ($(FREEWPC_IRQPROFILE),y)
CFLAGS += -DIRQPROFILE
endif
endif
ifndef SYSTEM_MAJOR
SYSTEM_MAJOR = 0
endif
CFLAGS += -DFREEWPC_MAJOR_VERSION=$(SYSTEM_MAJOR)
ifndef SYSTEM_MINOR
SYSTEM_MINOR = 0
endif
CFLAGS += -DFREEWPC_MINOR_VERSION=$(SYSTEM_MINOR)
ifndef MACHINE_MAJOR
MACHINE_MAJOR = $(SYSTEM_MAJOR)
endif
CFLAGS += -DMACHINE_MAJOR_VERSION=$(MACHINE_MAJOR)
ifndef MACHINE_MINOR
MACHINE_MINOR = $(SYSTEM_MINOR)
endif
CFLAGS += -DMACHINE_MINOR_VERSION=$(MACHINE_MINOR)
ifdef USER_TAG
CFLAGS += -DUSER_TAG=$(USER_TAG)
endif
ifeq ($(USE_DIRECT_PAGE),y)
CFLAGS += -DHAVE_FASTRAM_ATTRIBUTE -mdirect
else
CFLAGS += -mnodirect
endif
ifeq ($(USE_LIBC),y)
CFLAGS += -DHAVE_LIBC
endif
ifeq ($(FREE_ONLY),y)
CFLAGS += -DFREE_ONLY
endif

#######################################################################
###	Include Machine Extensions
#######################################################################
include $(MACHINE)/Makefile

#######################################################################
###	Include Platform Extensions
#######################################################################
-include Makefile.$(PLATFORM)

# Fix up names based on machine definitions
ifdef GAME_ROM_PREFIX
GAME_ROM = $(GAME_ROM_PREFIX)$(MACHINE_MAJOR)_$(MACHINE_MINOR).rom
else
GAME_ROM = freewpc.rom
endif

#######################################################################
###	Object File Distribution
#######################################################################

# Because WPC uses ROM paging, the linking job is more
# difficult to get right.  We require that the programmer
# explicitly state which pages things should belong in.

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
DIRECT_AREA = 0x4
RAM_AREA = 0x100
DIRECT_LNK_CMD = "-b direct = $(DIRECT_AREA)"
else
RAM_AREA = 0x4
DIRECT_LNK_CMD = "-x"
endif

ifeq ($(USE_LOCALS),y)
LOCAL_AREA = 0x1200
LOCAL_AREA_SIZE = 0xA0
LOCAL_SAVE_AREA = 0x12A0
LOCAL_LNK_CMD = "-b local = $(LOCAL_AREA)"
else
LOCAL_LNK_CMD = "-x"
endif

MALLOC_AREA = 0x1400
MALLOC_AREA_SIZE = 0x200

STACK_AREA = 0x1600
STACK_AREA_SIZE = 0x200

# The first 16-bytes of the nonvolatile area are reserved.
# PinMAME has a hack that overwrites this area.
NVRAM_AREA = 0x1810

PAGED_AREA = 0x4000
FIXED_AREA = 0x8000
VECTOR_AREA = 0xFFF0

MACHINE_OBJS = $(patsubst %,$(MACHINE)/%,$(GAME_OBJS))
MACHINE_TEST_OBJS = $(patsubst %,$(MACHINE)/%,$(GAME_TEST_OBJS))
MACHINE_PAGED_OBJS = $(patsubst %,$(MACHINE)/%,$(GAME_PAGED_OBJS))
SYSTEM_HEADER_OBJS =	$(BLD)/freewpc.o

#
# Define a mapping between object files and page numbers in
# which they should be placed.  This information must be
# provided in both directions.  Also, the mapping must be
# available to the C code in the form of xxx_PAGE macros
# for when the code wants to switch the page to a particular
# class of function.
#
page56_OBJS = $(BLD)/page56.o $(COMMON_OBJS)
page57_OBJS = $(BLD)/page57.o $(TRANS_OBJS)
page58_OBJS = $(BLD)/page58.o $(TEST_OBJS) $(MACHINE_TEST_OBJS)
page59_OBJS = $(BLD)/page59.o $(MACHINE_PAGED_OBJS)
page60_OBJS = $(BLD)/page60.o $(XBM_OBJS)
page61_OBJS = $(BLD)/page61.o $(FONT_OBJS) $(FON_OBJS)
SYSTEM_OBJS = $(SYSTEM_HEADER_OBJS) $(KERNEL_ASM_OBJS) $(KERNEL_OBJS) $(MACHINE_OBJS)

$(COMMON_OBJS) : PAGE=56
$(TRANS_OBJS) : PAGE=57
$(TEST_OBJS) $(MACHINE_TEST_OBJS): PAGE=58
$(MACHINE_PAGED_OBJS) : PAGE=59
$(XBM_OBJS) : PAGE=60
$(FONT_OBJS) $(FON_OBJS) : PAGE=61
$(SYSTEM_OBJS) : PAGE=62

PAGE_DEFINES := -DCOMMON_PAGE=56 -DTRANS_PAGE=57 -DTEST_PAGE=58 -DMACHINE_PAGE=59 -DXBM_PAGE=60 -DFONT_PAGE=61 -DSYS_PAGE=62
CFLAGS += $(PAGE_DEFINES)


PAGED_OBJS = $(page56_OBJS) $(page57_OBJS) \
				 $(page58_OBJS) $(page59_OBJS) $(page60_OBJS) $(page61_OBJS)


PAGE_HEADER_OBJS = $(BLD)/page56.o $(BLD)/page57.o $(BLD)/page58.o $(BLD)/page59.o \
						 $(BLD)/page60.o $(BLD)/page61.o

AS_OBJS = $(SYSTEM_HEADER_OBJS) $(KERNEL_ASM_OBJS)

C_OBJS = $(KERNEL_OBJS) $(COMMON_OBJS) $(TRANS_OBJS) $(TEST_OBJS) \
	$(MACHINE_OBJS) $(MACHINE_PAGED_OBJS) $(MACHINE_TEST_OBJS) \
	$(FONT_OBJS)


OBJS = $(C_OBJS) $(AS_OBJS) $(XBM_OBJS) $(FON_OBJS)

MACH_LINKS = .mach .include_mach


MAKE_DEPS = Makefile $(MACHINE)/Makefile user.make
DEPS = $(MAKE_DEPS) $(INCLUDES) $(XBM_H) $(MACH_LINKS)

GENDEFINES = \
	include/gendefine_gid.h \
	include/gendefine_deff.h \
	include/gendefine_leff.h \
	include/gendefine_lampset.h \
	include/gendefine_devno.h

#######################################################################
###	Set Default Target
#######################################################################
ifdef TARGET_ROMPATH
default_target : clean_err check_prereqs install
else
default_target : clean_err check_prereqs build
endif

#######################################################################
###	Begin Makefile Targets
#######################################################################

.PHONY : clean_err
clean_err:
	@rm -f $(ERR)

.PHONY : check_prereqs
check_prereqs :

install : $(TARGET_ROMPATH)/$(PINMAME_MACHINE).zip

$(TARGET_ROMPATH)/$(PINMAME_MACHINE).zip : $(TARGET_ROMPATH)/$(PINMAME_GAME_ROM)
	@echo Installing to MAME directory '$(TARGET_ROMPATH)' ...; \
	cd $(TARGET_ROMPATH); \
	if [ ! -f $(PINMAME_MACHINE).zip.original ]; then \
		echo "Saving original MAME roms..."; \
		mv $(PINMAME_MACHINE).zip $(PINMAME_MACHINE).zip.original; \
	fi; \
	rm -f $(PINMAME_MACHINE).zip; \
	zip -9 $(PINMAME_MACHINE).zip $(PINMAME_GAME_ROM) $(PINMAME_OTHER_ROMS)

uninstall :
	@cd $(TARGET_ROMPATH) && \
	if [ -f $(PINMAME_MACHINE).zip.original ]; then \
		if [ -f $(PINMAME_MACHINE).zip ]; then \
			echo "Restoring original $(MACHINE) ROM in $(TARGET_ROMPATH)..."; \
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
build : $(GAME_ROM) post_build

post_build :
	@echo "Cleaning .i files..." && rm -f *.i

#
# How to make a ROM image, which is the concatenation of each of the
# paged binaries, the system binary, and padding to fill out the length
# to that expected for the particular machine.
#
$(GAME_ROM) : $(BLD)/blank$(BLANK_SIZE).bin $(BINFILES)
	@echo Padding ... && \
		cat $(BLD)/blank$(BLANK_SIZE).bin $(PAGED_BINFILES) $(SYSTEM_BINFILES) > $@

#
# How to make a blank file.  This creates an empty file of any desired size
# in multiples of 1KB.
#
$(BLD)/blank%.bin: $(BLD)/blankpage.bin
	@echo "Creating $*KB blank file ..." && $(BLANKER) if=/dev/zero of=$@ bs=1k count=$* > /dev/null 2>&1

$(BLD)/blankpage.bin: $(SR)
	@echo "Creating blank 16KB page ..." && $(SR) -o $@ -l 0x4000 -f 0 -B

$(SYSTEM_BINFILES) : %.bin : %.s19 $(SR)
	@echo Converting $< to binary ... && $(SR) -o $@ -s 0x8000 -l 0x8000 -f 0 $<

$(PAGED_BINFILES) : %.bin : %.s19 $(SR)
	@echo Converting $< to binary ... && $(SR) -o $@ -s 0x4000 -l 0x4000 -f 0xFF $<

#
# General rule for linking a group of object files.  The linker produces
# a Motorola S-record file by default (S19).
#
$(BINFILES:.bin=.s19) : %.s19 : %.lnk $(LD) $(OBJS) $(AS_OBJS) $(PAGE_HEADER_OBJS)
	@echo Linking $@... && $(LD6809) -f $< >> $(ERR) 2>&1

#
# How to make the linker command file for a paged section.
#
# This is complicated.  Every paged area of the ROM needs visibility to
# all of the object files, no matter what pages they reside in, so that
# "far" objects can be referenced.  To do this, we have to link all of
# the objects once for every paged section.  The linker lets us specify
# for each object file whether it should be output into the image (-o)
# or is only used for resolving external references (-v).  So on every
# link step, a different set of -o and -v flags are used, depending on
# which page we are trying to build.
#
# Two helper functions are defined.  OBJ_PAGE_LINKOPT returns the
# right linker option followed by the name of the object file, given
# an object filename and the name of the linker command file.
# OBJ_PAGE_LIST does the same thing, but for all object files rather
# than just one of them.  The output of OBJ_PAGE_LIST has all of the
# options/filenames on a single line; the linker requires that they all
# be on separate lines.  We use a for loop in the bash code to iterate
# over the OBJ_PAGE_LIST output to split it into multiple lines.
#

# OBJ_PAGE_LINKOPT : Expands to either -o if in the page or -v if not
# $1 = object file name
# $2 = paged linkcmd file
# Example use: $(call OBJ_PAGE_LINKOPT,obj.o,page58.lnk)
# Example output: -o obj.o or -v obj.o
OBJ_PAGE_LINKOPT = $(subst -v $(1) $(1),-o $(1),-v $(1) $(findstring $(1),$($(2:.lnk=_OBJS))))

# OBJ_PAGE_LIST : Call OBJ_PAGE_LINKOPT for every relevant object file
# $1 = paged linkcmd file
# Note: the object filenames of the form pageXX.o are skipped as the
# bash code below already outputs these explicitly.
OBJ_PAGE_LIST = $(foreach obj,$(filter-out $(1:.lnk=.o),$(SYSTEM_OBJS) $(PAGED_OBJS)),$(call OBJ_PAGE_LINKOPT,$(obj),$(patsubst $(BLD)/%,%,$1)))

ifeq ($(ASVER),4.1.0)
DUP_PAGE_OBJ = $1
else
DUP_PAGE_OBJ = -x
endif

$(PAGED_LINKCMD) : $(MAKE_DEPS)
	@echo Creating linker command file $@ ... ;\
	rm -f $@ ;\
	echo "-xswz" >> $@ ;\
	echo $(DIRECT_LNK_CMD) >> $@ ;\
	echo "-b ram = $(RAM_AREA)" >> $@ ;\
	echo $(LOCAL_LNK_CMD) >> $@ ;\
	echo "-b nvram = $(NVRAM_AREA)" >> $@ ;\
	for f in `echo $(PAGED_SECTIONS)`; \
		do echo "-b $$f = $(PAGED_AREA)" >> $@ ;\
	done ;\
	echo "-b sysrom = $(FIXED_AREA)" >> $@ ;\
	echo "-o" >> $@ ;\
	echo "$(@:.lnk=.o)" >> $@ ;\
	echo "$(call DUP_PAGE_OBJ,$(@:.lnk=.o))" >> $@ ;\
	for f in `echo $(call OBJ_PAGE_LIST,$@)` ;\
	   do echo $$f >> $@ ;\
	done ;\
	echo "-k $(LIBC_DIR)/" >> $@ ;\
	echo "-l c.a" >> $@ ;\
	echo "-e" >> $@

#
# How to build the system page header source file.
#
$(BLD)/freewpc.s:
	@echo ".area sysrom" > $@


#
# How to build a page header source file.
#
$(BLD)/page%.s:
	@echo ".area page$*" >> $@
	@echo ".db 0" >> $@

#
# How to make the linker command file for the system section.
#
$(LINKCMD) : $(MAKE_DEPS)
	@echo Creating linker command file $@ ... ;\
	rm -f $(LINKCMD) ;\
	echo "-mxswz" >> $(LINKCMD) ;\
	echo $(DIRECT_LNK_CMD) >> $(LINKCMD) ;\
	echo "-b ram = $(RAM_AREA)" >> $(LINKCMD) ;\
	echo $(LOCAL_LNK_CMD) >> $(LINKCMD) ;\
	echo "-b nvram = $(NVRAM_AREA)" >> $(LINKCMD) ;\
	for f in `echo $(PAGED_SECTIONS)`; \
		do echo "-b $$f = $(PAGED_AREA)" >> $(LINKCMD); done ;\
	echo "-b sysrom = $(FIXED_AREA)" >> $(LINKCMD) ;\
	echo "-b vector = $(VECTOR_AREA)" >> $(LINKCMD) ;\
	for f in `echo $(BLD)/freewpc.o $(SYSTEM_OBJS)`; do echo $$f >> $(LINKCMD); done ;\
	echo "-v" >> $(LINKCMD) ;\
	for f in `echo $(PAGED_OBJS)`; do echo $$f >> $(LINKCMD); done ;\
	echo "-k $(LIBC_DIR)/" >> $(LINKCMD) ;\
	echo "-l c.a" >> $(LINKCMD) ;\
	echo "-e" >> $(LINKCMD)


#
# General rule for how to build any assembler file.  We use gcc to do
# it rather than invoking the assembler directly.  This lets us embed
# #include, #define, etc. within the assembly code, since the C preprocessor
# is invoked first.
#
$(AS_OBJS) : %.o : %.s $(REQUIRED) $(DEPS)
	@echo Assembling $< ... && $(AS) -o $@ $< > $(ERR) 2>&1

#
# General rule for how to build a page header, which is a special
# version of an assembly file.
#
$(PAGE_HEADER_OBJS) : $(BLD)/page%.o : $(BLD)/page%.s $(REQUIRED) $(DEPS)
	@echo Assembling page header $< ... && $(AS) -o $@ $< > $(ERR) 2>&1

#
# General rule for how to build any C or XBM module.
# The basic rule is the same, but with a few differences that are
# handled through some extra variables:
#
$(C_OBJS) : PAGEFLAGS="-DDECLARE_PAGED=__attribute__((section(\"page$(PAGE)\")))"
$(XBM_OBJS) $(FON_OBJS): PAGEFLAGS="-Dstatic=__attribute__((section(\"page$(PAGE)\")))"

$(C_OBJS) : GCC_LANG=
$(XBM_OBJS) $(FON_OBJS): GCC_LANG=-x c

$(C_OBJS) : %.o : %.c 

$(C_OBJS) : $(DEPS) $(GENDEFINES) $(REQUIRED)

$(XBM_OBJS) : %.o : %.xbm

$(FON_OBJS) : %.o : %.fon

$(C_OBJS) $(XBM_OBJS) $(FON_OBJS):
	@echo "Compiling $< (in page $(PAGE)) ..." && $(CC) -o $@ $(CFLAGS) -c $(PAGEFLAGS) -DPAGE=$(PAGE) -mfar-code-page=$(PAGE) $(GCC_LANG) $< > $(ERR) 2>&1

#
# For testing the compiler on sample code
#
ctest:
	echo "Test compiling $< ..." && $(CC) -o ctest.o $(CFLAGS) -c ctest.c

cpptest:
	@echo Test compiling $< ... && $(CC) -o cpptest.S $(CFLAGS) -c cpptest.cpp

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
	@echo Autogenerating task IDs... && \
		$(GENDEFINE) -p GID_ > $@

include/gendefine_deff.h :
	@echo Autogenerating display effect IDs... && \
		$(GENDEFINE) -p DEFF_ -c MAX_DEFFS > $@

include/gendefine_leff.h :
	@echo Autogenerating lamp effect IDs... && \
		$(GENDEFINE) -p LEFF_ -c MAX_LEFFS > $@

include/gendefine_lampset.h :
	@echo Autogenerating lampset IDs... && \
		$(GENDEFINE) -p LAMPSET_ -c MAX_LAMPSET > $@

include/gendefine_devno.h :
	@echo Autogenerating device IDs... && \
		$(GENDEFINE) -p DEVNO_ -f 0 -c NUM_DEVICES > $@

.PHONY : clean_gendefines
clean_gendefines:
	@echo Deleting autogenerated files... && rm -f $(GENDEFINES)

.PHONY : gendefines_again
gendefines_again: clean_gendefines gendefines

#
# How to automake callsets
#
callset: $(BLD)/callset.o

$(BLD)/callset.c : $(MACH_LINKS) tools/gencallset
	@echo "Generating callsets ... " && tools/gencallset

.PHONY : callset_again
callset_again:
	rm -rf $(BLD)/callset.c && $(MAKE) callset

.PHONY : fonts clean-fonts
fonts clean-fonts:
	@echo "Making $@... " && $(MAKE) -f Makefile.fonts $@

#######################################################################
###	Tools
#######################################################################

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
.mach:
	@echo Setting symbolic link for machine source code &&\
		touch .mach && ln -s $(MACHINE) mach

.include_mach:
	@echo Setting symbolic link for machine include files &&\
		touch .include_mach && cd include && ln -s $(MACHINE) mach

#
# Remake machine prototypes file
#
protos : include/$(MACHINE)/protos.h

include/$(MACHINE)/protos.h :
	cproto -o $@ -I include -I include/sys $(MACHINE)/*.c

#
# Install to the web server
# Set the location of the web documents in WEBDIR in your user.make.
#
web : webdocs

webdocs : webdir
	cp -p doc/* $(WEBDIR)
	cd $(WEBDIR) && chmod -R og+w *

ifdef WEBDIR
webdir : $(WEBDIR)

$(WEBDIR):
	mkdir -p $(WEBDIR)
	mkdir -p $(WEBDIR)/releases
endif

#
# Documentation (doxygen)
#
.PHONY : doc
doc: Doxyfile
	doxygen

#
# User configuration files
#
.IGNORE : user.make
user.make :

#
# For debugging the makefile settings
#
.PHONY : info
info:
	@echo "Machine : $(MACHINE)"
	@echo "GAME_ROM : $(GAME_ROM)"
	@echo "GCC_VERSION = $(GCC_VERSION)"
	@echo "CC = $(CC)"
	-@$(CC) -v
	@echo "AS = $(AS)"
	-@$(AS)
	@echo "CFLAGS = $(CFLAGS)"
	@echo "PAGEFLAGS = $(PAGEFLAGS)"
	@echo "BLANK_SIZE = $(BLANK_SIZE)"
	@echo "REQUIRED = $(REQUIRED)"
	@echo "PATH_REQUIRED = $(PATH_REQUIRED)"

#
# 'make clean' does what you think.
#
.PHONY : clean
clean: clean_derived clean_gendefines
	@for dir in `echo . kernel common fonts images test $(MACHINE)`;\
		do echo Removing files in \'$$dir\' ... && \
		cd $$dir && rm -f $(TMPFILES) && cd -; done

clean_derived:
	@for file in `echo $(XBM_H) mach include/mach` ;\
		do echo "Removing derived file $$file..." && \
		rm -f $$file; done && \
		rm -rf .mach .include_mach

show_objs:
	@echo $(OBJS)

