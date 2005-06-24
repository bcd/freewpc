#
# FreeWPC makefile
#

#######################################################################
###	Configuration
#######################################################################

# Set this to the name of the machine for which you are targetting.
TARGET_MACHINE = tz92

# Set this to the path where the final ROM image should be installed
TARGET_ROMPATH = /home/bcd/eptools/mameroms

#######################################################################
###	Filenames
#######################################################################

# Where to write errors
ERR = err
TMPFILES += $(ERR)

# The linker command file (generated dynamically)
LINKCMD = sys.lnk
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
AS_OS_OBJS = sys.o clib.o trace.o heap.o \
	switch.o lamp1.o task1.o \
	dmd1.o segment.o lampset.o test1.o \
	deff.o table.o \
	tz.o \
	service.o \
	vector.o \

OS_OBJS = div10.o init.o sysinfo.o task.o lamp.o sol.o dmd.o \
	ctry.o switches.o sound.o

OS_INCLUDES = wpc.h


GAME_OBJS = clock.o
AS_GAME_OBJS =

GAME_INCLUDES =

INCLUDES = $(OS_INCLUDES) $(GAME_INCLUDES)

ASMFLAGS = $(A) -I. -Iinclude -D__SASM__
CFLAGS = -I. -Iinclude -I../../coco/libc-coco/include

CFLAGS += -O1 -fstrength-reduce -frerun-loop-opt -fomit-frame-pointer -Wunknown-pragmas
CFLAGS += -da
CFLAGS += -Wall
CFLAGS += -Werror-implicit-function-declaration

OBJS = $(OS_OBJS) $(GAME_OBJS)
AS_OBJS = $(AS_OS_OBJS) $(AS_GAME_OBJS)

DEPS = $(DEFMACROS) $(INCLUDES) Makefile

INSTALL_TARGET=install_$(TARGET_MACHINE)

#######################################################################
###	Begin Makefile Targets
#######################################################################

default_target : clean_err check_prereqs $(INSTALL_TARGET)

debug: clean_err check_prereqs
	$(MAKE) $(DEFAULT_TARGET) A="-N --save-temps"

clean_err:
	rm -f $(ERR)

check_prereqs :

install_tz92 : freewpc.rom
	@echo Copying to mame directory ...; \
	cp -p freewpc.rom $(TARGET_ROMPATH)/tzone9_2.rom; \
	cd $(TARGET_ROMPATH); \
	rm -f tz_92.zip; \
	zip -9 tz_92.zip tzone9_2.rom tzu*.rom

freewpc.rom : blank256.bin blank128.bin blank64.bin blank32.bin sys.bin
	@echo Padding ... && cat blank256.bin blank128.bin blank64.bin blank32.bin sys.bin > $@

blank%.bin:
	@echo Creating blank file ... && dd if=/dev/zero of=$@ bs=1k count=$*

sys.bin : sys.s19
	@echo Converting to binary ... && $(ROMMER) sys.s19 --motorola --output - --binary | dd of=sys.bin bs=1k skip=32

sys.s19 : $(LINKCMD) $(OBJS) $(AS_OBJS)
	@echo Linking... && aslink -f sys >> $(ERR) 2>&1

$(AS_OBJS) : %.o : %.s $(AS) $(DEPS)
	$(AS) $(ASMFLAGS) $<

$(OBJS) : %.o : %.c $(CC) $(DEPS)
	@echo Compiling $< ... && $(CC) -o $(@:.o=.S) -S $(CFLAGS) $<
	@$(CC) -o $@ -c $(CFLAGS) $< > /dev/null 2>&1

$(LINKCMD) : $(DEPS)
	@echo Creating linker command file...
	@rm -f $(LINKCMD)
	@echo "-mxswz" >> $(LINKCMD)
	@echo "-b fastram = 0x0" >> $(LINKCMD)
	@echo "-b ram = 0x100" >> $(LINKCMD)
	@echo "-b _DATA = 0x800" >> $(LINKCMD)
	#@echo "-b rom = 0x4000" >> $(LINKCMD)
	@echo "-b sysrom = 0x8000" >> $(LINKCMD)
	@echo "-b vector = 0xFFF0" >> $(LINKCMD)
	@for f in `echo $(AS_OBJS) $(OBJS)`; do echo $$f >> $(LINKCMD); done
	@echo "/home/bcd/src/coco/libc-coco/libc.a" >> $(LINKCMD)
	@echo "-e" >> $(LINKCMD)

clean:
	rm -f *.sp *.o *.rel $(LINKCMD) *.s19 *.map *.bin *.rom *.lst *.s2 *.s3 *.s4 *.S *.c.[0-9]*.* *.lst $(ERR)
