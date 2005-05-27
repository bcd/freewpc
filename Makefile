
ERR = err

EPTOOLS_DIR=/home/bcd/eptools

EPTOOLS_ROM_DIR=$(EPTOOLS_DIR)/roms

MAME_ROMDIR=/home/bcd/eptools/mameroms

DEFAULT_TARGET=install_as_tz92

DEPS = m6809.m4 syscall.m4 wpc.h Makefile

OS_SRCS = sys.s clib.s math.s trace.s heap.s \
	switch.s lamp.s sol.s sound.s task.s \
	dmd.s segment.s lampset.s test.s \
	deff.s table.s \
	tz.s \
	service.s sysinfo.s timer.s \
	vector.s

GAME_SRCS = clock.s

SRCS = $(OS_SRCS) $(GAME_SRCS)

ASMFLAGS = $(A)

PREPROCS = $(SRCS:.s=.sp)
OBJS = $(SRCS:.s=.rel)

default_target : clean_err check_prereqs $(DEFAULT_TARGET)

debug: clean_err check_prereqs
	$(MAKE) $(DEFAULT_TARGET) A="-N --save-temps"

clean_err:
	rm -f $(ERR)

check_prereqs :

install_as_tz92 : install
	@echo Copying to mame directory ...; \
	cp -p freewpc.rom $(MAME_ROMDIR)/tzone9_2.rom; \
	cd $(MAME_ROMDIR); \
	rm -f tz_92.zip; \
	zip -9 tz_92.zip tzone9_2.rom tzu*.rom

install : freewpc.rom
	@echo Copying to ROM directory ... && cp -p $< $(EPTOOLS_ROM_DIR)

freewpc.rom : blank256.bin blank128.bin blank64.bin blank32.bin sys.bin
	@echo Padding ... && cat blank256.bin blank128.bin blank64.bin blank32.bin sys.bin > $@

blank%.bin:
	@echo Creating blank file ... && dd if=/dev/zero of=$@ bs=1k count=$*

sys.bin : sys.s19
	@echo Converting to binary ... && srec_cat sys.s19 --motorola --output - --binary | dd of=sys.bin bs=1k skip=32

sys.s19 : sys.lnk $(OBJS)
	@echo Linking... && aslink -f sys >> $(ERR) 2>&1

$(OBJS) : %.rel : %.s $(DEPS)
	./sasm09 $(ASMFLAGS) $<

#$(OBJS) : %.rel : %.sp
#	@echo -n Assembling $*.s ... && \
#	as6809 -aglxoz $*.sp; \
#	(if [ "$$?" != "0" ]; then rm -f $*.rel; exit 1; fi) \
#	>> $(ERR) 2>&1
#
#$(PREPROCS) : %.sp : %.s $(DEPS)
#	@echo Preprocessing $*.s ... && \
#		cat $*.s | awk '{print $$0 "   ;;__SASMLINE " line++; }' - | m4 m6809.m4 syscall.m4 - > $*.sp

sys.lnk : $(DEPS)
	@echo Creating linker command file...
	@rm -f sys.lnk
	@echo "-mxswz" >> sys.lnk
	@echo "-b fastram = 0x0" >> sys.lnk
	@echo "-b ram = 0x100" >> sys.lnk
	#@echo "-b rom = 0x4000" >> sys.lnk
	@echo "-b sysrom = 0x8000" >> sys.lnk
	@echo "-b vector = 0xFFF0" >> sys.lnk
	@for f in `echo $(OBJS)`; do echo $$f >> sys.lnk; done
	@echo "-e" >> sys.lnk

clean:
	rm -f *.sp *.o *.rel *.lnk *.s19 *.map *.bin *.rom *.lst *.s2 *.s3 *.s4 err
