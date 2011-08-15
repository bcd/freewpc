
# Root directory for the platform files
P := platform/wpcsound
CONFIG_BARE := y

# Common 6809 configuration
CPU := m6809
include cpu/$(CPU)/Makefile

# The default CFLAGS required on the WPC Sound Board
# A few notes:
# 1. -Wno-format is to not check format strings, because we define those
# differently than ANSI C does (e.g. look at sprintf()).
INT_CFLAGS = -mint8
CFLAGS += $(INT_CFLAGS) -mdirect -DCONFIG_PLATFORM_WPCSOUND -fno-builtin -mcode-section=.text -mdata-section=.text -mbss-section=ram -Wno-format
EXTRA_ASFLAGS += -DCONFIG_PLATFORM_WPCSOUND

# Rules for converting sound files into ROM
# Output options
SOX_DACOPTS := -b -c 1 -u -r 11025
SOX_CVSDOPTS := -b -c 1 -u -r 22050

DAC_CLIPS = $(DAC_OBJS:.o=.dac)
DAC_SRCS = $(DAC_OBJS:.o=.c)

$(DAC_SRCS) : %.c : %.dac
	$(BIN2C) $< $@

$(DAC_CLIPS) : %.dac : %.wav
	sox $< $(SOX_DACOPTS) $@

$(CVSD_CLIPS) : %.cvs : %.wav
	sox $< $(SOX_CVSDOPTS) $@

KERNEL_OBJS += $(P)/main.o $(P)/interrupt.o $(P)/volume.o $(P)/host.o \
	$(P)/dac.o $(P)/cvsd.o $(P)/fm.o # kernel/printf.o

DAC_OBJS += $(P)/bell.o
KERNEL_OBJS += $(DAC_OBJS)

KERNEL_ASM_OBJS += $(P)/start.o

