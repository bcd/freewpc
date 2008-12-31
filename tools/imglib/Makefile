CC=gcc
CFLAGS = -g -I../../include -I../../build -I../wpclib -DCONFIG_NATIVE -DWPC_DMD_LOW_PAGE=0 -DWPC_DMD_HIGH_PAGE=0

FONT_DIR = ../../fonts
WPCLIB_DIR = ../../../wpclib
IMG_OBJS = imglib.o font.o wpclib.o
FONT_OBJS = mono5.o mono9.o
FON_OBJS = fixed10.o
OBJS = $(IMG_OBJS) $(FONT_OBJS) $(FON_OBJS)
VPATH += ../wpclib

PROGS = test_compression genmask dmdsim testpgm

all : $(PROGS)

$(PROGS) : % : $(OBJS) %.o
	$(CC) -o $@ $^ -lm

$(IMG_OBJS) : %.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $(FONT_INCLUDES) $^

$(FONT_OBJS) : %.o : $(FONT_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ -c $(FONT_INCLUDES) $^

$(FON_OBJS) : %.o : $(FONT_DIR)/%.fon
	$(CC) $(CFLAGS) -o $@ -x c -c $(FONT_INCLUDES) $^

clean:
	rm -f $(OBJS)
