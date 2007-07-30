#
# FreeWPC makefile
#
# (C) Copyright 2005-2007 by Brian Dominy.
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

# Set this to the path where the final ROM image should be installed
# This is left blank on purpose: set this in your user.make file.
# There is no default value.
TARGET_ROMPATH =

# Which version of the 6809 compiler to use.  This is ignored for native builds.
GCC_VERSION ?= 3.4.6

# Set to 'y' if the tools have been configured to generate profiling
# information after each source file is compiled.
TOOL_PROFILING ?= n

# Build date (now)
BUILD_DATE = \"$(shell date +%m/%d/%y)\"

#######################################################################
###	Include User Settings
#######################################################################
-include user.make

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

BLD := build
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

PAGED_LINKCMD = $(PAGED_SECTIONS:%=$(BLD)/%.lnk)
#PAGED_LINKCMD = $(BLD)/page55.lnk $(BLD)/page56.lnk \
#$(BLD)/page57.lnk $(BLD)/page58.lnk \
#$(BLD)/page59.lnk $(BLD)/page60.lnk $(BLD)/page61.lnk

# The XBM prototype header file
XBM_H = images/xbmproto.h

SYSTEM_BINFILES = $(BLD)/freewpc.bin

PAGED_BINFILES = $(PAGED_SECTIONS:%=$(BLD)/%.bin)
#PAGED_BINFILES = $(BLD)/page55.bin $(BLD)/page56.bin $(BLD)/page57.bin \
#	$(BLD)/page58.bin \
#	$(BLD)/page59.bin $(BLD)/page60.bin $(BLD)/page61.bin

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
# Define GCC4 if GCC_VERSION begins with 4.
ifeq ($(PLATFORM),wpc)
GCC_ROOT = /usr/local/bin
CC := $(CCACHE) $(GCC_ROOT)/m6809-gcc-$(GCC_VERSION)
AS = $(CC) -xassembler-with-cpp
ifneq ($(shell echo $(GCC_VERSION) | grep ^4),)
EXTRA_CFLAGS += -DGCC4
endif
LD = $(GCC_ROOT)/m6809-ld
REQUIRED += $(CC:$(CCACHE)=) $(LD)
else
GCC_VERSION = NATIVE
endif

HOSTCC := $(CCACHE) gcc

TOOLS :=
HOST_OBJS :=

D := tools/srec2bin
include $(D)/srec2bin.make

D := tools/csum
include $(D)/csum.make

D := tools/wpcdebug
include $(D)/wpcdebug.make

D := tools/sched
include $(D)/sched.make

D := tools/fiftool
include $(D)/fiftool.make

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

# All internal tools are marked required for now
REQUIRED += $(TOOLS)

# Where pinmame is located
PINMAME ?= xpinmamed.x11
PINMAME_FLAGS = -skip_gameinfo -skip_disclaimer -si -s 2 -fs 8 $(EXTRA_PINMAME_FLAGS)


#######################################################################
###	Source and Binary Filenames
#######################################################################

# Basic kernel modules are generic and do not depend on
# the machine type at all.
KERNEL_BASIC_OBJS = \
	kernel/list.o \
	kernel/misc.o \
	kernel/puts.o \
	kernel/random.o \
	kernel/sysinfo.o

# Software kernel modules are hardware-agnostic, but may depend
# on machine flags and so need to be recompiled when the machine
# flags change.
KERNEL_SW_OBJS = \
	kernel/deff.o \
	kernel/font.o \
	kernel/game.o \
	kernel/lampset.o \
	kernel/player.o \
	kernel/printf.o \
	kernel/score.o

# Hardware kernel modules are incredibly hardware dependent,
# and are subject to change on different hardware platforms.
# This isn't really supported yet but it might be in the future
# (e.g. supporting System 11, or Whitestar, etc.)
KERNEL_OBJS = $(KERNEL_BASIC_OBJS) $(KERNEL_SW_OBJS) \
	kernel/ac.o \
	kernel/adj.o \
	kernel/audio.o \
	kernel/audit.o \
	kernel/csum.o \
	kernel/dmd.o \
	kernel/flasher.o \
	kernel/flip.o \
	kernel/init.o \
	kernel/irq.o \
	kernel/lamp.o \
	kernel/leff.o \
	kernel/reset.o \
	kernel/sol.o \
	kernel/sound.o \
	kernel/switches.o \
	kernel/timer.o \
	kernel/triac.o

# The 'common basic objects' are those that do not depend on autogenerated
# .h files, but are not important enough to do into the system region.
COMMON_BASIC_OBJS = \
	common/abort.o \
	common/coin.o \
	common/db.o \
	common/event-audit.o \
	common/flipcode.o \
	common/initials.o \
	common/match.o \
	common/music.o \
	common/plunger.o \
	common/rtc.o \
	common/service.o \
	common/tilt.o \
	common/tournament.o \
	common/trough.o \

# The other common objects are dependent on autogenerated .h files as well.
COMMON_OBJS = $(COMMON_BASIC_OBJS) \
	common/buyin.o \
	common/device.o \
	common/diag.o \
	common/eb.o \
	common/highscore.o \
	common/inspector.o \
	common/knocker.o \
	common/onecoin.o \
	common/replay.o \
	common/search.o \
	common/status.o \
	common/special.o

EFFECT_OBJS = \
	common/effect.o

EVENT_OBJS = build/callset.o

TEST_OBJS = test/window.o test/mix.o

TEST2_OBJS = test/format.o

TRANS_OBJS = kernel/dmdtrans.o


# FONT_OBJS are manually constructed and maintained in .c code.
# FON_OBJS are autogenerated from X fonts.  They are maintained as .fon
# files in source control, but can always be regenerated if necessary.
# TODO : only link in the fonts that the machine/system require

FONT_OBJS = fonts/mono5.o fonts/mono9.o fonts/var5.o fonts/tinynum.o \
fonts/bitmap.o

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

XBM_OBJS =

FIF_SRCS += \
	images/freewpc_logo.fif \
	images/tuxlogo.fif

FIF_OBJS = $(patsubst %.fif,%.o,$(FIF_SRCS))

BASIC_OBJS = $(KERNEL_BASIC_OBJS) $(COMMON_BASIC_OBJS) $(FONT_OBJS) $(TRANS_OBJS)

-include build/Makefile.xbms
XBM_OBJS +=	$(XBMGEN_OBJS)

OS_INCLUDES = include/freewpc.h include/platform/$(PLATFORM).h

INCLUDES = $(OS_INCLUDES) $(GAME_INCLUDES)

XBM_SRCS = $(patsubst %.o,%.xbm,$(XBM_OBJS))

FON_SRCS = $(patsubst %.o,%.fon,$(FON_OBJS))
export FON_SRCS

#######################################################################
###	Compiler / Assembler / Linker Flags
#######################################################################

CFLAGS :=

# Program include directories
CFLAGS += -Ibuild -I$(INCLUDE_DIR) -I$(MACHINE_DIR)

# Additional defines
CFLAGS += -DGCC_VERSION=$(GCC_VERSION)

# Please, turn on all warnings!
CFLAGS += -Wall -Wstrict-prototypes

#
# Define lots of other things based on make parameters
#
CFLAGS += -DBUILD_DATE=$(BUILD_DATE)

ifeq ($(FREEWPC_DEBUGGER),y)
CFLAGS += -DDEBUGGER 
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

CFLAGS += $(EXTRA_CFLAGS)

#######################################################################
###	Include Autogenerated Machine Extensions
#######################################################################
-include build/mach-Makefile

#######################################################################
###	Include Manual Machine Extensions
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

ifndef MACHINE_FILE
MACHINE_FILE = $(MACHINE).md
endif

ifdef GAME_FSMS
FSM_SRCS = $(GAME_FSMS:%.fsm=build/%.c)
FSM_OBJS = $(GAME_FSMS:%.fsm=build/%.o)
endif

# Add files generated by the machine description
PAGED_MD_OBJS = \
	build/mach-strings.o \
	build/mach-lampsets.o

SYSTEM_MD_OBJS = \
	build/mach-switchmasks.o \
	build/mach-scores.o \
	build/mach-switches.o \
	build/mach-containers.o \
	build/mach-deffs.o

MD_OBJS = $(PAGED_MD_OBJS) $(SYSTEM_MD_OBJS)


#######################################################################
###	Object File Distribution
#######################################################################

# Because WPC uses ROM paging, the linking job is more
# difficult to get right.  We require that the programmer
# explicitly state which pages things should belong in.
# TODO : if you overflow a page, bad things will happen.
# We should really check for this here.

# A list of the paged sections that we will use.  Not all pages
# are currently needed.
PAGE_NUMBERS = 55 56 57 58 59 60 61

PAGED_SECTIONS = $(foreach pg,$(PAGE_NUMBERS),page$(pg))
NUM_PAGED_SECTIONS := $(words $(PAGE_NUMBERS))
NUM_BLANK_PAGES := $(shell echo $(ROM_PAGE_COUNT) - 2 - $(NUM_PAGED_SECTIONS) | $(BC))
BLANK_SIZE := $(shell echo $(NUM_BLANK_PAGES) \* 16 | $(BC))

#
# The WPC physical memory map is divided into four sections.
# The build procedure can control which section is used for a
# particular function.
#
# The first 16-bytes of the nonvolatile area are reserved.
# PinMAME has a hack that overwrites this area.

AREA_LIST :=
define AREA_SETUP
ifneq ($(strip $4),virtual)
AREA_LIST += $(1)
endif
AREA_$(strip $1) = $(2)
AREASIZE_$(strip $1) = $(3)
endef

$(eval $(call AREA_SETUP, direct,    0x0004,   0x00FC))
$(eval $(call AREA_SETUP, ram,       0x0100,   0x1300))
$(eval $(call AREA_SETUP, local,     0x1400,   0x00A0))
$(eval $(call AREA_SETUP, stack,     0x1600,   0x0200,  virtual))
$(eval $(call AREA_SETUP, nvram,     0x1810,   0x07F0))
$(eval $(call AREA_SETUP, paged,     0x4000,   0x4000,  virtual))
$(eval $(call AREA_SETUP, sysrom,    0x8000,   0x7FF0,  virtual))
$(eval $(call AREA_SETUP, vector,    0xFFF0,   0x0010,  virtual))


MACHINE_OBJS = $(patsubst %,$(MACHINE_DIR)/%,$(GAME_OBJS))
MACHINE_TEST_OBJS = $(patsubst %,$(MACHINE_DIR)/%,$(GAME_TEST_OBJS))
MACHINE_PAGED_OBJS = $(patsubst %,$(MACHINE_DIR)/%,$(GAME_PAGED_OBJS))
SYSTEM_HEADER_OBJS =

#
# Define a mapping between object files and page numbers in
# which they should be placed.  This information must be
# provided in both directions.  Also, the mapping must be
# available to the C code in the form of xxx_PAGE macros
# for when the code wants to switch the page to a particular
# class of function.
#

# PAGE_INIT : Initialize each page to contain the page header object.
# $1 = the page number
define PAGE_INIT
page$(strip $1)_OBJS := $(BLD)/page$(strip $1).o
endef

# PAGE_ALLOC : Allocate a paged region for a particular class of objects.
# $1 = the page number
# $2 = the object class
# $3 = the page define (derived if $2 if not given)
# Example : PAGE_ALLOC(55,EFFECT)
define PAGE_ALLOC
page$(strip $1)_OBJS += $($(strip $2)_OBJS)
$($(strip $2)_OBJS) : PAGE=$(strip $1)
ifneq ($(strip $3),)
CFLAGS += -D$(strip $3)_PAGE=$(strip $1)
else
CFLAGS += -D$(strip $2)_PAGE=$(strip $1)
endif
endef

$(foreach page,$(PAGE_NUMBERS),$(eval $(call PAGE_INIT, $(page))))
$(eval $(call PAGE_ALLOC, 55, PAGED_MD, MD))
$(eval $(call PAGE_ALLOC, 55, EFFECT))
$(eval $(call PAGE_ALLOC, 56, COMMON))
$(eval $(call PAGE_ALLOC, 56, EVENT))
$(eval $(call PAGE_ALLOC, 57, TRANS))
$(eval $(call PAGE_ALLOC, 57, PRG))
$(eval $(call PAGE_ALLOC, 57, FIF))
$(eval $(call PAGE_ALLOC, 58, TEST))
$(eval $(call PAGE_ALLOC, 58, MACHINE_TEST))
$(eval $(call PAGE_ALLOC, 59, MACHINE_PAGED, MACHINE))
$(eval $(call PAGE_ALLOC, 59, FSM))
$(eval $(call PAGE_ALLOC, 60, XBM))
$(eval $(call PAGE_ALLOC, 60, TEST2))
$(eval $(call PAGE_ALLOC, 61, FONT))
$(eval $(call PAGE_ALLOC, 61, FON))

SYSTEM_OBJS := $(SYSTEM_MD_OBJS) $(SYSTEM_HEADER_OBJS) $(KERNEL_ASM_OBJS) $(KERNEL_OBJS) $(MACHINE_OBJS)
$(SYSTEM_OBJS) : PAGE=62
CFLAGS += -DSYS_PAGE=62

PAGED_OBJS = $(foreach area,$(PAGED_SECTIONS),$($(area)_OBJS))

PAGE_HEADER_OBJS = $(foreach area,$(PAGED_SECTIONS),$(BLD)/$(area).o)

AS_OBJS = $(SYSTEM_HEADER_OBJS) $(KERNEL_ASM_OBJS)

C_OBJS = $(MD_OBJS) $(KERNEL_OBJS) $(COMMON_OBJS) $(EVENT_OBJS) \
	$(TRANS_OBJS) $(TEST_OBJS) $(TEST2_OBJS) $(FSM_OBJS) \
	$(MACHINE_OBJS) $(MACHINE_PAGED_OBJS) $(MACHINE_TEST_OBJS) \
	$(FONT_OBJS) $(EFFECT_OBJS)


ifeq ($(PLATFORM),wpc)
OBJS = $(C_OBJS) $(AS_OBJS) $(XBM_OBJS) $(FIF_OBJS) $(FON_OBJS) $(PRG_OBJS)
else
OBJS = $(C_OBJS) $(XBM_OBJS) $(PRG_OBJS) $(FIF_OBJS) $(FON_OBJS)
endif

MACH_LINKS = .mach .include_mach

ifneq ($(nodefs),y)
MAKE_DEPS = Makefile $(MACHINE_DIR)/Makefile user.make 
endif

C_DEPS += build/mach-config.h
MACH_DESC = $(MACHINE_DIR)/$(MACHINE_FILE)
C_DEPS += $(MAKE_DEPS) $(INCLUDES) $(MACH_LINKS)

GENDEFINES = include/gendefine_gid.h

#######################################################################
###	Begin Makefile Targets
###   See 'default_target' above for which of these rules is actually
###   used as the root target.
#######################################################################

.DELETE_ON_ERROR :

.PHONY : clean_err
clean_err:
	@rm -f $(ERR)

.PHONY : check_prereqs
check_prereqs : xbmgen_run xbmprotos

.PHONY : run
run: install
	# Start pinmame up and let it run indefinitely.
	$(PINMAME) $(PINMAME_MACHINE) $(PINMAME_FLAGS) &

.PHONY : debug
debug: install
	# Start pinmame up and let it run indefinitely.
	$(PINMAME) $(PINMAME_MACHINE) -d $(PINMAME_FLAGS) &

.PHONY : rund
rund: install run $(DBCON)
	xterm -e "$(DBCON) -1" &

.PHONY : rund-file
rund-file: install run $(DBCON)
	xterm -e "$(DBCON) -1 > $(DBCON_FILE) 2>&1" &

.PHONY : endrun
endrun :
	kill $(shell pidof $(PINMAME))

.PHONY : timedrun
timedrun:
	$(MAKE) rund-file DBCON_FILE="test.log"
	sleep 30
	$(MAKE) endrun

.PHONY : install
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

.PHONY : uninstall
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
$(GAME_ROM) : $(BLD)/blank$(BLANK_SIZE).bin $(BINFILES) $(CSUM)
	@echo Padding ... && \
		cat $(BLD)/blank$(BLANK_SIZE).bin $(PAGED_BINFILES) $(SYSTEM_BINFILES) > $@
	@echo Updating ROM checksum ... && \
		$(CSUM) -f $(GAME_ROM) -v 0x$(SYSTEM_MINOR) -u

#
# How to make a blank file.  This creates an empty file of any desired size
# in multiples of 1KB.
#
$(BLD)/blank%.bin: $(BLD)/blankpage.bin
	@echo "Creating $*KB blank file ..." && $(BLANKER) if=/dev/zero of=$@ bs=1k count=$* > /dev/null 2>&1

$(BLD)/blankpage.bin: $(SR)
	@echo "Creating blank 16KB page ..." && $(SR) -o $@ -l 0x4000 -f 0 -B

$(SYSTEM_BINFILES) : %.bin : %.s19 $(SR)
	@echo Converting $< to binary ... && $(SR) -o $@ -s $(AREA_sysrom) -l 0x8000 -f 0 $<

$(PAGED_BINFILES) : %.bin : %.s19 $(SR)
	@echo Converting $< to binary ... && $(SR) -o $@ -s $(AREA_paged) -l $(AREASIZE_paged) -f 0xFF $<

#
# General rule for linking a group of object files.  The linker produces
# a Motorola S-record file by default (S19).
#
ifeq ($(PLATFORM),wpc)
$(BINFILES:.bin=.s19) : %.s19 : %.lnk $(OBJS) $(AS_OBJS) $(PAGE_HEADER_OBJS)
	@echo Linking $@... && $(CC) -Wl,-T -Wl,$< >> $(ERR) 2>&1
endif

ifeq ($(PLATFORM),linux)
freewpc : $(OBJS)
	@echo Linking ... && $(HOSTCC) $(HOST_LFLAGS) `pth-config --ldflags` -o freewpc $(OBJS) $(HOST_LIBS) >> $(ERR) 2>&1
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
# It is _extremely_ important that the order in which object files are
# named here is the same for every page, otherwise references won't match
# up with definitions.
#
# Two helper functions are defined.  OBJ_PAGE_LINKOPT returns the
# right linker option followed by the name of the object file, given
# an object filename and the name of the linker command file.
# OBJ_PAGE_LIST does the same thing, but for all object files rather
# than just one of them.  The output of OBJ_PAGE_LIST has all of the
# options/filenames on a single line; the linker requires that they all
# be on separate lines.  We use a for loop in the bash code to iterate
# over the OBJ_PAGE_LIST output to split it into multiple lines.

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

$(PAGED_LINKCMD) : $(MAKE_DEPS) build/Makefile.xbms platform/$(PLATFORM)/Makefile
	@echo Creating linker command file $@ ... ;\
	rm -f $@ ;\
	echo "-xswz" >> $@ ;\
	( $(foreach area,$(AREA_LIST),echo -b $(area) = $(AREA_$(area));) ) >> $@ ;\
	for f in `echo $(PAGED_SECTIONS)`; \
		do echo "-b $$f = $(AREA_paged)" >> $@ ;\
	done ;\
	echo "-b .text = $(AREA_sysrom)" >> $@ ;\
	echo "-o" >> $@ ;\
	echo "$(@:.lnk=.o)" >> $@ ;\
	echo "$(call DUP_PAGE_OBJ,$(@:.lnk=.o))" >> $@ ;\
	for f in `echo $(call OBJ_PAGE_LIST,$@)` ;\
	   do echo $$f >> $@ ;\
	done ;\
	echo "-e" >> $@


$(BLD)/freewpc.s:
	@echo ".area .text" >> $@

#
# How to build a page header source file.
#
$(BLD)/page%.s:
	@echo ".area page$*" >> $@
	@echo ".db $*" >> $@

#
# How to make the linker command file for the system section.
#
$(LINKCMD) : $(MAKE_DEPS) build/Makefile.xbms platform/$(PLATFORM)/Makefile
	@echo Creating linker command file $@ ... ;\
	rm -f $(LINKCMD) ;\
	echo "-mxswz" >> $(LINKCMD) ;\
	( $(foreach area,$(AREA_LIST),echo -b $(area) = $(AREA_$(area));) ) >> $(LINKCMD) ;\
	for f in `echo $(PAGED_SECTIONS)`; \
		do echo "-b $$f = $(AREA_paged)" >> $(LINKCMD); done ;\
	echo "-b .text = $(AREA_sysrom)" >> $(LINKCMD) ;\
	echo "-b vector = $(AREA_vector)" >> $(LINKCMD) ;\
	echo "$(BLD)/freewpc.o" >> $(LINKCMD) ;\
	for f in `echo $(SYSTEM_OBJS)`; do echo $$f >> $(LINKCMD); done ;\
	echo "-v" >> $(LINKCMD) ;\
	for f in `echo $(PAGED_OBJS)`; do echo $$f >> $(LINKCMD); done ;\
	echo "-e" >> $(LINKCMD)


#
# General rule for how to build any assembler file.  This uses the native
# assembler and not gcc.
#
$(AS_OBJS) : %.o : %.s $(REQUIRED)
	@echo Assembling $< ... && $(AS) $(EXTRA_ASFLAGS) -o $@ -c $< >> $(ERR) 2>&1

#
# General rule for how to build a page header, which is a special
# version of an assembly file.
#
$(PAGE_HEADER_OBJS) : $(BLD)/page%.o : $(BLD)/page%.s $(REQUIRED)
	@echo Assembling page header $< ... && $(AS) -o $@ -c $< >> $(ERR) 2>&1

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
$(PRG_OBJS) $(XBM_OBJS) $(FON_OBJS) $(FIF_OBJS): GCC_LANG=-x c

$(C_OBJS) : %.o : %.c 

$(XBM_OBJS) : %.o : %.xbm

$(FON_OBJS) : %.o : %.fon

$(PRG_OBJS) : %.o : %.prg

$(FIF_OBJS) : %.o : %.fif tools/fiftool/fiftool

$(filter-out $(BASIC_OBJS),$(C_OBJS)) : $(C_DEPS) $(GENDEFINES) $(REQUIRED)

$(BASIC_OBJS) $(FON_OBJS) : $(MAKE_DEPS) $(GENDEFINES) $(REQUIRED)

$(C_OBJS) $(XBM_OBJS) $(PRG_OBJS) $(FON_OBJS) $(FIF_OBJS):
ifeq ($(PLATFORM),wpc)
	@echo "Compiling $< (in page $(PAGE)) ..." && $(CC) -o $@ $(CFLAGS) -c $(PAGEFLAGS) -DPAGE=$(PAGE) -mfar-code-page=$(PAGE) $(GCC_LANG) $< >> $(ERR) 2>&1
else
	@echo "Compiling $< ..." && $(HOSTCC) -o $@ $(CFLAGS) -c $(PAGEFLAGS) $(GCC_LANG) $< >> $(ERR) 2>&1
endif
ifeq ($(TOOL_PROFILING),y)
	@mkdir -p gprof.data
	$(shell mv gmon.out gprof.data/gmon.$$RANDOM.out)
endif

#######################################################################
###	Machine Description Compiler
#######################################################################
CONFIG_CMDS = dump strings switchmasks containers switches scores lampsets deffs
CONFIG_SRCS = $(CONFIG_CMDS:%=build/mach-%.c)
CONFIG_FILES = build/mach-config.h $(CONFIG_SRCS)

.PHONY : config
config : $(CONFIG_FILES)

build/mach-Makefile : $(MACH_DESC)
	@echo "Regenerating $@ if necessary..." && \
	tools/genmachine $< makefile > $@.tmp && \
	tools/move-if-change $@.tmp $@

build/mach-config.h : $(MACH_DESC)
	@echo "Regenerating $@ if necessary..." && \
	tools/genmachine $< config > $@.tmp && \
	tools/move-if-change $@.tmp $@
	
$(CONFIG_SRCS) : build/mach-%.c : $(MACH_DESC) build/mach-config.h
	@echo "Regenerating $@ if necessary..." && \
	tools/genmachine $(MACH_DESC) $(@:build/mach-%.c=%) > $@.tmp && \
	tools/move-if-change $@.tmp $@

$(CONFIG_FILES) : tools/genmachine kernel/freewpc.md

ifdef GAME_FSMS
$(FSM_SRCS) : build/%.c : $(MACHINE_DIR)/%.fsm tools/fsmgen
	tools/fsmgen $(FSMFLAGS) -o $@ $<
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
	@echo "Generating callsets ... " && rm -f $@ \
		&& tools/gencallset -D build -D kernel -D common -D mach -D test \
			-D platform/$(PLATFORM) $(CALLSET_FLAGS)

.PHONY : callset_again
callset_again:
	rm -rf $(BLD)/callset.c && $(MAKE) callset

.PHONY : fonts clean-fonts
fonts clean-fonts:
	@echo "Making $@... " && $(MAKE) -f Makefile.fonts $@

#######################################################################
###	Host Tools
#######################################################################

tools : $(TOOLS)

$(TOOLS) $(HOST_OBJS) : CC=$(HOSTCC) 

$(HOST_OBJS) : CFLAGS=-Wall -I.

$(HOST_OBJS) : %.o : %.c
	$(CC) $(CFLAGS) $(TOOL_CFLAGS) -o $@ -c $<

#######################################################################
###	XBM Generators
#######################################################################

HOST_XBM_LIBS = build/pgmlib.o

HOST_XBM_OBJS = \
	build/sysgen.o build/borders.o build/backgrounds.o

HOST_XBM_CFLAGS = -Itools/pgmlib -Iinclude -g

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

# Provide a target for user.make, but don't generate an error if
# it doesn't exist.  If it does exist, changes to it will be considered
# as a prerequisite for rebuilding nearly everything.
.IGNORE : user.make
user.make:

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
	@echo "TOOLS = $(TOOLS)"
	@echo "REQUIRED = $(REQUIRED)"
	@echo "PATH_REQUIRED = $(PATH_REQUIRED)"
	@echo "FIF_OBJS = $(FIF_OBJS)"

#
# 'make clean' does what you think.
#
.PHONY : clean
clean: clean_derived clean_gendefines clean_tools
	@for dir in `echo . kernel common fonts images test $(MACHINE_DIR) $(PLATFORM_DIR)`;\
		do echo Removing files in \'$$dir\' ... && \
		cd $$dir && rm -f $(TMPFILES) && cd -; done

.PHONY : clean_derived
clean_derived:
	@for file in `echo $(XBM_H) mach include/mach` ;\
		do echo "Removing derived file $$file..." && \
		rm -f $$file; done && \
		rm -rf .mach .include_mach && \
		rm -f *.s *.i

.PHONY : clean_tools
clean_tools:
	rm -f $(HOST_OBJS) $(TOOLS)

.PHONY : show_objs
show_objs:
	@echo $(OBJS)

