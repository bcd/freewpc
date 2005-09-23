######################################################################
#  Makefile
#		LINUX Version
#		=============
#
#  01/04/99	Alan Baldwin	Created for LINUX gcc 2.7.2.1 / make /
#
######################################################################

.PHONY:	all clean

all:	as6500 as6800 as6801 as6804	\
	as6805 as6808 as6809 as6811	\
	as6812 as6816 as8051 as8085	\
	ash8 asz80	\
	aslink	\
	asxcnv asxscn

clean:
	rm $(SRCASX)*.o
	rm $(SRCLNK)*.o
	rm $(SRCMISC)*.o
	rm $(SRC6500)*.o
	rm $(SRC6800)*.o
	rm $(SRC6801)*.o
	rm $(SRC6804)*.o
	rm $(SRC6805)*.o
	rm $(SRC6808)*.o
	rm $(SRC6809)*.o
	rm $(SRC6811)*.o
	rm $(SRC6812)*.o
	rm $(SRC6816)*.o
	rm $(SRC8051)*.o
	rm $(SRC8085)*.o
	rm $(SRCH8)*.o
	rm $(SRCZ80)*.o
	rm *.new


CC=		gcc
CCOPT=		-O3
CCFLAGS=	-Wall -funsigned-char $(CCOPT)

LD=		gcc
LDFLAGS=	-Wall

#  Set ASXBAS relative to this make file.

ASXBAS:=	../../../
ASXBIN:=	$(ASXBAS)bin/

SRCASX:=	$(ASXBAS)asxxsrc/
SRCLNK:=	$(ASXBAS)linksrc/
SRCMISC:=	$(ASXBAS)asxxmisc/
DSTEXE:=	$(ASXBAS)asxmak/linux/exe/

SRC6500=	$(ASXBAS)as6500/
SRC6800=	$(ASXBAS)as6800/
SRC6801=	$(ASXBAS)as6801/
SRC6804=	$(ASXBAS)as6804/
SRC6805=	$(ASXBAS)as6805/
SRC6808=	$(ASXBAS)as6808/
SRC6809=	$(ASXBAS)as6809/
SRC6811=	$(ASXBAS)as6811/
SRC6812=	$(ASXBAS)as6812/
SRC6816=	$(ASXBAS)as6816/
SRC8051=	$(ASXBAS)as8051/
SRC8085=	$(ASXBAS)as8085/
SRCH8=		$(ASXBAS)ash8/
SRCZ80=		$(ASXBAS)asz80/


.SUFFIXES:	.c .o .new

%.o: %.c
	$(CC) $(CCFLAGS) -I$(SRCMISC) -I$(SRCASX) -c $< -o $@

%: %.new
	strip $<
	cp $< $@
	cp $< $(DSTEXE)$@
	cp $< $(ASXBIN)$@


ASXX =	asdata.o asexpr.o aslex.o aslist.o	\
	asmain.o asout.o assubr.o assym.o

ASXXSRC =	$(addprefix $(SRCASX),$(ASXX))

$(ASXXSRC):	$(SRCMISC)alloc.h $(SRCASX)asxxxx.h


AS6500 =	r65adr.o r65ext.o r65mch.o r65pst.o

AS6500SRC =	$(addprefix $(SRC6500),$(AS6500))

$(AS6500SRC):	$(SRCMISC)alloc.h $(SRCASX)asxxxx.h $(SRC6500)r6500.h

as6500.new:	$(ASXXSRC) $(AS6500SRC)
	$(LD) $(LDFLAGS) $(ASXXSRC) $(AS6500SRC) -o as6500.new


AS6800 =	m00adr.o m00ext.o m00mch.o m00pst.o

AS6800SRC =	$(addprefix $(SRC6800),$(AS6800))

$(AS6800SRC):	$(SRCMISC)alloc.h $(SRCASX)asxxxx.h $(SRC6800)m6800.h

as6800.new:	$(ASXXSRC) $(AS6800SRC)
	$(LD) $(LDFLAGS) $(ASXXSRC) $(AS6800SRC) -o as6800.new


AS6801 =	m01adr.o m01ext.o m01mch.o m01pst.o

AS6801SRC =	$(addprefix $(SRC6801),$(AS6801))

$(AS6801SRC):	 $(SRCMISC)alloc.h $(SRCASX)asxxxx.h $(SRC6801)m6801.h

as6801.new:	$(ASXXSRC) $(AS6801SRC)
	$(LD) $(LDFLAGS) $(ASXXSRC) $(AS6801SRC) -o as6801.new


AS6804 =	m04adr.o m04ext.o m04mch.o m04pst.o

AS6804SRC =	$(addprefix $(SRC6804),$(AS6804))

$(AS6804SRC):	$(SRCMISC)alloc.h $(SRCASX)asxxxx.h $(SRC6804)m6804.h

as6804.new:	$(ASXXSRC) $(AS6804SRC)
	$(LD) $(LDFLAGS) $(ASXXSRC) $(AS6804SRC) -o as6804.new


AS6805 =	m05adr.o m05ext.o m05mch.o m05pst.o

AS6805SRC =	$(addprefix $(SRC6805),$(AS6805))

$(AS6805SRC):	$(SRCMISC)alloc.h $(SRCASX)asxxxx.h $(SRC6805)m6805.h

as6805.new:	$(ASXXSRC) $(AS6805SRC)
	$(LD) $(LDFLAGS) $(ASXXSRC) $(AS6805SRC) -o as6805.new


AS6808 =	m08adr.o m08ext.o m08mch.o m08pst.o

AS6808SRC =	$(addprefix $(SRC6808),$(AS6808))

$(AS6808SRC):	$(SRCMISC)alloc.h $(SRCASX)asxxxx.h $(SRC6808)m6808.h

as6808.new:	$(ASXXSRC) $(AS6808SRC)
	$(LD) $(LDFLAGS) $(ASXXSRC) $(AS6808SRC) -o as6808.new


AS6809 =	m09adr.o m09ext.o m09mch.o m09pst.o

AS6809SRC =	$(addprefix $(SRC6809),$(AS6809))

$(AS6809SRC):	$(SRCMISC)alloc.h $(SRCASX)asxxxx.h $(SRC6809)m6809.h

as6809.new:	$(ASXXSRC) $(AS6809SRC)
	$(LD) $(LDFLAGS) $(ASXXSRC) $(AS6809SRC) -o as6809.new


AS6811 =	m11adr.o m11ext.o m11mch.o m11pst.o

AS6811SRC =	$(addprefix $(SRC6811),$(AS6811))

$(AS6811SRC):	$(SRCMISC)alloc.h $(SRCASX)asxxxx.h $(SRC6811)m6811.h

as6811.new:	$(ASXXSRC) $(AS6811SRC)
	$(LD) $(LDFLAGS) $(ASXXSRC) $(AS6811SRC) -o as6811.new


AS6812 =	m12adr.o m12ext.o m12mch.o m12pst.o

AS6812SRC =	$(addprefix $(SRC6812),$(AS6812))

$(AS6812SRC):	$(SRCMISC)alloc.h $(SRCASX)asxxxx.h $(SRC6812)m6812.h

as6812.new:	$(ASXXSRC) $(AS6812SRC)
	$(LD) $(LDFLAGS) $(ASXXSRC) $(AS6812SRC) -o as6812.new


AS6816 =	m16adr.o m16ext.o m16mch.o m16pst.o

AS6816SRC =	$(addprefix $(SRC6816),$(AS6816))

$(AS6816SRC):	$(SRCMISC)alloc.h $(SRCASX)asxxxx.h $(SRC6816)m6816.h

as6816.new:	$(ASXXSRC) $(AS6816SRC)
	$(LD) $(LDFLAGS) $(ASXXSRC) $(AS6816SRC) -o as6816.new


AS8051 =	i51adr.o i51ext.o i51mch.o i51pst.o

AS8051SRC =	$(addprefix $(SRC8051),$(AS8051))

$(AS8051SRC):	$(SRCMISC)alloc.h $(SRCASX)asxxxx.h $(SRC8051)i8051.h

as8051.new:	$(ASXXSRC) $(AS8051SRC)
	$(LD) $(LDFLAGS) $(ASXXSRC) $(AS8051SRC) -o as8051.new


AS8085 =	i85ext.o i85mch.o i85pst.o

AS8085SRC =	$(addprefix $(SRC8085),$(AS8085))

$(AS8085SRC):	$(SRCMISC)alloc.h $(SRCASX)asxxxx.h $(SRC8085)i8085.h

as8085.new:	$(ASXXSRC) $(AS8085SRC)
	$(LD) $(LDFLAGS) $(ASXXSRC) $(AS8085SRC) -o as8085.new


ASH8 =		h8adr.o h8ext.o h8mch.o h8pst.o

ASH8SRC =	$(addprefix $(SRCH8),$(ASH8))

$(ASH8SRC):	$(SRCMISC)alloc.h $(SRCASX)asxxxx.h $(SRCH8)h8.h

ash8.new:	$(ASXXSRC) $(ASH8SRC)
	$(LD) $(LDFLAGS) $(ASXXSRC) $(ASH8SRC) -o ash8.new


ASZ80 =		z80adr.o z80ext.o z80mch.o z80pst.o

ASZ80SRC =	$(addprefix $(SRCZ80),$(ASZ80))

$(ASZ80SRC):	$(SRCMISC)alloc.h $(SRCASX)asxxxx.h $(SRCZ80)z80.h

asz80.new:	$(ASXXSRC) $(ASZ80SRC)
	$(LD) $(LDFLAGS) $(ASXXSRC) $(ASZ80SRC) -o asz80.new


ASLINK =	lkarea.o lkdata.o lkeval.o lkhead.o	\
		lkihx.o lklex.o lklibr.o lklist.o	\
		lkmain.o lkrloc.o lks19.o lksym.o

ASLINKSRC =	$(addprefix $(SRCLNK),$(ASLINK))

$(ASLINK):	$(SRCMISC)alloc.h $(SRCLNK)aslink.h

aslink.new:	$(ASLINKSRC)
	$(LD) $(LDFLAGS) $(ASLINKSRC) -o aslink.new


$(SRCMISC)asxcnv.o:	$(SRCASX)asxxxx.h
asxcnv.new:	$(SRCMISC)asxcnv.o
	$(LD) $(LDFLAGS) $(SRCMISC)asxcnv.o -o asxcnv.new

$(SRCMISC)asxscn.o:	$(SRCASX)asxxxx.h
asxscn.new:	$(SRCMISC)asxscn.o
	$(LD) $(LDFLAGS) $(SRCMISC)asxscn.o -o asxscn.new


### end #########################################################
