#
# FreeWPC makefile
#
# (C) Copyright 2005-2008 by Brian Dominy.
#
# This Makefile can be used to build an entire, FreeWPC game ROM
# from source code.
#
# To build the product, just type "make".
#
# To customize the build, create a file named .config.
# See .config.example for an example of how this should look.
# The settings in .config override any defaults given below.
#
# By default, make will also install your game ROM into your pinmame
# ROMs directory.  The original MAME zip file will be saved into
# a file with the .original extension appended.  You can do a
# "make uninstall" to delete the FreeWPC zip and rename the original
# zip, so that things are back to the way that they started.
#


# Include the .config file, which must exist
include .config

define require
$(if $($1),,$(error $1 is not defined : $($1)))
endef

define md_config
$(if $(shell grep ^$1:.*Yes $(M)/$(MACHINE_FILE)),y,)
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

#ifneq ($(PLATFORM),wpc-shell)

# MACHINE says which machine you are targetting.  It must be defined.
# This loads in per-machine rules.
$(eval $(call require,MACHINE))

M := machine/$(MACHINE)
MMAKEFILE := $(M)/Makefile
MACH_DESC = $(MACHINE_DIR)/$(MACHINE_FILE)
MACHINE_DIR = machine/$(MACHINE)

top_target : default_target

include $(BLDDIR)/mach-Makefile
include $(MMAKEFILE)

# MACHINE_FILE must be set by the machine Makefile.  We can
# grep it to set additional configuration variables.
$(eval $(call require,MACHINE_FILE))
PLATFORM ?= wpc
CONFIG_DMD := $(call md_config,DMD)
# CONFIG_DMD := $(if $(shell grep ^DMD:.*Yes $(M)/$(MACHINE_FILE)),y,)
CONFIG_PIC := $(if $(shell grep ^PIC:.*Yes $(M)/$(MACHINE_FILE)),y,)
CONFIG_FLIPTRONIC := $(if $(shell grep ^Fliptronic:.*Yes $(M)/$(MACHINE_FILE)),y,)
CONFIG_DCS := $(if $(shell grep ^DCS:.*Yes $(M)/$(MACHINE_FILE)),y,)
CONFIG_WPC95 := $(if $(shell grep ^WPC95:.*Yes $(M)/$(MACHINE_FILE)),y,)

#else
#MACH_DESC = platform/wpc-shell/wpc-shell.md
#endif

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

ifdef NATIVE
include platform/native/Makefile
else
PMAKEFILE := platform/$(PLATFORM)/Makefile
-include $(PMAKEFILE)
endif

# Set this to the name of the CPU.  In simulation this is always
# 'native'; otherwise it is already set by the platform.
ifdef NATIVE
CPU := native
endif

# Build date (now)
BUILD_MONTH := $(shell date +%-m)
BUILD_DAY := $(shell date +%-d)
BUILD_YEAR := $(shell date +%Y)


.PHONY : platform_target
ifdef NATIVE
platform_target : freewpc
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
TMPFILES += $(ERR)

#######################################################################
###	Programs
#######################################################################

# Path to the compiler and linker
# Define GCC4 if GCC_VERSION begins with 4.
ifeq ($(CPU),m6809)
GCC_ROOT = /usr/local/bin
ifneq ($(shell echo $(GCC_VERSION) | grep ^4),)
EXTRA_CFLAGS += -DGCC4
CC := $(CCACHE) $(GCC_ROOT)/m6809-unknown-none-gcc-$(GCC_VERSION)
else
CC := $(CCACHE) $(GCC_ROOT)/m6809-gcc-$(GCC_VERSION)
endif
AS = $(CC) -xassembler-with-cpp
LD = $(GCC_ROOT)/m6809-ld
REQUIRED += $(CC:$(CCACHE)=) $(LD)
else
GCC_VERSION = NATIVE
endif

HOSTCC := $(CCACHE) gcc

TOOLS :=
HOST_OBJS :=

define include-tool
D := tools/$1
include tools/$1/$1.make
endef

$(eval $(call include-tool,srec2bin))
$(eval $(call include-tool,csum))
$(eval $(call include-tool,wpcdebug))
$(eval $(call include-tool,sched))
$(eval $(call include-tool,fiftool))
$(eval $(call include-tool,softscope))
$(eval $(call include-tool,scope))
$(eval $(call include-tool,bin2c))


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

ifneq ($(CONFIG_BARE),y)
include kernel/Makefile
include common/Makefile
ifeq ($(CONFIG_FONT),y)
include fonts/Makefile
endif
ifdef CONFIG_WPC
include test/Makefile
endif

EVENT_OBJS = $(BLDDIR)/callset.o

ifeq ($(CONFIG_DMD),y)
FIF_SRCS := images/freewpc_logo.fif images/tuxlogo.fif $(FIF_SRCS)

FIF_SRCS += $(patsubst %.pgm,%.fif,$(PGM_SRCS))

FIF_OBJS = $(patsubst %.fif,%.o,$(FIF_SRCS))
endif

BASIC_OBJS = $(KERNEL_BASIC_OBJS) $(COMMON_BASIC_OBJS) $(FONT_OBJS) $(TRANS_OBJS)

OS_INCLUDES = include/freewpc.h include/platform/$(PLATFORM).h

INCLUDES = $(OS_INCLUDES) $(GAME_INCLUDES)

FON_SRCS = $(patsubst %.o,%.fon,$(FON_OBJS))
export FON_SRCS

TEMPLATE_SRCS = $(patsubst %.o,%.c,$(TEMPLATE_OBJS))
endif

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
ifeq ($(FREEWPC_DEBUGGER),y)
CFLAGS += -DDEBUGGER
EXTRA_ASFLAGS += -DDEBUGGER
endif

CFLAGS += $(EXTRA_CFLAGS)

SCHED_HEADERS := include/freewpc.h include/interrupt.h $(SCHED_HEADERS)
SCHED_FLAGS += $(patsubst %,-i % , $(notdir $(SCHED_HEADERS))) $(MACHINE_SCHED_FLAGS)


# Fix up names based on machine definitions
ifdef GAME_ROM_PREFIX
GAME_ROM = $(GAME_ROM_PREFIX)$(MACHINE_MAJOR)_$(MACHINE_MINOR).rom
else
GAME_ROM = freewpc.rom
endif
MAP_FILE = $(GAME_ROM:.rom=.map)

ifndef MACHINE_FILE
MACHINE_FILE = $(MACHINE).md
endif

ifdef GAME_FSMS
FSM_SRCS = $(GAME_FSMS:%.fsm=$(BLDDIR)/%.c)
FSM_OBJS = $(GAME_FSMS:%.fsm=$(BLDDIR)/%.o)
endif

# Add files generated by the machine description
# Files should go into SYSTEM_MD_OBJS by default, unless
# code is written to handle the paging.
ifneq ($(CONFIG_BARE),y)
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
endif

#######################################################################
###	Object File Distribution
#######################################################################

# Because WPC uses ROM paging, the linking job is more
# difficult to get right.  We require that the programmer
# explicitly state which pages things should belong in.

# A list of the paged sections that we will use.  Not all pages
# are currently needed.
PAGE_NUMBERS = 52 53 54 55 56 57 58 59 60 61
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

MACHINE_OBJS = $(patsubst %,$(MACHINE_DIR)/%,$(GAME_OBJS))
MACHINE_TEST_OBJS = $(patsubst %,$(MACHINE_DIR)/%,$(GAME_TEST_OBJS))
MACHINE_PAGED_OBJS = $(patsubst %,$(MACHINE_DIR)/%,$(GAME_PAGED_OBJS))
SYSTEM_HEADER_OBJS =
SYSTEM_OBJS := $(SYSTEM_MD_OBJS) $(SYSTEM_HEADER_OBJS) $(KERNEL_ASM_OBJS) $(KERNEL_OBJS) $(MACHINE_OBJS) $(SCHED_OBJ)

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
$(eval $(call PAGE_ALLOC, 55, TRANS))
$(eval $(call PAGE_ALLOC, 55, FIF))
$(eval $(call PAGE_ALLOC, 56, COMMON))
$(eval $(call PAGE_ALLOC, 57, EVENT))
$(eval $(call PAGE_ALLOC, 58, TEST))
$(eval $(call PAGE_ALLOC, 58, MACHINE_TEST))
$(eval $(call PAGE_ALLOC, 59, MACHINE_PAGED, MACHINE))
$(eval $(call PAGE_ALLOC, 59, FSM))
$(eval $(call PAGE_ALLOC, 60, PAGED_MD, MD))
$(eval $(call PAGE_ALLOC, 60, EFFECT))
$(eval $(call PAGE_ALLOC, 60, TEST2))
$(eval $(call PAGE_ALLOC, 61, FONT))
$(eval $(call PAGE_ALLOC, 61, FON))

$(SYSTEM_OBJS) : PAGE=62
CFLAGS += -DSYS_PAGE=62

PAGED_OBJS = $(foreach area,$(PAGED_SECTIONS),$($(area)_OBJS))

PAGE_HEADER_OBJS = $(foreach area,$(PAGED_SECTIONS),$(BLDDIR)/$(area).o)

AS_OBJS := $(SYSTEM_HEADER_OBJS) $(KERNEL_ASM_OBJS)

C_OBJS := $(MD_OBJS) $(KERNEL_OBJS) $(COMMON_OBJS) $(EVENT_OBJS) \
	$(TRANS_OBJS) $(TEST_OBJS) $(TEST2_OBJS) $(FSM_OBJS) \
	$(MACHINE_OBJS) $(MACHINE_PAGED_OBJS) $(MACHINE_TEST_OBJS) \
	$(FONT_OBJS) $(EFFECT_OBJS) $(SCHED_OBJ)


ifeq ($(PLATFORM),wpc)
OBJS = $(C_OBJS) $(AS_OBJS) $(FIF_OBJS) $(FON_OBJS)
else
ifeq ($(PLATFORM),whitestar)
OBJS = $(C_OBJS) $(AS_OBJS)
else
OBJS = $(C_OBJS) $(FIF_OBJS) $(FON_OBJS)
endif
endif

MACH_LINKS = .mach .include_mach

MAKE_DEPS = Makefile kernel/Makefile common/Makefile $(MMAKEFILE) $(BLDDIR)/mach-Makefile .config
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
run: install
	# Start pinmame up and let it run indefinitely.
	$(PINMAME) $(PINMAME_MACHINE) $(PINMAME_FLAGS) &

.PHONY : debug
debug: install
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
	m6809-run -s wpc -I 1952 -b -C -d -m -1 $(BLDDIR)/$(GAME_ROM)

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
	cp -p $(BLDDIR)/$(GAME_ROM) $(TARGET_ROMPATH)/$(PINMAME_GAME_ROM)

#
# Use 'make compile' to compile the ROM without installing it.
#
compile: $(BLDDIR)/$(GAME_ROM)

.PHONY : $(BLDDIR)
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
$(BLDDIR)/$(GAME_ROM) : $(BLDDIR)/blank$(BLANK_SIZE).bin $(BINFILES) $(CSUM)
	$(Q)echo Padding ... && \
		cat $(BLDDIR)/blank$(BLANK_SIZE).bin $(PAGED_BINFILES) $(SYSTEM_BINFILE) > $@
	$(Q)echo "Updating ROM checksum ..." && $(CSUM) -f $@ -v 0x$(SYSTEM_MINOR) -u
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
	$(Q)echo "Creating blank 32KB page ..." && $(SR) -o $@.1 -l 0x8000 -f 0xFF -B
	$(Q)(for ((a=0; a < 32; a++)); do cat $@.1; done ) > $@
	$(Q)rm -f $@.1


#
# Convert each S-record file, one per page, into binary.
#
$(SYSTEM_BINFILE) : %.bin : %.s19 $(SR) $(BLDDIR)/$(MAP_FILE)
	$(Q)echo "Checking for overflow..." && tools/mapcheck $(BLDDIR)/$(MAP_FILE)
	$(Q)echo "Converting $< to binary ..." && $(SR) -o $@ -s $(AREA_sysrom) -l $(SYSROM_SIZE) -f 0xFF $<

$(PAGED_BINFILES) : %.bin : %.s19 $(SR)
	$(Q)echo "Converting $< to binary ..." && $(SR) -o $@ -s $(AREA_paged) -l $(AREASIZE_paged) -f 0xFF $<


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
freewpc : $(OBJS)
	$(Q)echo "Linking ..." && $(HOSTCC) $(HOST_LFLAGS) `pth-config --ldflags` -o freewpc $(OBJS) $(HOST_LIBS) >> $(ERR) 2>&1
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
OBJ_PAGE_LIST = $(foreach obj,$(filter-out $(1:.lnk=.o),$(SYSTEM_OBJS) $(PAGED_OBJS)),$(call OBJ_PAGE_LINKOPT,$(obj),$(patsubst $(BLDDIR)/%,%,$1)))
DUP_PAGE_OBJ = $1

$(PAGED_LINKCMD) : $(MAKE_DEPS) $(PMAKEFILE)
	$(Q)echo Creating linker command file $@ ... ;\
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
	echo "-o" >> $(LINKCMD) ;\
	echo "-e" >> $@


$(BLDDIR)/freewpc.s:
	$(Q)echo ".area .text" >> $@

#
# How to build a page header source file.
#
$(BLDDIR)/page%.s:
	$(Q)echo ".area page$*" >> $@
	$(Q)echo ".db $*" >> $@

#
# How to make the linker command file for the system section.
#
$(LINKCMD) : $(MAKE_DEPS) $(PMAKEFILE)
	$(Q)echo Creating linker command file $@ ... ;\
	rm -f $(LINKCMD) ;\
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
#    PAGEFLAGS sets a macro 'DECLARED_PAGED' to be used on function
#    and variable definitions for asserting what page they are in.
#    Only the XBM image files need this defined.
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

ifeq ($(CPU),m6809)
$(FON_OBJS): PAGEFLAGS="-Dstatic=__attribute__((section(\"page$(PAGE)\")))"
$(BASIC_OBJS) $(KERNEL_SW_OBJS) $(COMMON_OBJS): SOFTREG_CFLAGS=$(SOFTREG_OPTIONS)
endif

$(C_OBJS) : %.o : %.c 

$(FON_OBJS) : %.o : %.fon

$(FIF_OBJS) : %.o : %.fif

$(filter-out $(BASIC_OBJS),$(C_OBJS)) : $(C_DEPS) $(GENDEFINES) $(REQUIRED)

$(BASIC_OBJS) $(FON_OBJS) : $(MAKE_DEPS) $(GENDEFINES) $(REQUIRED)

$(FIF_OBJS) : $(GENDEFINES)

$(KERNEL_OBJS) : kernel/Makefile
$(COMMON_OBJS) : common/Makefile

$(C_OBJS) $(FON_OBJS) $(FIF_OBJS):
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
ifneq ($(CONFIG_BARE),y)
CONFIG_CMDS = dump strings switchmasks containers switches scores lamplists deffs drives
ifeq ($(CONFIG_FONT),y)
CONFIG_CMDS += fonts
endif
CONFIG_SRCS = $(CONFIG_CMDS:%=$(BLDDIR)/mach-%.c)
endif
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

$(CONFIG_FILES) : tools/genmachine platform/$(PLATFORM)/$(PLATFORM).md

ifdef GAME_FSMS
$(FSM_SRCS) : $(BLDDIR)/%.c : $(MACHINE_DIR)/%.fsm tools/fsmgen
	tools/fsmgen $(FSMFLAGS) -o $@ $<
endif

#######################################################################
###	Header File Targets
#######################################################################

#
# How to automake files of #defines
#
gendefines: $(GENDEFINES) $(MACH_LINKS)

include/gendefine_gid.h: $(FSM_SRCS) $(CONFIG_SRCS) $(TEMPLATE_SRCS)
	$(Q)echo Autogenerating task IDs... && \
		$(GENDEFINE) -p GID_ > $@

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

$(BLDDIR)/callset.c : $(MACH_LINKS) $(CONFIG_SRCS) $(TEMPLATE_SRCS) tools/gencallset
	$(Q)echo "Generating callsets ... " && rm -f $@ \
		&& tools/gencallset $(filter-out build/callset.c,$(C_OBJS:.o=.c)) # $(CALLSET_FLAGS)

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
	$(SCHED) -o $@ $(SCHED_FLAGS) $(SYSTEM_SCHEDULE) $(MACHINE_SCHEDULE)

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

$(HOST_OBJS) : %.o : %.c
	$(CC) $(CFLAGS) $(TOOL_CFLAGS) -o $@ -c $< >> $(ERR) 2>&1

#######################################################################
###	Standard Dependencies
#######################################################################

# Provide a target for .config that will run 'configure' if it does
# not exist.
.config:
	echo "No .config" && exit 1

#
# Symbolic links to the machine code.  Once set, code can reference
# 'mach' and 'include/mach' without knowing the specific machine type.
#
.mach:
	$(Q)echo "Setting symbolic link for machine source code..."
	$(Q)touch .mach && ln -s $(MACHINE_DIR) mach

.include_mach:
	$(Q)echo "Setting symbolic link for machine include files..."
	$(Q)touch .include_mach && cd include && ln -s $(MACHINE) mach

#
# Remake machine prototypes file
#
protos : include/$(MACHINE)/protos.h

include/$(MACHINE)/protos.h :
	cproto -o $@ -I include -I include/sys $(MACHINE)/*.c

#
# Install to the web server
# Set the location of the web documents in WEBDIR in your .config.
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
	$(Q)echo "FIF_OBJS = $(FIF_OBJS)"
	$(Q)echo "NUM_BLANK_PAGES = $(NUM_BLANK_PAGES)"
	$(Q)echo "CONFIG_DMD = $(CONFIG_DMD)"
	$(Q)echo "CONFIG_PIC = $(CONFIG_PIC)"
	$(Q)echo "MACH_DESC = $(MACH_DESC)"
	$(Q)echo "HOST_OBJS = $(HOST_OBJS)"
	$(Q)echo "CONFIG_BARE = $(CONFIG_BARE)"
	$(Q)echo "SCHED_FLAGS = $(SCHED_FLAGS)"

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
	$(Q)for dir in `echo . kernel common fonts images test $(MACHINE_DIR) $(PLATFORM_DIR)`;\
		do echo "Cleaning in '$$dir' ..." && \
		pushd $$dir >/dev/null && rm -f $(TMPFILES) && \
		popd >/dev/null ; done

.PHONY : clean_derived
clean_derived:
	$(Q)for file in `echo $(XBM_H) mach include/mach` ;\
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

