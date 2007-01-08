
CC = /usr/local/m6809/bin/gcc
CFLAGS = -I. -I../include -save-temps -O2 -Wall -fno-builtin -mdirect

C_OBJS = sndhw.o vector.o
ASM_OBJS = delay.o
OBJS = $(C_OBJS) $(ASM_OBJS)

DEPS = Makefile

ROMDIR = /home/bcd/roms/pinmame

SR = ../tools/srec2bin/srec2bin

ZIP = tz_92.zip

ROM = tzu18_l2.rom
SREC = tzu18_l2.srec
BIN = tzu18_l2.bin

.PHONY : install
install : build
	@echo "Installing into ZIP..." && \
		cp -p $(ROMDIR)/$(ZIP) . && \
		zip -d $(ZIP) $(ROM) && \
		zip $(ZIP) $(ROM) && \
		cp -p $(ZIP) $(ROMDIR)

.PHONY : build
build : $(ROM)

$(ROM) : blank496.bin $(BIN)
	@echo "Concatenating..." && cat $^ > $(ROM)

blank%.bin:
	@echo "Creating blank space..." && dd if=/dev/zero of=$@ bs=1k count=$* > /dev/null 2>&1

$(BIN) : $(SREC)
	@echo "Converting from S-records to binary..." && \
		../tools/srec2bin/srec2bin -o $@ -s 0xC000 -l 0x4000 -f 0xFF $<


$(SREC): $(OBJS) $(DEPS)
	@echo "Linking..." && $(CC) -Wl,--map \
		-Wl,--section-start -Wl,direct=0x200 \
		-Wl,-Tbss -Wl,0x300 \
		-Wl,-Ttext -Wl,0xC000 \
		-Wl,-Tdata -Wl,0xF000 \
		-o $@ $(OBJS) -nostdlib

$(OBJS) : $(DEPS)

$(C_OBJS) : %.o : %.c
	@echo "Compiling $<..." && $(CC) $(CFLAGS) -o $@ -c $<

$(ASM_OBJS) : %.o : %.s
	@echo "Compiling $<..." && $(CC) $(CFLAGS) -o $@ -x assembler-with-cpp -c $<

.PHONY : clean
clean :
	rm -f $(OBJS) *.map *.bin $(ROM) $(SREC) *.i
