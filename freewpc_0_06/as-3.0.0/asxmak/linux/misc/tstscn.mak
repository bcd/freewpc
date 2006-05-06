####################################################################
#  tstscn.mak	Makefile
#		LINUX Version
#		=============
#
#  2-Apr-99	Alan Baldwin
#
####################################################################

.PHONY:	all

all:
	../exe/as6500 -glaxff ../../../asxtst/asmtl.asm
	../exe/asxscn ../../../asxtst/asmtl.lst
	../exe/as6800 -glaxff ../../../asxtst/asmth.asm
	../exe/asxscn ../../../asxtst/asmth.lst
	../exe/as6801 -glaxff ../../../asxtst/asmth.asm
	../exe/asxscn ../../../asxtst/asmth.lst
	../exe/as6804 -glaxff ../../../asxtst/asmth.asm
	../exe/asxscn ../../../asxtst/asmth.lst
	../exe/as6805 -glaxff ../../../asxtst/asmth.asm
	../exe/asxscn ../../../asxtst/asmth.lst
	../exe/as6808 -glaxff ../../../asxtst/asmth.asm
	../exe/asxscn ../../../asxtst/asmth.lst
	../exe/as6809 -glaxff ../../../asxtst/asmth.asm
	../exe/asxscn ../../../asxtst/asmth.lst
	../exe/as6811 -glaxff ../../../asxtst/asmth.asm
	../exe/asxscn ../../../asxtst/asmth.lst
	../exe/as6812 -glaxff ../../../asxtst/asmth.asm
	../exe/asxscn ../../../asxtst/asmth.lst
	../exe/as6816 -glaxff ../../../as6816/t6816.asm
	../exe/asxscn ../../../asxtst/asmth.lst
	../exe/as8085 -glaxff ../../../asxtst/asmtl.asm
	../exe/asxscn ../../../asxtst/asmtl.lst
	../exe/as8xcxxx -glaxff ../../../asxtst/asmth.asm
	../exe/asxscn ../../../asxtst/asmth.lst
	../exe/as8xcxxx -glaxff ../../../asxtst/asmt3h.asm
	../exe/asxscn -x3 ../../../asxtst/asmt3h.lst
	../exe/as8051 -glaxff ../../../asxtst/asmth.asm
	../exe/asxscn ../../../asxtst/asmth.lst
	../exe/asz80 -glaxff ../../../asxtst/asmtl.asm
	../exe/asxscn ../../../asxtst/asmtl.lst
	../exe/ash8 -glaxff ../../../asxtst/asmth.asm
	../exe/asxscn ../../../asxtst/asmth.lst
	../exe/as6500 -glaxff ../../../as6500/t6500.asm
	../exe/asxscn ../../../as6500/t6500.lst
	../exe/as6500 -glaxff ../../../as6500/t6500s.asm
	../exe/asxscn ../../../as6500/t6500s.lst
	../exe/as6800 -glaxff ../../../as6800/t6800.asm
	../exe/asxscn ../../../as6800/t6800.lst
	../exe/as6800 -glaxff ../../../as6800/t6800s.asm
	../exe/asxscn ../../../as6800/t6800s.lst
	../exe/as6801 -glaxff ../../../as6801/t6801.asm
	../exe/asxscn ../../../as6801/t6801.lst
	../exe/as6801 -glaxff ../../../as6801/t6801s.asm
	../exe/asxscn ../../../as6801/t6801s.lst
	../exe/as6804 -glaxff ../../../as6804/t6804s.asm
	../exe/asxscn ../../../as6804/t6804s.lst
	../exe/as6805 -glaxff ../../../as6805/t6805s.asm
	../exe/asxscn ../../../as6805/t6805s.lst
	../exe/as6808 -glaxff ../../../as6808/t6808s.asm
	../exe/asxscn ../../../as6808/t6808s.lst
	../exe/as6809 -glaxff ../../../as6809/t6809.asm
	../exe/asxscn ../../../as6809/t6809.lst
	../exe/as6811 -glaxff ../../../as6811/t6811.asm
	../exe/asxscn ../../../as6811/t6811.lst
	../exe/as6811 -glaxff ../../../as6811/t6811s.asm
	../exe/asxscn ../../../as6811/t6811s.lst
	../exe/as6812 -glaxff ../../../as6812/t6812.asm
	../exe/asxscn ../../../as6812/t6812.lst
	../exe/as6812 -glaxff ../../../as6812/t6812a.asm
	../exe/asxscn ../../../as6812/t6812a.lst
	../exe/as6812 -glaxff ../../../as6812/t6812b.asm
	../exe/asxscn ../../../as6812/t6812b.lst
	../exe/as6816 -glaxff ../../../as6816/t6816.asm
	../exe/asxscn ../../../as6816/t6816.lst
	../exe/as8085 -glaxff ../../../as8085/t8085.asm
	../exe/asxscn ../../../as8085/t8085.lst
	../exe/as8051 -glaxff ../../../as8051/t8051.asm
	../exe/asxscn ../../../as8051/t8051.lst
	../exe/as8xcxxx -glaxff ../../../as8xcxxx/t80c390.asm
	../exe/asxscn -x3 ../../../as8xcxxx/t80c390.lst
	../exe/asz80 -glaxff ../../../asz80/tz80.asm
	../exe/asxscn ../../../asz80/tz80.lst
	../exe/ash8 -glaxff ../../../ash8/th8.asm
	../exe/asxscn ../../../ash8/th8.lst

### end #########################################################
