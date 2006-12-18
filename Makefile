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

# Which version of the 6809 compiler to use.  This is ignored for native builds.
GCC_VERSION ?= 3.4.6

# Set to 'y' if you want to save the assembly sources
SAVE_ASM ?= n

# Build date (now)
BUILD_DATE = \"$(shell date +%m/%d/%y)\"

# Uncomment this if you want extra debug notes from the compiler.
# Normally, you do not want to turn this on.
DEBUG_COMPILER ?= n

#######################################################################
###	Include User Settings
#######################################################################
-include user.make
.IGNORE : user.make

#######################################################################
###	Set Default Target
#######################################################################
ifeq ($(PLATFORM),linux)
default_target : clean_err check_prereqs freewpc
else
ifdef TARGET_ROMPATH
default_target : clean_err check_prereqs install post_build
else
default_target : clean_err check_prereqs build post_build
endif
endif


#######################################################################
###	Directories
#######################################################################

INCLUDE_DIR = ./include
MACHINE_DIR = machine/$(MACHINE)
PLATFORM_DIR = platform/$(PLATFORM)

#######################################################################
###	Filenames
#######################################################################

# Where to write errors
ERR = err
TMPFILES += $(ERR)

# The linker command file (generated dynamically)
LINKCMD = $(BLD)/freewpc.lnk
PAGED_LINKCMD = $(BLD)/page55.lnk $(BLD)/page56.lnk \
	$(BLD)/page57.lnk $(BLD)/page58.lnk \
	$(BLD)/page59.lnk $(BLD)/page60.lnk $(BLD)/page61.lnk

# The XBM prototype header file
XBM_H = images/xbmproto.h

SYSTEM_BINFILES = $(BLD)/freewpc.bin
PAGED_BINFILES = $(BLD)/page55.bin $(BLD)/page56.bin $(BLD)/page57.bin \
	$(BLD)/page58.bin \
	$(BLD)/page59.bin $(BLD)/page60.bin $(BLD)/page61.bin

BINFILES = $(SYSTEM_BINFILES) $(PAGED_BINFILES)
TMPFILES += $(LINKCMD)

TMPFILES += *.o
TMPFILES += *.rom
TMPFILES += *.lst
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
ifeq ($(PLATFORM),wpc)
GCC_ROOT = /usr/local/m6809/bin
else
GCC_ROOT = /usr/bin
GCC_VERSION =
endif

ifdef GCC_VERSION
CC := $(GCC_ROOT)/gcc-$(GCC_VERSION)
else
CC := $(GCC_ROOT)/gcc
endif

HOSTCC = gcc

# We use the latest versions of astools, version 4.1.0
LD6809 = $(GCC_ROOT)/aslink
LD = $(GCC_ROOT)/ld
AS = $(GCC_ROOT)/as

ifeq ($(PLATFORM),wpc)
REQUIRED += $(CC) $(LD6809) $(LD)
else
REQUIRED += $(CC) $(LD)
endif

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

KERNEL_BASIC_OBJS = \
	kernel/misc.o \
	kernel/random.o \
	kernel/sysinfo.o

KERNEL_OBJS = $(KERNEL_BASIC_OBJS) \
	kernel/ac.o \
	kernel/adj.o \
	kernel/audit.o \
	kernel/coin.o \
	kernel/csum.o \
	kernel/deff.o \
	kernel/device.o \
	kernel/dmd.o \
	kernel/flasher.o \
	kernel/flip.o \
	kernel/font.o \
	kernel/game.o \
	kernel/init.o \
	kernel/lamp.o \
	kernel/lampset.o \
	kernel/leff.o \
	kernel/player.o \
	kernel/printf.o \
	kernel/reset.o \
	kernel/score.o \
	kernel/sol.o \
	kernel/sound.o \
	kernel/switches.o \
	kernel/timer.o \
	kernel/triac.o \
	kernel/tilt.o

COMMON_BASIC_OBJS = \
	common/audio.o \
	common/db.o \
	common/event-audit.o \
	common/initials.o \
	common/match.o \
	common/rtc.o \
	common/service.o \
	common/trough.o \

COMMON_OBJS = $(COMMON_BASIC_OBJS) \
	common/buyin.o \
	common/diag.o \
	common/eb.o \
	common/highscore.o \
	common/inspector.o \
	common/knocker.o \
	common/replay.o \
	common/search.o \
	common/status.o \

EVENT_OBJS = build/callset.o

TEST_OBJS = test/window.o

TRANS_OBJS = kernel/dmdtrans.o

FONT_OBJS = fonts/mono5.o fonts/mono9.o fonts/var5.o fonts/bitmap.o

FON_OBJS = \
	fonts/fixed10.o \
	fonts/fixed12.o \
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

BASIC_OBJS = $(KERNEL_BASIC_OBJS) $(COMMON_BASIC_OBJS) $(FONT_OBJS) $(TRANS_OBJS)

-include build/Makefile.xbms
XBM_OBJS +=	$(XBMGEN_OBJS)

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

# Program include directories
CFLAGS += -Ibuild -I$(INCLUDE_DIR) -I$(MACHINE_DIR)

# Additional defines
ifdef GCC_VERSION
CFLAGS += -DGCC_VERSION=$(GCC_VERSION)
else
CFLAGS += -DGCC_VERSION=3.4.6
endif

# Default optimizations.  -O2 works OK for me, but hasn't always; you might
# want to fall back to -O1 if you have problems.
ifndef OPT
OPT = -O2
endif
CFLAGS += $(OPT) -fstrength-reduce -frerun-loop-opt -Wunknown-pragmas -foptimize-sibling-calls -fstrict-aliasing -fregmove

# This didn't work before, but now it does!
# However, it is still disabled by default.
# This implies -fstrength-reduce and -frerun-cse-after-loop
ifdef UNROLL_LOOPS
CFLAGS += -funroll-loops
endif


# Turn on compiler debug.  This will cause a bunch of compiler
# debug files to get written out during every phase of the build.
ifeq ($(DEBUG_COMPILER),y)
CFLAGS += -da -dA
endif

# Please, turn on all warnings!  But don't check format strings,
# because we define those differently than ANSI C.
CFLAGS += -Wall -Wno-format


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
ifeq ($(FREE_ONLY),y)
CFLAGS += -DFREE_ONLY
endif

#######################################################################
###	Include Machine Extensions
#######################################################################
include machine/$(MACHINE)/Makefile

#######################################################################
###	Include Platform Extensions
#######################################################################
include platform/$(PLATFORM)/Makefile

# Fix up names based on machine definitions
ifdef GAME_ROM_PREFIX
GAME_ROM = $(GAME_ROM_PREFIX)$(MACHINE_MAJOR)_$(MACHINE_MINOR).rom
else
GAME_ROM = freewpc.rom
endif

ifdef GAME_FSMS
FSM_SRCS = $(GAME_FSMS:%.fsm=build/%.c)
FSM_OBJS = $(GAME_FSMS:%.fsm=build/%.o)
endif

# Add files generated by the machine description
ifdef MACHINE_FILE
PAGED_MD_OBJS = \
	build/mach-strings.o

SYSTEM_MD_OBJS = \
	build/mach-switchmasks.o \
	build/mach-scores.o \
	build/mach-switches.o \
	build/mach-containers.o \
	build/mach-lampsets.o

MD_OBJS = $(PAGED_MD_OBJS) $(SYSTEM_MD_OBJS)
endif


#######################################################################
###	Object File Distribution
#######################################################################

# Because WPC uses ROM paging, the linking job is more
# difficult to get right.  We require that the programmer
# explicitly state which pages things should belong in.

PAGED_SECTIONS = page55 page56 page57 page58 page59 page60 page61

NUM_PAGED_SECTIONS := 7

NUM_BLANK_PAGES := \
	$(shell echo $(ROM_PAGE_COUNT) - 2 - $(NUM_PAGED_SECTIONS) | $(BC))

BLANK_SIZE := $(shell echo $(NUM_BLANK_PAGES) \* 16 | $(BC))

#
# The WPC physical memory map is divided into four sections.
# The build procedure can control which section is used for a
# particular function.
#

DIRECT_AREA = 0x4
RAM_AREA = 0x100
DIRECT_LNK_CMD = "-b direct = $(DIRECT_AREA)"

LOCAL_AREA = 0x1400
LOCAL_AREA_SIZE = 0xA0
LOCAL_SAVE_AREA = 0x12A0
LOCAL_LNK_CMD = "-b local = $(LOCAL_AREA)"

STACK_AREA = 0x1600
STACK_AREA_SIZE = 0x200

# The first 16-bytes of the nonvolatile area are reserved.
# PinMAME has a hack that overwrites this area.
NVRAM_AREA = 0x1810

PAGED_AREA = 0x4000
FIXED_AREA = 0x8000
VECTOR_AREA = 0xFFF0

MACHINE_OBJS = $(patsubst %,$(MACHINE_DIR)/%,$(GAME_OBJS))
MACHINE_TEST_OBJS = $(patsubst %,$(MACHINE_DIR)/%,$(GAME_TEST_OBJS))
MACHINE_PAGED_OBJS = $(patsubst %,$(MACHINE_DIR)/%,$(GAME_PAGED_OBJS))
SYSTEM_HEADER_OBJS =	$(BLD)/freewpc.o

#
# Define a mapping between object files and page numbers in
# which they should be placed.  This information must be
# provided in both directions.  Also, the mapping must be
# available to the C code in the form of xxx_PAGE macros
# for when the code wants to switch the page to a particular
# class of function.
#
page55_OBJS = $(BLD)/page55.o $(PAGED_MD_OBJS)
page56_OBJS = $(BLD)/page56.o $(COMMON_OBJS) $(EVENT_OBJS)
page57_OBJS = $(BLD)/page57.o $(TRANS_OBJS)
page58_OBJS = $(BLD)/page58.o $(TEST_OBJS) $(MACHINE_TEST_OBJS)
page59_OBJS = $(BLD)/page59.o $(MACHINE_PAGED_OBJS) $(FSM_OBJS)
page60_OBJS = $(BLD)/page60.o $(XBM_OBJS)
page61_OBJS = $(BLD)/page61.o $(FONT_OBJS) $(FON_OBJS)
SYSTEM_OBJS = $(SYSTEM_MD_OBJS) $(SYSTEM_HEADER_OBJS) $(KERNEL_ASM_OBJS) $(KERNEL_OBJS) $(MACHINE_OBJS)

$(PAGED_MD_OBJS) : PAGE=55
$(COMMON_OBJS) $(EVENT_OBJS) : PAGE=56
$(TRANS_OBJS) : PAGE=57
$(TEST_OBJS) $(MACHINE_TEST_OBJS): PAGE=58
$(MACHINE_PAGED_OBJS) $(FSM_OBJS): PAGE=59
$(XBM_OBJS) : PAGE=60
$(FONT_OBJS) $(FON_OBJS) : PAGE=61
$(SYSTEM_OBJS) : PAGE=62

PAGE_DEFINES := -DMD_PAGE=55 -DCOMMON_PAGE=56 -DEVENT_PAGE=56 -DTRANS_PAGE=57 -DTEST_PAGE=58 -DMACHINE_PAGE=59 -DXBM_PAGE=60 -DFONT_PAGE=61 -DSYS_PAGE=62
CFLAGS += $(PAGE_DEFINES)


PAGED_OBJS = $(page55_OBJS) $(page56_OBJS) $(page57_OBJS) \
				 $(page58_OBJS) $(page59_OBJS) $(page60_OBJS) $(page61_OBJS)


PAGE_HEADER_OBJS = $(BLD)/page55.o $(BLD)/page56.o $(BLD)/page57.o \
	$(BLD)/page58.o $(BLD)/page59.o $(BLD)/page60.o $(BLD)/page61.o

AS_OBJS = $(SYSTEM_HEADER_OBJS) $(KERNEL_ASM_OBJS)

C_OBJS = $(MD_OBJS) $(KERNEL_OBJS) $(COMMON_OBJS) $(EVENT_OBJS) \
	$(TRANS_OBJS) $(TEST_OBJS) $(FSM_OBJS) \
	$(MACHINE_OBJS) $(MACHINE_PAGED_OBJS) $(MACHINE_TEST_OBJS) \
	$(FONT_OBJS)


ifeq ($(PLATFORM),wpc)
OBJS = $(C_OBJS) $(AS_OBJS) $(XBM_OBJS) $(FON_OBJS)
else
OBJS = $(C_OBJS) $(XBM_OBJS) $(FON_OBJS)
endif

MACH_LINKS = .mach .include_mach

ifneq ($(nodefs),y)
MAKE_DEPS = Makefile $(MACHINE_DIR)/Makefile user.make 
endif

ifdef MACHINE_FILE
C_DEPS += build/mach-config.h
MACH_DESC = $(MACHINE_DIR)/$(MACHINE_FILE)
CFLAGS += -DUSE_MD
endif
C_DEPS += $(MAKE_DEPS) $(INCLUDES) $(MACH_LINKS)

GENDEFINES = include/gendefine_gid.h
ifndef MACHINE_FILE
GENDEFINES += include/gendefine_deff.h \
	include/gendefine_leff.h \
	include/gendefine_lampset.h \
	include/gendefine_devno.h
endif

#######################################################################
###	Begin Makefile Targets
#######################################################################

.DELETE_ON_ERROR :

.PHONY : clean_err
clean_err:
	@rm -f $(ERR)

.PHONY : check_prereqs
check_prereqs : xbmgen_run xbmprotos

install : $(TARGET_ROMPATH)/$(PINMAME_MACHINE).zip

$(TARGET_ROMPATH)/$(PINMAME_MACHINE).zip : $(TARGET_ROMPATH)/$(PINMAME_GAME_ROM)
	@echo Installing to MAME directory '$(TARGET_ROMPATH)' ...; \
	cd $(TARGET_ROMPATH); \
	if [ ! -f $(PINMAME_MACHINE).zip.original ]; then \
		echo "Saving original MAME roms..."; \
		mv $(PINMAME_MACHINE).zip $(PINMAME_MACHINE).zip.original; \
	fi; \
	rm -f $(PINMAME_MACHINE).zip; \
	zip $(PINMAME_MACHINE).zip $(PINMAME_GAME_ROM) $(PINMAME_OTHER_ROMS)

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
build : $(GAME_ROM)

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
	@echo Converting $< to binary ... && $(SR) -o $@ -s $(FIXED_AREA) -l 0x8000 -f 0 $<

$(PAGED_BINFILES) : %.bin : %.s19 $(SR)
	@echo Converting $< to binary ... && $(SR) -o $@ -s $(PAGED_AREA) -l 0x4000 -f 0xFF $<

#
# General rule for linking a group of object files.  The linker produces
# a Motorola S-record file by default (S19).
#
ifeq ($(PLATFORM),wpc)
$(BINFILES:.bin=.s19) : %.s19 : %.lnk $(LD6809) $(OBJS) $(AS_OBJS) $(PAGE_HEADER_OBJS)
	@echo Linking $@... && $(LD6809) -f $< >> $(ERR) 2>&1
endif

ifeq ($(PLATFORM),linux)
freewpc : $(OBJS)
	@echo Linking ... && $(HOSTCC) `pth-config --ldflags` -o freewpc $(OBJS) -lpth >> $(ERR) 2>&1
endif

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
DUP_PAGE_OBJ = $1

$(PAGED_LINKCMD) : $(MAKE_DEPS) build/Makefile.xbms
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
$(LINKCMD) : $(MAKE_DEPS) build/Makefile.xbms
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
	echo "-e" >> $(LINKCMD)


#
# General rule for how to build any assembler file.  This uses the native
# assembler and not gcc.
#
$(AS_OBJS) : %.o : %.s $(REQUIRED)
	@echo Assembling $< ... && $(AS) -o $@ $< >> $(ERR) 2>&1

#
# General rule for how to build a page header, which is a special
# version of an assembly file.
#
$(PAGE_HEADER_OBJS) : $(BLD)/page%.o : $(BLD)/page%.s $(REQUIRED)
	@echo Assembling page header $< ... && $(AS) -o $@ $< >> $(ERR) 2>&1

#
# General rule for how to build any C or XBM module.
# The basic rule is the same, but with a few differences that are
# handled through some extra variables:
#
ifeq ($(PLATFORM),wpc)
$(C_OBJS) : PAGEFLAGS="-DDECLARE_PAGED=__attribute__((section(\"page$(PAGE)\")))"
$(XBM_OBJS) $(FON_OBJS): PAGEFLAGS="-Dstatic=__attribute__((section(\"page$(PAGE)\")))"
else
$(C_OBJS) : PAGEFLAGS=-DDECLARE_PAGED=
$(XBM_OBJS) : PAGEFLAGS=-Dstatic=
endif

$(C_OBJS) : GCC_LANG=
$(XBM_OBJS) $(FON_OBJS): GCC_LANG=-x c

$(C_OBJS) : %.o : %.c 

$(filter-out $(BASIC_OBJS),$(C_OBJS)) : $(C_DEPS) $(GENDEFINES) $(REQUIRED)
$(BASIC_OBJS) : $(MAKE_DEPS) $(GENDEFINES) $(REQUIRED)

$(XBM_OBJS) : %.o : %.xbm

$(FON_OBJS) : %.o : %.fon

$(C_OBJS) $(XBM_OBJS) $(FON_OBJS):
ifeq ($(PLATFORM),wpc)
	@echo "Compiling $< (in page $(PAGE)) ..." && $(CC) -o $@ $(CFLAGS) -c $(PAGEFLAGS) -DPAGE=$(PAGE) -mfar-code-page=$(PAGE) $(GCC_LANG) $< >> $(ERR) 2>&1
else
	@echo "Compiling $< ..." && $(HOSTCC) -o $@ $(CFLAGS) -c $(PAGEFLAGS) $(GCC_LANG) $< >> $(ERR) 2>&1
endif

#######################################################################
###	Machine Description Compiler
#######################################################################
ifdef MACHINE_FILE
CONFIG_CMDS = dump strings switchmasks containers switches scores lampsets
CONFIG_SRCS = $(CONFIG_CMDS:%=build/mach-%.c)
CONFIG_FILES = build/mach-config.h $(CONFIG_SRCS)

.PHONY : config
config : $(CONFIG_FILES)

build/mach-config.h : $(MACH_DESC)
	@echo "Regenerating $@ if necessary..." && \
	tools/genmachine $< config > $@.tmp && \
	tools/move-if-change $@.tmp $@
	
build/mach-%.c : $(MACH_DESC) build/mach-config.h
	@echo "Regenerating $@ if necessary..." && \
	tools/genmachine $(MACH_DESC) $(@:build/mach-%.c=%) > $@.tmp && \
	tools/move-if-change $@.tmp $@

$(CONFIG_FILES) : tools/genmachine kernel/freewpc.md
endif

ifdef GAME_FSMS
$(FSM_SRCS) : build/%.c : $(MACHINE_DIR)/%.fsm tools/fsmgen
	tools/fsmgen -d -o $@ $<
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

include/gendefine_gid.h: $(FSM_SRCS) $(CONFIG_SRCS)
	@echo Autogenerating task IDs... && \
		$(GENDEFINE) -p GID_ > $@

ifndef MACHINE_FILE
include/gendefine_leff.h :
	@echo Autogenerating lamp effect IDs... && \
		$(GENDEFINE) -p LEFF_ -c MAX_LEFFS > $@

include/gendefine_deff.h :
	@echo Autogenerating display effect IDs... && \
		$(GENDEFINE) -p DEFF_ -c MAX_DEFFS > $@

include/gendefine_lampset.h :
	@echo Autogenerating lampset IDs... && \
		$(GENDEFINE) -p LAMPSET_ -c MAX_LAMPSET > $@

include/gendefine_devno.h :
	@echo Autogenerating device IDs... && \
		$(GENDEFINE) -p DEVNO_ -f 0 -c NUM_DEVICES > $@
endif

.PHONY : clean_gendefines
clean_gendefines:
	@echo Deleting autogenerated files... && rm -f $(GENDEFINES)

.PHONY : gendefines_again
gendefines_again: clean_gendefines gendefines

#
# How to automake callsets
#
.PHONY : callset
callset: $(BLD)/callset.o

$(BLD)/callset.c : $(MACH_LINKS) $(CONFIG_SRCS) tools/gencallset
	@echo "Generating callsets ... " && rm -f $@ && tools/gencallset

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


#######################################################################
###	XBM Generators
#######################################################################

HOST_XBM_LIBS = build/pgmlib.o

HOST_XBM_OBJS = \
	build/sysgen.o build/borders.o build/backgrounds.o

HOST_XBM_CFLAGS = -Itools/pgmlib -g

xbmgen_clean :
	@echo "Cleaning host XBM files..." && \
	rm -f build/xbmgen $(HOST_XBM_LIBS) $(HOST_XBM_OBJS)

.PHONY : xbmgen_run
xbmgen_run : build/xbmgen
	@$(MAKE) xbmgen_objs

build/Makefile.xbms build/xbmgen : $(HOST_XBM_LIBS) $(HOST_XBM_OBJS)
	@echo "Linking XBM generator..." && \
	$(HOSTCC) -o build/xbmgen $(HOST_XBM_LIBS) $(HOST_XBM_OBJS) && \
	echo "Generating XBM files..." && build/xbmgen

$(HOST_XBM_OBJS) : build/%.o : images/%.c
	$(HOSTCC) -o $@ -c $< $(HOST_XBM_CFLAGS)

build/pgmlib.o : tools/pgmlib/pgmlib.c
	$(HOSTCC) -o $@ -c $< $(HOST_XBM_CFLAGS)


#######################################################################
###	Standard Dependencies
#######################################################################


#
# Symbolic links to the machine code.  Once set, code can reference
# 'mach' and 'include/mach' without knowing the specific machine type.
#
.mach:
	@echo Setting symbolic link for machine source code &&\
		touch .mach && ln -s $(MACHINE_DIR) mach

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
.PHONY : doxygen
doxygen: Doxyfile
	doxygen

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
	-@$(AS) --version
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
	@for dir in `echo . kernel common fonts images test $(MACHINE_DIR) $(PLATFORM_DIR)`;\
		do echo Removing files in \'$$dir\' ... && \
		cd $$dir && rm -f $(TMPFILES) && cd -; done

clean_derived:
	@for file in `echo $(XBM_H) mach include/mach` ;\
		do echo "Removing derived file $$file..." && \
		rm -f $$file; done && \
		rm -rf .mach .include_mach && \
		rm -f *.s *.i

show_objs:
	@echo $(OBJS)

