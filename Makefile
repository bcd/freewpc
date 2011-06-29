#
# FreeWPC makefile
#
# (C) Copyright 2005-2010 by Brian Dominy.
#
# This Makefile can be used to build an entire, FreeWPC game ROM
# from source code.
#
# To build the product, just type "make".
#
# To customize the build, create a file named .config.
# See config.example for an example of how this should look.
# The settings in .config override any defaults given below.
#
# By default, make will also install your game ROM into your pinmame
# ROMs directory.  The original MAME zip file will be saved into
# a file with the .original extension appended.  You can do a
# "make uninstall" to delete the FreeWPC zip and rename the original
# zip, so that things are back to the way that they started.
#

SHELL := /bin/bash
export SHELL

# Include the .config file, which must exist
DOTCONFIG ?= .config
ifneq ($(DOTCONFIG),)
include $(DOTCONFIG)
endif

define require
$(if $($1),,$(error $1 is not defined : $($1)))
endef

define have
$1 := y
AUTO_CFLAGS += -D$1
EXTRA_ASFLAGS += -D$1
SCHED_FLAGS += -D $1
endef

define nohave
$1 :=
AUTO_CFLAGS += -U$1
EXTRA_ASFLAGS += -U$1
endef

ifndef Q
Q := @
endif

#######################################################################
###	Directories
#######################################################################

BLDDIR := build
INCLUDE_DIR = ./include


#######################################################################
###	Configuration
#######################################################################

# MACHINE says which machine you are targetting.  It must be defined.
# This loads in per-machine rules.
$(eval $(call require,MACHINE))

M := machine/$(MACHINE)
MMAKEFILE := $(M)/Makefile
MACH_DESC = $(MACHINE_DIR)/$(MACHINE_FILE)
MACHINE_DIR = machine/$(MACHINE)
PLATFORM_DESC = $(shell grep include $(MACH_DESC) | head -n 1)

top_target : default_target

include $(BLDDIR)/mach-Makefile
include $(MMAKEFILE)

# MACHINE_FILE must be set by the machine Makefile.  We can
# grep it to set additional configuration variables.
$(eval $(call require,MACHINE_FILE))
PLATFORM ?= wpc

# PLATFORM says which hardware platform is targeted.  Valid values
# are 'wpc' and 'whitestar'.  The MACHINE Makefile should have
# defined this.
$(eval $(call require,PLATFORM))
PLATFORM_DIR = platform/$(PLATFORM)

#######################################################################
###	Set Default Target
#######################################################################

.PHONY : default_target

default_target : post_compile
post_compile : platform_target
platform_target : check_prereqs
check_prereqs : clean_err

KERNEL_OBJS :=
COMMON_BASIC_OBJS :=

# TODO : include the real, non-sim Makefile after the simulation
# Makefile too.
ifeq ($(CONFIG_SIM), y)
include sim/Makefile
else
PMAKEFILE := platform/$(PLATFORM)/Makefile
-include $(PMAKEFILE)
endif


# Build date (now)
BUILD_MONTH := $(shell date +%-m)
BUILD_DAY := $(shell date +%-d)
BUILD_YEAR := $(shell date +%Y)


.PHONY : platform_target
ifeq ($(CONFIG_SIM), y)
platform_target : native
else
ifdef TARGET_ROMPATH
platform_target : install
else
platform_target : compile
endif
endif


#######################################################################
###	Filenames
#######################################################################

# Where to write errors
ERR = err
TMPFILES += $(ERR)

# The linker command file (generated dynamically)
LINKCMD = $(BLDDIR)/freewpc.lnk

PAGED_LINKCMD = $(PAGED_SECTIONS:%=$(BLDDIR)/%.lnk)

SYSTEM_BINFILE = $(BLDDIR)/freewpc.bin

PAGED_BINFILES = $(PAGED_SECTIONS:%=$(BLDDIR)/%.bin)

BINFILES = $(SYSTEM_BINFILE) $(PAGED_BINFILES)
TMPFILES += $(LINKCMD)

TMPFILES += *.o
TMPFILES += *.lst
TMPFILES += *.i
TMPFILES += *.c.[0-9]*.*
TMPFILES += *.fon.[0-9]*.*
TMPFILES += *.out
TMPFILES += *.gcda
TMPFILES += *.gcno
TMPFILES += $(ERR)

#######################################################################
###	Programs
#######################################################################

# Path to the compiler and linker
ifeq ($(CPU),m6809)
GCC_ROOT ?= /usr/local/bin
CC := $(GCC_ROOT)/m6809-unknown-none-gcc-$(GCC_VERSION)
AS = $(CC) -xassembler-with-cpp
LD = $(GCC_ROOT)/m6809-unknown-none-ld
REQUIRED += $(CC) $(LD)
else
GCC_VERSION = NATIVE
endif

HOSTCC := gcc

###
###	List host tools which are required in order to build.
###	These are always compiled using your native C compiler
###	and run locally on the build machine, even when cross-
###	compiling for a different architecture.
###
###	Each host tool declares its build rules in a <tool>.make
###	file in the tools/<tool> subdirectory.
###
TOOLS :=
HOST_OBJS :=

define include-tool
D := tools/$1
include tools/$1/$1.make
endef

$(eval $(call include-tool,sched))       # Realtime scheduler
$(eval $(call include-tool,imgld))       # Image linker

ifeq ($(CPU),m6809)
$(eval $(call include-tool,srec2bin))    # SREC to binary converter
$(eval $(call include-tool,csum))        # Checksum update utility
$(eval $(call include-tool,wpcdebug))    # Emulated debug console
endif

ifdef CONFIG_OLD_HOST_TOOLS
$(eval $(call include-tool,softscope))   # Signal scope #1
$(eval $(call include-tool,scope))       # Signal scope #2
$(eval $(call include-tool,bin2c))       # Binary-to-C converter.
endif

# Name of the blanker to use
BLANKER = dd
PATH_REQUIRED += $(BLANKER)

# The gendefine script
GENDEFINE = tools/gendefine

# Where pinmame is located
PINMAME ?= xpinmamed.x11
PINMAME_FLAGS = -skip_gameinfo -skip_disclaimer -si -s 2 -fs 8 $(EXTRA_PINMAME_FLAGS)

# The template compiler
CTEMP=tools/ctemp -o $(BLDDIR)

#######################################################################
###	Source and Binary Filenames
#######################################################################

include kernel/Makefile
include common/Makefile
ifeq ($(CONFIG_FONT),y)
include fonts/Makefile
endif
ifdef CONFIG_PLATFORM_WPC
include test/Makefile
endif
# FUTURE : include effect/Makefile


EVENT_OBJS = $(BLDDIR)/callset.o

BASIC_OBJS = $(KERNEL_BASIC_OBJS) $(COMMON_BASIC_OBJS) $(FONT_OBJS) $(TRANS_OBJS)

OS_INCLUDES = include/freewpc.h include/platform/$(PLATFORM).h

INCLUDES = $(OS_INCLUDES) $(GAME_INCLUDES)

FON_SRCS = $(patsubst %.o,%.fon,$(FON_OBJS))
export FON_SRCS

TEMPLATE_SRCS = $(patsubst %.o,%.c,$(TEMPLATE_OBJS))

#######################################################################
###	Compiler / Assembler / Linker Flags
#######################################################################

# Program include directories
CFLAGS += -I$(BLDDIR) -I$(INCLUDE_DIR) -I$(MACHINE_DIR)
EXTRA_ASFLAGS += -I$(BLDDIR) -I$(INCLUDE_DIR) -I$(MACHINE_DIR)

# Additional defines
CFLAGS += -DGCC_VERSION=$(GCC_VERSION)

# Please, turn on all warnings!
CFLAGS += -Wall -Wstrict-prototypes

#
# Define lots of other things based on make parameters
#
CFLAGS += -DBUILD_MONTH=$(BUILD_MONTH) -DBUILD_DAY=$(BUILD_DAY) -DBUILD_YEAR=$(BUILD_YEAR)

SYSTEM_MAJOR ?= 1
SYSTEM_MINOR ?= 10
SYSTEM_EXTRAVERSION ?= DEV
MACHINE_MAJOR ?= 0
MACHINE_MINOR ?= 00
CFLAGS += -DFREEWPC_MAJOR_VERSION=$(SYSTEM_MAJOR)
CFLAGS += -DFREEWPC_MINOR_VERSION=$(SYSTEM_MINOR)
CFLAGS += -DFREEWPC_EXTRA_VERSION=$(SYSTEM_EXTRAVERSION)
CFLAGS += -DMACHINE_MAJOR_VERSION=$(MACHINE_MAJOR)
CFLAGS += -DMACHINE_MINOR_VERSION=$(MACHINE_MINOR)
ifeq ($(FREEWPC_DEBUGGER),y)
CFLAGS += -DDEBUGGER
EXTRA_ASFLAGS += -DDEBUGGER
endif

CFLAGS += $(EXTRA_CFLAGS) $(AUTO_CFLAGS)

SCHED_HEADERS := include/freewpc.h include/interrupt.h $(SCHED_HEADERS)
SCHED_FLAGS += $(patsubst %,-i % , $(notdir $(SCHED_HEADERS)))

# Fix up names based on machine definitions
ifdef GAME_ROM_PREFIX
GAME_ROM = $(GAME_ROM_PREFIX)$(MACHINE_MAJOR)_$(MACHINE_MINOR).rom
else
GAME_ROM = freewpc.rom
endif
MAP_FILE = $(GAME_ROM:.rom=.map)
NATIVE_PROG = $(BLDDIR)/freewpc_$(MACHINE)

ifndef MACHINE_FILE
MACHINE_FILE = $(MACHINE).md
endif

ifdef IMAGE_MAP
IMAGE_ROM = build/$(MACHINE)_images.rom
IMAGE_HEADER = build/imagemap.h
C_DEPS += $(IMAGE_HEADER)
endif

# Add files generated by the machine description
# Files should go into SYSTEM_MD_OBJS by default, unless
# code is written to handle the paging.
PAGED_MD_OBJS = \
	$(BLDDIR)/mach-strings.o \
	$(BLDDIR)/mach-lamplists.o

SYSTEM_MD_OBJS = \
	$(BLDDIR)/mach-switchmasks.o \
	$(BLDDIR)/mach-scores.o \
	$(BLDDIR)/mach-switches.o \
	$(BLDDIR)/mach-containers.o \
	$(BLDDIR)/mach-drives.o \
	$(BLDDIR)/mach-deffs.o

ifeq ($(CONFIG_FONT),y)
SYSTEM_MD_OBJS += $(BLDDIR)/mach-fonts.o
endif

MD_OBJS = $(PAGED_MD_OBJS) $(SYSTEM_MD_OBJS)

#######################################################################
###	Object File Distribution
#######################################################################

# Because WPC uses ROM paging, the linking job is more
# difficult to get right.  We require that the programmer
# explicitly state which pages things should belong in.

# A list of the paged sections that we will use.  Not all pages
# are currently needed.
#
# Alphanumeric games like Funhouse are restricted to 128KB total,
# so only 6 non-system pages.  DMD games are allowed to use more.
ifeq ($(CONFIG_DMD),y)
PAGE_NUMBERS += 52 53 54 55
endif
PAGE_NUMBERS += 56 57 58 59 60 61
ifeq ($(PLATFORM),wpcsound)
BLANK_SIZE := 304
else
PAGE_SIZE = 16
FIXED_PAGE_COUNT = 2
NUM_PAGED_SECTIONS := $(words $(PAGE_NUMBERS))
NUM_BLANK_PAGES := $(shell echo $$(($(ROM_PAGE_COUNT) - $(FIXED_PAGE_COUNT) - $(NUM_PAGED_SECTIONS))))
BLANK_SIZE := $(shell echo $$(( $(NUM_BLANK_PAGES) * $(PAGE_SIZE))))
endif
PAGED_SECTIONS = $(foreach pg,$(PAGE_NUMBERS),page$(pg))

#
# Memory Map
#
# AREA_SETUP (name, address, length):
# Define a new linker area.
#
# The first 16-bytes of the nonvolatile area are reserved.
# PinMAME has a hack that overwrites this area.
#
# The size of the local area given here is the per-player value.
#
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
ifneq ($(PLATFORM),wpcsound)
$(eval $(call AREA_SETUP, local,     0x1400,   0x0040))
$(eval $(call AREA_SETUP, permanent, 0x1600,   0x0080))
endif
$(eval $(call AREA_SETUP, stack,     0x1680,   0x0180,  virtual))
ifeq ($(PLATFORM),wpcsound)
$(eval $(call AREA_SETUP, paged,     0x4000,   0x8000,  virtual))
$(eval $(call AREA_SETUP, sysrom,    0xC000,   0x3FF0,  virtual))
else
$(eval $(call AREA_SETUP, nvram,     0x1810,   0x07F0))
$(eval $(call AREA_SETUP, paged,     0x4000,   0x4000,  virtual))
$(eval $(call AREA_SETUP, sysrom,    0x8000,   0x7FF0,  virtual))
endif
$(eval $(call AREA_SETUP, vector,    0xFFF0,   0x0010,  virtual))

SYSROM_SIZE := $(shell echo $$(($(AREASIZE_sysrom) + $(AREASIZE_vector))))

MACHINE_SYS_OBJS = $(patsubst %,$(MACHINE_DIR)/%,$(GAME_OBJS))
MACHINE_TEST_OBJS = $(patsubst %,$(MACHINE_DIR)/%,$(GAME_TEST_OBJS))
MACHINE_OBJS = $(patsubst %,$(MACHINE_DIR)/%,$(GAME_PAGED_OBJS))
MACHINE2_OBJS = $(patsubst %,$(MACHINE_DIR)/%,$(GAME2_OBJS))
MACHINE3_OBJS = $(patsubst %,$(MACHINE_DIR)/%,$(GAME3_OBJS))
MACHINE4_OBJS = $(patsubst %,$(MACHINE_DIR)/%,$(GAME4_OBJS))
MACHINE5_OBJS = $(patsubst %,$(MACHINE_DIR)/%,$(GAME5_OBJS))
SYSTEM_HEADER_OBJS =
SYSTEM_OBJS := $(SYSTEM_MD_OBJS) $(SYSTEM_HEADER_OBJS) $(KERNEL_ASM_OBJS) $(KERNEL_OBJS) $(MACHINE_SYS_OBJS) $(SCHED_OBJ)

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
page$(strip $1)_OBJS := $(BLDDIR)/page$(strip $1).o
endef

# PAGE_ALLOC : Allocate a paged region for a particular class of objects.
# $1 = the page number
# $2 = the object class
# $3 = the page define (derived if $2 if not given)
# Example : PAGE_ALLOC(56,EFFECT)
define PAGE_ALLOC
page$(strip $1)_OBJS += $($(strip $2)_OBJS)
page$(strip $1)_SECTIONS += $(strip $2)
$($(strip $2)_OBJS) : PAGE=$(strip $1)
ifneq ($(strip $3),)
CFLAGS += -D$(strip $3)_PAGE=$(strip $1)
else
CFLAGS += -D$(strip $2)_PAGE=$(strip $1)
endif
endef

$(foreach page,$(PAGE_NUMBERS),$(eval $(call PAGE_INIT, $(page))))
ifeq ($(CONFIG_DMD),y)
$(eval $(call PAGE_ALLOC, 52, MACHINE3))
$(eval $(call PAGE_ALLOC, 53, MACHINE2))
$(eval $(call PAGE_ALLOC, 54, MACHINE))
$(eval $(call PAGE_ALLOC, 55, MACHINE4))
$(eval $(call PAGE_ALLOC, 59, MACHINE5))
else
$(eval $(call PAGE_ALLOC, 55, MACHINE2))
$(eval $(call PAGE_ALLOC, 59, MACHINE))
endif
$(eval $(call PAGE_ALLOC, 55, TRANS))
$(eval $(call PAGE_ALLOC, 56, COMMON))
$(eval $(call PAGE_ALLOC, 57, EFFECT))
$(eval $(call PAGE_ALLOC, 57, DEFF))
$(eval $(call PAGE_ALLOC, 57, LEFF))
$(eval $(call PAGE_ALLOC, 57, INIT))
$(eval $(call PAGE_ALLOC, 58, TEST))
$(eval $(call PAGE_ALLOC, 58, MACHINE_TEST))
$(eval $(call PAGE_ALLOC, 59, EVENT))
$(eval $(call PAGE_ALLOC, 60, PAGED_MD, MD))
$(eval $(call PAGE_ALLOC, 60, TEST2))
$(eval $(call PAGE_ALLOC, 61, FONT))
$(eval $(call PAGE_ALLOC, 61, FON))

$(SYSTEM_OBJS) : PAGE=62
CFLAGS += -DSYS_PAGE=62 -DSYSTEM_PAGE=62

PAGED_OBJS = $(foreach area,$(PAGED_SECTIONS),$($(area)_OBJS))

PAGE_HEADER_OBJS = $(foreach area,$(PAGED_SECTIONS),$(BLDDIR)/$(area).o)

AS_OBJS := $(SYSTEM_HEADER_OBJS) $(KERNEL_ASM_OBJS)

C_OBJS := $(MD_OBJS) $(KERNEL_OBJS) $(COMMON_OBJS) $(EVENT_OBJS) \
	$(TRANS_OBJS) $(TEST_OBJS) $(TEST2_OBJS) \
	$(MACHINE_SYS_OBJS) $(MACHINE_OBJS) $(MACHINE_TEST_OBJS) \
	$(MACHINE2_OBJS) $(MACHINE3_OBJS) $(MACHINE4_OBJS) $(MACHINE5_OBJS) \
	$(FONT_OBJS) $(EFFECT_OBJS) \
	$(INIT_OBJS) $(DEFF_OBJS) $(LEFF_OBJS) $(SCHED_OBJ)

ifeq ($(PLATFORM),wpc)
OBJS = $(C_OBJS) $(AS_OBJS) $(FON_OBJS)
else
ifeq ($(PLATFORM),whitestar)
OBJS = $(C_OBJS) $(AS_OBJS)
else
OBJS = $(C_OBJS) $(FON_OBJS)
endif
endif

MACH_LINKS = .mach .include_mach

MAKE_DEPS = Makefile kernel/Makefile common/Makefile $(MMAKEFILE) $(BLDDIR)/mach-Makefile $(DOTCONFIG)
ifeq ($(CONFIG_FONT),y)
MAKE_DEPS += fonts/Makefile
endif
C_DEPS += $(BLDDIR)/mach-config.h
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
	$(Q)rm -f $(ERR)

.PHONY : check_prereqs
check_prereqs : $(BLDDIR) tools sched

.PHONY : run
run:
	# Start pinmame up and let it run indefinitely.
	$(PINMAME) $(PINMAME_MACHINE) $(PINMAME_FLAGS) &

.PHONY : debug
debug:
	# Start pinmame up and let it run indefinitely.
	$(PINMAME) $(PINMAME_MACHINE) -d $(PINMAME_FLAGS) &

.PHONY : rund
rund: run $(DBCON)
	xterm -e "$(DBCON) -1" &

.PHONY : rund-file
rund-file: run $(DBCON)
	xterm -e "$(DBCON) -1 > $(DBCON_FILE) 2>&1" &

.PHONY : endrun
endrun :
	kill $(shell pidof $(PINMAME))

.PHONY : timedrun
timedrun:
	$(MAKE) rund-file DBCON_FILE="test.log"
	sleep 30
	$(MAKE) endrun

.PHONY : runcon
runcon: $(BLDDIR)/$(GAME_ROM)
	wpc-run $(BLDDIR)/$(GAME_ROM)

.PHONY : install
install : $(TARGET_ROMPATH)/$(PINMAME_MACHINE).zip

$(TARGET_ROMPATH)/$(PINMAME_MACHINE).zip : $(TARGET_ROMPATH)/$(PINMAME_GAME_ROM)
	$(Q)echo Installing to MAME directory '$(TARGET_ROMPATH)' ...; \
	cd $(TARGET_ROMPATH); \
	if [ ! -f $(PINMAME_MACHINE).zip.original ]; then \
		echo "Saving original MAME roms..."; \
		mv $(PINMAME_MACHINE).zip $(PINMAME_MACHINE).zip.original; \
	fi; \
	rm -f $(PINMAME_MACHINE).zip; \
	zip $(PINMAME_MACHINE).zip $(PINMAME_GAME_ROM) $(PINMAME_OTHER_ROMS)

.PHONY : uninstall
uninstall :
	$(Q)cd $(TARGET_ROMPATH) && \
	if [ -f $(PINMAME_MACHINE).zip.original ]; then \
		if [ -f $(PINMAME_MACHINE).zip ]; then \
			echo "Restoring original $(MACHINE) ROM in $(TARGET_ROMPATH)..."; \
			rm -f $(PINMAME_MACHINE).zip && \
			mv $(PINMAME_MACHINE).zip.original $(PINMAME_MACHINE).zip; \
			unzip $(PINMAME_MACHINE).zip; \
		fi; \
	fi

#
# PinMAME will want the ROM file to be named differently...
#
$(TARGET_ROMPATH)/$(PINMAME_GAME_ROM) : $(BLDDIR)/$(GAME_ROM)
	cp $(BLDDIR)/$(GAME_ROM) $(TARGET_ROMPATH)/$(PINMAME_GAME_ROM)

#
# Use 'make compile' to compile the ROM without installing it.
#
compile: $(BLDDIR)/$(GAME_ROM)

$(BLDDIR):
	$(Q)echo "Making build directory..." && mkdir -p $(BLDDIR)

post_compile :
	$(Q)echo "Cleaning .i files..." && rm -f *.i

#
# How to make a ROM image, which is the concatenation of each of the
# paged binaries, the system binary, and padding to fill out the length
# to that expected for the particular machine.
#
ifndef REMOTE_PATH
$(BLDDIR)/$(GAME_ROM) : $(BLDDIR)/blank$(BLANK_SIZE).bin $(BINFILES) $(IMAGE_ROM) $(CSUM)
	$(Q)echo Padding ... && \
		cat $(BLDDIR)/blank$(BLANK_SIZE).bin $(PAGED_BINFILES) $(SYSTEM_BINFILE) > $@ && \
		echo "Updating ROM checksum ..." && $(CSUM) -f $@ -v 0x$(SYSTEM_MINOR) -u
ifdef IMAGE_MAP
	$(Q)echo "Importing image ROM ..." && dd if=$(IMAGE_ROM) of=$(BLDDIR)/$(GAME_ROM) conv=notrunc
endif
else
download: prep-download $(BLDDIR)/$(GAME_ROM)

prep-download:
	rm -f $(BLDDIR)/$(GAME_ROM)

$(BLDDIR)/$(GAME_ROM):
	scp $(REMOTE_PATH)/$(BLDDIR)/$(GAME_ROM) $(BLDDIR)/$(GAME_ROM)
endif

#
# How to make a blank file.  This creates an empty file of any desired size
# in multiples of 1KB.
#
$(BLDDIR)/blank%.bin: $(BLDDIR)/blankpage.bin
	$(Q)echo "Creating $*KB blank file ..." && $(BLANKER) if=$(BLDDIR)/blankpage.bin of=$@ bs=1k count=$* > /dev/null 2>&1

$(BLDDIR)/blankpage.bin: $(SR)
	$(Q)echo "Creating blank 32KB page ..." && $(SR) -o $@.1 -l 0x8000 -f 0xFF -B && \
		(for ((a=0; a < 32; a++)); do cat $@.1; done ) > $@ && \
		rm -f $@.1


#
# Convert each S-record file, one per page, into binary.
#
$(SYSTEM_BINFILE) : %.bin : %.s19 $(SR) $(BLDDIR)/$(MAP_FILE)
	$(Q)echo "Checking for overflow..." && tools/mapcheck $(BLDDIR)/$(MAP_FILE) >> $(ERR) 2>&1
	$(Q)$(SR) -o $@ -s $(AREA_sysrom) -l $(SYSROM_SIZE) -f 0xFF $<

$(PAGED_BINFILES) : %.bin : %.s19 $(SR)
	$(Q)$(SR) -o $@ -s $(AREA_paged) -l $(AREASIZE_paged) -f 0xFF $<


#
# The map file generated by the linker needs to be renamed from
# freewpc.map to something with the version number in it.
#
$(BLDDIR)/$(MAP_FILE) : build/freewpc.s19
	$(Q)echo "Renaming map file..." && mv $(BLDDIR)/freewpc.map $(BLDDIR)/$(MAP_FILE)


#
# General rule for linking a group of object files.  The linker produces
# a Motorola S-record file by default (S19).
#
ifeq ($(CPU),m6809)
$(BINFILES:.bin=.s19) : %.s19 : %.lnk $(OBJS) $(AS_OBJS) $(PAGE_HEADER_OBJS)
	$(Q)echo "Linking $@..." && $(CC) -Wl,-T -Wl,$< >> $(ERR) 2>&1
endif

ifeq ($(CPU),native)
native : $(NATIVE_PROG)
$(NATIVE_PROG) : $(IMAGE_ROM) $(OBJS) $(NATIVE_OBJS)
	$(Q)echo "Linking $@ ..." && $(HOSTCC) $(HOST_LFLAGS) `pth-config --ldflags` -o $(NATIVE_PROG) $(OBJS) $(NATIVE_OBJS) $(HOST_LIBS) >> $(ERR) 2>&1
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
# named here is the same for every page, otherwise the addresses of
# references won't match up with their actual definitions.
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
OBJ_PAGE_LIST = $(foreach obj,$(filter-out $(1:.lnk=.o),$(SYSTEM_OBJS) $(PAGED_OBJS)),$(call OBJ_PAGE_LINKOPT,$(obj),$(patsubst $(BLDDIR)/%,%,$1)))
DUP_PAGE_OBJ = $1

$(PAGED_LINKCMD) : $(MAKE_DEPS) $(PMAKEFILE)
	$(Q)rm -f $@ ;\
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
	echo "-o" >> $(LINKCMD) ;\
	echo "-e" >> $@


$(BLDDIR)/freewpc.s:
	$(Q)echo ".area .text" >> $@

#
# How to build a page header source file.
#
$(BLDDIR)/page%.s:
	$(Q)echo ".area page$*" >> $@ && echo ".db $*" >> $@

#
# How to make the linker command file for the system section.
#
$(LINKCMD) : $(MAKE_DEPS) $(PMAKEFILE)
	$(Q)rm -f $(LINKCMD) ;\
	echo "-mxswz" >> $(LINKCMD) ;\
	( $(foreach area,$(AREA_LIST),echo -b $(area) = $(AREA_$(area));) ) >> $(LINKCMD) ;\
	for f in `echo $(PAGED_SECTIONS)`; \
		do echo "-b $$f = $(AREA_paged)" >> $(LINKCMD); done ;\
	echo "-b .text = $(AREA_sysrom)" >> $(LINKCMD) ;\
	echo "$(BLDDIR)/freewpc.o" >> $(LINKCMD) ;\
	for f in `echo $(SYSTEM_OBJS)`; do echo $$f >> $(LINKCMD); done ;\
	echo "-v" >> $(LINKCMD) ;\
	for f in `echo $(PAGED_OBJS)`; do echo $$f >> $(LINKCMD); done ;\
	echo "-o" >> $(LINKCMD) ;\
	echo "-e" >> $(LINKCMD)

#
# General rule for how to build any assembler file.  This uses GCC
# as a front end to the actual assembler, so the preprocessor is
# available.
#
$(AS_OBJS) : %.o : %.s $(CC) $(MAKE_DEPS)
	$(Q)echo Assembling $< ... && $(AS) $(EXTRA_ASFLAGS) -o $@ -c $< >> $(ERR) 2>&1

#
# General rule for how to build a page header, which is a special
# version of an assembly file.
#
$(PAGE_HEADER_OBJS) : $(BLDDIR)/page%.o : $(BLDDIR)/page%.s $(CC)
	$(Q)echo Assembling page header $< ... && $(AS) -o $@ -c $< >> $(ERR) 2>&1

#
# General rule for how to compile any C code for the target.
# This includes ordinary .c files but also other file types that
# actually contain C code.
#
# Many options are passed to gcc, and these differ depending on the type
# of file being compiled:
#
#    PAGEFLAGS is only used by font files, and may be removed later...
#
#    SOFTREG_CFLAGS says how many soft registers should be used, if any.
#    It is unsafe to use soft registers in any file which declares
#    interrupt-level functions, because GCC does not save/restore them
#    as part of interrupt prologue/epilogue.  Such files will not have
#    these options used.  TODO: what would it take to get
#    KERNEL_HW_OBJS to benefit from soft regs?
#
#    PAGE is a macro set to the current page setting, so the code
#    knows what page it is being compiled in.  (-mfar-code-page tells
#    only GCC; this tells the code itself.  Ideally, GCC would define
#    something for us (another TODO))
#
# Also TODO - EFFECT_OBJS is not a candidate for soft registers because GCC
# is crashing on sound_effect.c

ifeq ($(CPU),m6809)
$(FON_OBJS): PAGEFLAGS="-Dstatic=__attribute__((section(\"page$(PAGE)\")))"
$(BASIC_OBJS) $(KERNEL_SW_OBJS) $(COMMON_OBJS) $(INIT_OBJS) $(TEST2_OBJS) $(TRANS_OBJS): SOFTREG_CFLAGS=$(SOFTREG_OPTIONS)
endif

$(NATIVE_OBJS) $(C_OBJS) : %.o : %.c

$(FON_OBJS) : %.o : %.fon

$(filter-out $(BASIC_OBJS),$(C_OBJS)) : $(C_DEPS) $(GENDEFINES) $(REQUIRED)

$(C_OBJS) $(FON_OBJS) : $(IMAGE_HEADER)

$(NATIVE_OBJS) : $(GENDEFINES) $(REQUIRED)

$(BASIC_OBJS) $(FON_OBJS) : $(MAKE_DEPS) $(GENDEFINES) $(REQUIRED)

$(KERNEL_OBJS) : kernel/Makefile
$(COMMON_OBJS) : common/Makefile

$(filter-out $(HOST_OBJS),$(NATIVE_OBJS)) $(C_OBJS) $(FON_OBJS):
ifeq ($(CPU),m6809)
	$(Q)echo "Compiling $< (in page $(PAGE)) ..." && $(CC) -x c -o $@ $(CFLAGS) -c $(PAGEFLAGS) -DPAGE=$(PAGE) -mfar-code-page=$(PAGE) $(SOFTREG_CFLAGS) $< >> $(ERR) 2>&1
else
	$(Q)echo "Compiling $< ..." && $(HOSTCC) -x c -o $@ $(CFLAGS) -c $(PAGEFLAGS) $< >> $(ERR) 2>&1
endif
ifeq ($(CONFIG_PROFILING),y)
	$(Q)mkdir -p gprof.data
	$(shell mv gmon.out gprof.data/gmon.$$RANDOM.out)
endif

#######################################################################
###	Machine Description Compiler
#######################################################################
CONFIG_CMDS = strings switchmasks containers switches scores lamplists deffs drives
ifeq ($(CONFIG_FONT),y)
CONFIG_CMDS += fonts
endif
CONFIG_SRCS = $(CONFIG_CMDS:%=$(BLDDIR)/mach-%.c)
CONFIG_FILES = $(BLDDIR)/mach-config.h $(CONFIG_SRCS) $(BLDDIR)/mach-Makefile

.PHONY : config
config : $(CONFIG_FILES)

$(BLDDIR)/mach-Makefile : $(MACH_DESC) $(BLDDIR)
	$(Q)echo "Regenerating $@ if necessary..." && \
	tools/genmachine $< makefile > $@.tmp && tools/move-if-change $@.tmp $@

$(BLDDIR)/mach-config.h : $(MACH_DESC)
	$(Q)echo "Regenerating $@ if necessary..." && \
	tools/genmachine $< config > $@.tmp && tools/move-if-change $@.tmp $@

$(CONFIG_SRCS) : $(BLDDIR)/mach-%.c : $(MACH_DESC) $(BLDDIR)/mach-config.h
	$(Q)echo "Regenerating $@ if necessary..." && \
	tools/genmachine $(MACH_DESC) $(@:$(BLDDIR)/mach-%.c=%) > $@.tmp && \
	tools/move-if-change $@.tmp $@

$(CONFIG_FILES) : tools/genmachine $(PLATFORM_DESC)

#######################################################################
###	Image Linking
#######################################################################

ifdef IMAGE_MAP
IMAGE_AREA_SIZE ?= $(BLANK_SIZE)
$(IMAGE_ROM) $(IMAGE_HEADER): $(IMAGE_MAP) $(IMGLD)
	$(IMGLD) -o $(IMAGE_ROM) -i $(IMAGE_HEADER) -p $(FIRST_BANK) \
		-s $(IMAGE_AREA_SIZE) $(IMAGE_MAP) && sleep 0.5
else
$(IMAGE_HEADER):
	touch $(IMAGE_HEADER)
endif

#######################################################################
###	Header File Targets
#######################################################################

#
# How to automake files of #defines
#
gendefines: $(GENDEFINES)

include/gendefine_gid.h: $(MACH_LINKS) $(CONFIG_SRCS) $(TEMPLATE_SRCS)
	$(Q)echo Autogenerating task IDs... && \
		$(GENDEFINE) -c NUM_GIDS -p GID_ > $@

.PHONY : clean_gendefines
clean_gendefines:
	$(Q)echo Deleting autogenerated files... && rm -f $(GENDEFINES)

.PHONY : gendefines_again
gendefines_again: clean_gendefines gendefines

#
# How to automake callsets
#
.PHONY : callset
callset: $(BLDDIR)/callset.o

CALLSET_SECTIONS := MACHINE MACHINE2 MACHINE3 MACHINE4 MACHINE5 COMMON EFFECT INIT TEST TEST2 SYSTEM
$(BLDDIR)/callset.c : $(MACH_LINKS) $(CONFIG_SRCS) $(TEMPLATE_SRCS) tools/gencallset
	$(Q)echo "Generating callsets ... " && rm -f $@ \
		&& tools/gencallset \
			$(foreach section,$(CALLSET_SECTIONS),$($(section)_OBJS:.o=.c:$(section)_PAGE)) \
			$(NATIVE_OBJS:.o=.c)

.PHONY : callset_again
callset_again:
	rm -rf $(BLDDIR)/callset.c && $(MAKE) callset

.PHONY : fonts clean-fonts
fonts clean-fonts:
	$(Q)echo "Making $@... " && $(MAKE) -f Makefile.fonts $@

#######################################################################
###	Static Scheduling
#######################################################################

.PHONY : sched
sched: $(SCHED_SRC) tools/sched/sched.make

$(SCHED_SRC): $(SYSTEM_SCHEDULE) $(MACHINE_SCHEDULE) $(SCHED) $(SCHED_HEADERS) $(MAKE_DEPS)
	$(SCHED) -o $@ $(SCHED_FLAGS) $(SYSTEM_SCHEDULE) $(MACHINE_SCHEDULE) $(MACHINE_SCHED_FLAGS)

#######################################################################
###	Tracing
#######################################################################

trace:
	$(MAKE) debug
	echo ""
	echo "1. Close the debugger, by typing 'G'."
	echo "2. After the system has nearly completed init, type a tilde to break."
	echo "3. Enter 'IGNORE 1'."
	echo "4. Enter 'TRACE 1 A B X Y U S'."
	echo "5. Type 'G' to go again."
	echo "6. Break with tilde when you are done tracing."
	echo "7. Type 'TRACE OFF' to end tracing and close the trace file."
	echo "8. Stop PinMAME by closing the window."
	echo ""
	echo "Press ENTER when you have done all of this."
	echo ""
	read akey

#######################################################################
###	Host Tools
#######################################################################

tools : $(TOOLS)

$(TOOLS) $(HOST_OBJS) : CC=$(HOSTCC)

$(HOST_OBJS) : CFLAGS=-Wall -I. -g

$(sort $(HOST_OBJS)) : %.o : %.c
	$(CC) $(CFLAGS) $(TOOL_CFLAGS) -o $@ -c $< >> $(ERR) 2>&1

#######################################################################
###	Standard Dependencies
#######################################################################

# Provide a target for .config that will run 'configure' or abort if it
# does not exist.
ifneq ($(DOTCONFIG),)
$(DOTCONFIG):
	echo "No $(DOTCONFIG)" && exit 1
endif

#
# Symbolic links to the machine code.  Once set, code can reference
# 'mach' and 'include/mach' without knowing the specific machine type.
#
.mach:
	$(Q)echo "Setting symbolic link for machine source code..." && \
		touch .mach && ln -s $(MACHINE_DIR) mach

.include_mach:
	$(Q)echo "Setting symbolic link for machine include files..." && \
		touch .include_mach && cd include && ln -s $(MACHINE) mach

#
# Remake machine prototypes file.  This never happens by default.
#
protos : include/$(MACHINE)/protos.h

include/$(MACHINE)/protos.h :
	cproto -o $@ -I include -I include/sys $(MACHINE)/*.c

#
# Build online tarball release
#
tarball:
ifdef SYSTEM_MAJOR
ifdef SYSTEM_MINOR
	git ls-files -x '*.zip' | grep -v "^web" |\
		xargs tar zcvf release-$(SYSTEM_MAJOR).$(SYSTEM_MINOR).tar.gz
endif
endif

#
# Documentation (doxygen)
#
.PHONY : doxygen
doxygen: doc/doxygen/Doxyfile
	doxygen $<

tex:
	cd doc && makeinfo --no-split --no-headers --html -o freewpc.html freewpc.texi
	cd doc && makeinfo --html freewpc.texi
	cp -p doc/freewpc.html $(WEBROOT)/manual.html
	cp -a doc/freewpc/* $(WEBROOT)/manual

#
# For debugging the makefile settings
#
.PHONY : info
info:
	$(Q)echo "MACHINE : $(MACHINE)"
	$(Q)echo "PLATFORM : $(PLATFORM)"
	$(Q)echo "CPU = $(CPU)"
	$(Q)echo "GAME_ROM : $(BLDDIR)/$(GAME_ROM)"
	$(Q)echo "CC = $(CC)"
	$(Q)echo "GCC_VERSION = $(GCC_VERSION)"
	$(Q)echo "CFLAGS = $(CFLAGS)"
	$(Q)echo "TOOLS = $(TOOLS)"
	$(Q)echo "REQUIRED = $(REQUIRED)"
	$(Q)echo "PATH_REQUIRED = $(PATH_REQUIRED)"
	$(Q)echo "NUM_BLANK_PAGES = $(NUM_BLANK_PAGES)"
	$(Q)echo "MACH_DESC = $(MACH_DESC)"
	$(Q)echo "HOST_OBJS = $(HOST_OBJS)"
	$(Q)echo "SCHED_FLAGS = $(SCHED_FLAGS)"
	$(Q)echo "NATIVE_PROG = $(NATIVE_PROG)"
	$(Q)echo "NATIVE_OBJS = $(NATIVE_OBJS)"
	$(Q)echo "C_OBJS = $(C_OBJS)"

.PHONY : areainfo
areainfo:
	@true $(foreach area,$(AREA_LIST),&& echo $(area) $(AREASIZE_$(area)))
	@true $(foreach page,$(PAGED_SECTIONS),&& echo $(page) 0x4000 $($(page)_SECTIONS))

callset.in :
	cat $(C_OBJS:.o=.c) | $(CC) -E $(CFLAGS) -DGENCALLSET - > callset.in


#
# 'make clean' does what you think.
#
.PHONY : clean
clean: clean_derived clean_build clean_gendefines clean_tools
	$(Q)for dir in `echo . kernel common effect fonts images test $(MACHINE_DIR) $(PLATFORM_DIR) sim cpu/$(CPU)`;\
		do echo "Cleaning in '$$dir' ..." && \
		pushd $$dir >/dev/null && rm -f $(TMPFILES) && \
		popd >/dev/null ; done

.PHONY : clean_derived
clean_derived:
	$(Q)for file in `echo mach include/mach` ;\
		do echo "Removing derived file $$file..." && \
		rm -f $$file; done && \
		rm -rf .mach .include_mach && \
		rm -f *.s *.i

.PHONY : clean_build
clean_build:
	$(Q)rm -f $(BLDDIR)/* && if [ -d $(BLDDIR) ]; then rmdir $(BLDDIR); fi

.PHONY : clean_tools
clean_tools:
	$(Q)rm -f $(HOST_OBJS) $(TOOLS)

.PHONY : show_objs
show_objs:
	$(Q)echo $(OBJS)

