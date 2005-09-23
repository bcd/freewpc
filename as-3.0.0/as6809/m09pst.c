/* M09PST:C */

/*
 * (C) Copyright 1989-2000
 * All Rights Reserved
 *
 * Alan R. Baldwin
 * 721 Berkeley St.
 * Kent, Ohio  44240
 */

#include <stdio.h>
#include <setjmp.h>
#include "asxxxx.h"
#include "m6809.h"

struct	mne	mne[] = {

	/* machine */

    {	NULL,	".setdp",	S_SDP,		0,	0	},

	/* system */

    {	NULL,	"CON",		S_ATYP,		0,	A_CON	},
    {	NULL,	"OVR",		S_ATYP,		0,	A_OVR	},
    {	NULL,	"REL",		S_ATYP,		0,	A_REL	},
    {	NULL,	"ABS",		S_ATYP,		0,  A_ABS|A_OVR	},
    {	NULL,	"NOPAG",	S_ATYP,		0,	A_NOPAG	},
    {	NULL,	"PAG",		S_ATYP,		0,	A_PAG	},

    {	NULL,	".byte",	S_DATA,		0,	1	},
    {	NULL,	".db",		S_DATA,		0,	1	},
    {	NULL,	".word",	S_DATA,		0,	2	},
    {	NULL,	".dw",		S_DATA,		0,	2	},
    {	NULL,	".ascii",	S_ASCII,	0,	0	},
    {	NULL,	".asciz",	S_ASCIZ,	0,	0	},
    {	NULL,	".blkb",	S_BLK,		0,	1	},
    {	NULL,	".ds",		S_BLK,		0,	1	},
    {	NULL,	".blkw",	S_BLK,		0,	2	},
    {	NULL,	".page",	S_PAGE,		0,	0	},
    {	NULL,	".title",	S_TITLE,	0,	0	},
    {	NULL,	".sbttl",	S_SBTL,		0,	0	},
    {	NULL,	".globl",	S_GLOBL,	0,	0	},
    {	NULL,	".area",	S_DAREA,	0,	0	},
    {	NULL,	".even",	S_EVEN,		0,	0	},
    {	NULL,	".odd",		S_ODD,		0,	0	},
    {	NULL,	".if",		S_IF,		0,	0	},
    {	NULL,	".else",	S_ELSE,		0,	0	},
    {	NULL,	".endif",	S_ENDIF,	0,	0	},
    {	NULL,	".include",	S_INCL,		0,	0	},
    {	NULL,	".radix",	S_RADIX,	0,	0	},
    {	NULL,	".org",		S_ORG,		0,	0	},
    {	NULL,	".module",	S_MODUL,	0,	0	},
    {	NULL,	".ascis",	S_ASCIS,	0,	0	},
    {	NULL,	".assume",	S_ERROR,	0,	0	},
    {	NULL,	".error",	S_ERROR,	0,	1	},

	/* 6800 Compatibility */

    {	NULL,	"ldaa",		S_ACC,		0,	0x86	},
    {	NULL,	"ldab",		S_ACC,		0,	0xC6	},
    {	NULL,	"oraa",		S_ACC,		0,	0x8A	},
    {	NULL,	"orab",		S_ACC,		0,	0xCA	},
    {	NULL,	"staa",		S_STR,		0,	0x87	},
    {	NULL,	"stab",		S_STR,		0,	0xC7	},

	/* if this is changed, change struct opdata mc6800[] */

    {	NULL,	"aba",		S_6800,		0,	0	},
    {	NULL,	"cba",		S_6800,		0,	1	},
    {	NULL,	"clc",		S_6800,		0,	2	},
    {	NULL,	"cli",		S_6800,		0,	3	},
    {	NULL,	"clv",		S_6800,		0,	4	},
    {	NULL,	"des",		S_6800,		0,	5	},
    {	NULL,	"dex",		S_6800,		0,	6	},
    {	NULL,	"ins",		S_6800,		0,	7	},
    {	NULL,	"inx",		S_6800,		0,	8	},
    {	NULL,	"psha",		S_6800,		0,	9	},
    {	NULL,	"pshb",		S_6800,		0,	10	},
    {	NULL,	"pula",		S_6800,		0,	11	},
    {	NULL,	"pulb",		S_6800,		0,	12	},
    {	NULL,	"sba",		S_6800,		0,	13	},
    {	NULL,	"sec",		S_6800,		0,	14	},
    {	NULL,	"sei",		S_6800,		0,	15	},
    {	NULL,	"sev",		S_6800,		0,	16	},
    {	NULL,	"tab",		S_6800,		0,	17	},
    {	NULL,	"tap",		S_6800,		0,	18	},
    {	NULL,	"tba",		S_6800,		0,	19	},
    {	NULL,	"tpa",		S_6800,		0,	20	},
    {	NULL,	"tsx",		S_6800,		0,	21	},
    {	NULL,	"txs",		S_6800,		0,	22	},
    {	NULL,	"wai",		S_6800,		0,	23	},

	/* 6809 */

    {	NULL,	"sty",		S_STR1,		0,	0x8F	},
    {	NULL,	"sts",		S_STR1,		0,	0xCF	},

    {	NULL,	"sta",		S_STR,		0,	0x87	},
    {	NULL,	"stb",		S_STR,		0,	0xC7	},
    {	NULL,	"std",		S_STR,		0,	0xCD	},
    {	NULL,	"stx",		S_STR,		0,	0x8F	},
    {	NULL,	"stu",		S_STR,		0,	0xCF	},
    {	NULL,	"jsr",		S_STR,		0,	0x8D	},

    {	NULL,	"cmpu",		S_LR2,		0,	0x83	},
    {	NULL,	"cmps",		S_LR2,		0,	0x8C	},

    {	NULL,	"cmpd",		S_LR1,		0,	0x83	},
    {	NULL,	"cmpy",		S_LR1,		0,	0x8C	},
    {	NULL,	"ldy",		S_LR1,		0,	0x8E	},
    {	NULL,	"lds",		S_LR1,		0,	0xCE	},

    {	NULL,	"subd",		S_LR,		0,	0x83	},
    {	NULL,	"addd",		S_LR,		0,	0xC3	},
    {	NULL,	"cmpx",		S_LR,		0,	0x8C	},
    {	NULL,	"cpx",		S_LR,		0,	0x8C	},
    {	NULL,	"ldd",		S_LR,		0,	0xCC	},
    {	NULL,	"ldx",		S_LR,		0,	0x8E	},
    {	NULL,	"ldu",		S_LR,		0,	0xCE	},

    {	NULL,	"leax",		S_LEA,		0,	0x30	},
    {	NULL,	"leay",		S_LEA,		0,	0x31	},
    {	NULL,	"leas",		S_LEA,		0,	0x32	},
    {	NULL,	"leau",		S_LEA,		0,	0x33	},

    {	NULL,	"pshs",		S_PULS,		0,	0x34	},
    {	NULL,	"puls",		S_PULS,		0,	0x35	},
    {	NULL,	"pshu",		S_PULU,		0,	0x36	},
    {	NULL,	"pulu",		S_PULU,		0,	0x37	},

    {	NULL,	"exg",		S_EXG,		0,	0x1E	},
    {	NULL,	"tfr",		S_EXG,		0,	0x1F	},

    {	NULL,	"cwai",		S_CC,		0,	0x3C	},
    {	NULL,	"orcc",		S_CC,		0,	0x1A	},
    {	NULL,	"andcc",	S_CC,		0,	0x1C	},

    {	NULL,	"swi3",		S_INH2,		0,	0x3F	},
    {	NULL,	"swi2",		S_INH1,		0,	0x3F	},
    {	NULL,	"swi1",		S_INH,		0,	0x3F	},

    {	NULL,	"abx",		S_INH,		0,	0x3A	},
    {	NULL,	"asla",		S_INH,		0,	0x48	},
    {	NULL,	"aslb",		S_INH,		0,	0x58	},
    {	NULL,	"asra",		S_INH,		0,	0x47	},
    {	NULL,	"asrb",		S_INH,		0,	0x57	},
    {	NULL,	"clra",		S_INH,		0,	0x4F	},
    {	NULL,	"clrb",		S_INH,		0,	0x5F	},
    {	NULL,	"coma",		S_INH,		0,	0x43	},
    {	NULL,	"comb",		S_INH,		0,	0x53	},
    {	NULL,	"daa",		S_INH,		0,	0x19	},
    {	NULL,	"deca",		S_INH,		0,	0x4A	},
    {	NULL,	"decb",		S_INH,		0,	0x5A	},
    {	NULL,	"inca",		S_INH,		0,	0x4C	},
    {	NULL,	"incb",		S_INH,		0,	0x5C	},
    {	NULL,	"lsla",		S_INH,		0,	0x48	},
    {	NULL,	"lslb",		S_INH,		0,	0x58	},
    {	NULL,	"lsra",		S_INH,		0,	0x44	},
    {	NULL,	"lsrb",		S_INH,		0,	0x54	},
    {	NULL,	"mul",		S_INH,		0,	0x3D	},
    {	NULL,	"nega",		S_INH,		0,	0x40	},
    {	NULL,	"negb",		S_INH,		0,	0x50	},
    {	NULL,	"nop",		S_INH,		0,	0x12	},
    {	NULL,	"rola",		S_INH,		0,	0x49	},
    {	NULL,	"rolb",		S_INH,		0,	0x59	},
    {	NULL,	"rora",		S_INH,		0,	0x46	},
    {	NULL,	"rorb",		S_INH,		0,	0x56	},
    {	NULL,	"rti",		S_INH,		0,	0x3B	},
    {	NULL,	"rts",		S_INH,		0,	0x39	},
    {	NULL,	"sex",		S_INH,		0,	0x1D	},
    {	NULL,	"swi",		S_INH,		0,	0x3F	},
    {	NULL,	"sync",		S_INH,		0,	0x13	},
    {	NULL,	"tsta",		S_INH,		0,	0x4D	},
    {	NULL,	"tstb",		S_INH,		0,	0x5D	},

    {	NULL,	"lbrn",		S_LBRA,		0,	0x21	},
    {	NULL,	"lbhi",		S_LBRA,		0,	0x22	},
    {	NULL,	"lbls",		S_LBRA,		0,	0x23	},
    {	NULL,	"lblos",	S_LBRA,		0,	0x23	},
    {	NULL,	"lbcc",		S_LBRA,		0,	0x24	},
    {	NULL,	"lbhs",		S_LBRA,		0,	0x24	},
    {	NULL,	"lbhis",	S_LBRA,		0,	0x24	},
    {	NULL,	"lbcs",		S_LBRA,		0,	0x25	},
    {	NULL,	"lblo",		S_LBRA,		0,	0x25	},
    {	NULL,	"lbne",		S_LBRA,		0,	0x26	},
    {	NULL,	"lbeq",		S_LBRA,		0,	0x27	},
    {	NULL,	"lbvc",		S_LBRA,		0,	0x28	},
    {	NULL,	"lbvs",		S_LBRA,		0,	0x29	},
    {	NULL,	"lbpl",		S_LBRA,		0,	0x2A	},
    {	NULL,	"lbmi",		S_LBRA,		0,	0x2B	},
    {	NULL,	"lbge",		S_LBRA,		0,	0x2C	},
    {	NULL,	"lblt",		S_LBRA,		0,	0x2D	},
    {	NULL,	"lbgt",		S_LBRA,		0,	0x2E	},
    {	NULL,	"lble",		S_LBRA,		0,	0x2F	},

    {	NULL,	"lbra",		S_LBSR,		0,	0x16	},
    {	NULL,	"lbsr",		S_LBSR,		0,	0x17	},

    {	NULL,	"neg",		S_SOP,		0,	0x40	},
    {	NULL,	"com",		S_SOP,		0,	0x43	},
    {	NULL,	"lsr",		S_SOP,		0,	0x44	},
    {	NULL,	"ror",		S_SOP,		0,	0x46	},
    {	NULL,	"asr",		S_SOP,		0,	0x47	},
    {	NULL,	"asl",		S_SOP,		0,	0x48	},
    {	NULL,	"lsl",		S_SOP,		0,	0x48	},
    {	NULL,	"rol",		S_SOP,		0,	0x49	},
    {	NULL,	"dec",		S_SOP,		0,	0x4A	},
    {	NULL,	"inc",		S_SOP,		0,	0x4C	},
    {	NULL,	"tst",		S_SOP,		0,	0x4D	},
    {	NULL,	"clr",		S_SOP,		0,	0x4F	},
    {	NULL,	"jmp",		S_SOP,		0,	0x4E	},

    {	NULL,	"suba",		S_ACC,		0,	0x80	},
    {	NULL,	"subb",		S_ACC,		0,	0xC0	},
    {	NULL,	"cmpa",		S_ACC,		0,	0x81	},
    {	NULL,	"cmpb",		S_ACC,		0,	0xC1	},
    {	NULL,	"sbca",		S_ACC,		0,	0x82	},
    {	NULL,	"sbcb",		S_ACC,		0,	0xC2	},
    {	NULL,	"anda",		S_ACC,		0,	0x84	},
    {	NULL,	"andb",		S_ACC,		0,	0xC4	},
    {	NULL,	"bita",		S_ACC,		0,	0x85	},
    {	NULL,	"bitb",		S_ACC,		0,	0xC5	},
    {	NULL,	"lda",		S_ACC,		0,	0x86	},
    {	NULL,	"ldb",		S_ACC,		0,	0xC6	},
    {	NULL,	"eora",		S_ACC,		0,	0x88	},
    {	NULL,	"eorb",		S_ACC,		0,	0xC8	},
    {	NULL,	"adca",		S_ACC,		0,	0x89	},
    {	NULL,	"adcb",		S_ACC,		0,	0xC9	},
    {	NULL,	"ora",		S_ACC,		0,	0x8A	},
    {	NULL,	"orb",		S_ACC,		0,	0xCA	},
    {	NULL,	"adda",		S_ACC,		0,	0x8B	},
    {	NULL,	"addb",		S_ACC,		0,	0xCB	},

    {	NULL,	"bra",		S_BRA,		0,	0x20	},
    {	NULL,	"brn",		S_BRA,		0,	0x21	},
    {	NULL,	"bhi",		S_BRA,		0,	0x22	},
    {	NULL,	"bls",		S_BRA,		0,	0x23	},
    {	NULL,	"blos",		S_BRA,		0,	0x23	},
    {	NULL,	"bcc",		S_BRA,		0,	0x24	},
    {	NULL,	"bhs",		S_BRA,		0,	0x24	},
    {	NULL,	"bhis",		S_BRA,		0,	0x24	},
    {	NULL,	"bcs",		S_BRA,		0,	0x25	},
    {	NULL,	"blo",		S_BRA,		0,	0x25	},
    {	NULL,	"bne",		S_BRA,		0,	0x26	},
    {	NULL,	"beq",		S_BRA,		0,	0x27	},
    {	NULL,	"bvc",		S_BRA,		0,	0x28	},
    {	NULL,	"bvs",		S_BRA,		0,	0x29	},
    {	NULL,	"bpl",		S_BRA,		0,	0x2A	},
    {	NULL,	"bmi",		S_BRA,		0,	0x2B	},
    {	NULL,	"bge",		S_BRA,		0,	0x2C	},
    {	NULL,	"blt",		S_BRA,		0,	0x2D	},
    {	NULL,	"bgt",		S_BRA,		0,	0x2E	},
    {	NULL,	"ble",		S_BRA,		0,	0x2F	},
    {	NULL,	"bsr",		S_BRA,		S_END,	0x8D	}
};

struct opdata mc6800[] = {

    {{	(char) 0x34, (char) 0x04,	/*	pshs	b	;aba	*/
	(char) 0xab, (char) 0xe0	/*	adda	,s+	*/	}},

    {{	(char) 0x34, (char) 0x04,	/*	pshs	b	;cba	*/
	(char) 0xa1, (char) 0xe0	/*	cmpa	,s+	*/	}},

    {{	(char) 0x1c, (char) 0xfe,	/*	andcc	#0xFE	;clc	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0x1c, (char) 0xef,	/*	andcc	#0xEF	;cli	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0x1c, (char) 0xfd,	/*	andcc	#0xFD	;clv	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0x32, (char) 0x7f,	/*	leas	-1,s	;des	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0x30, (char) 0x1f,	/*	leax	-1,x	;dex	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0x32, (char) 0x61,	/*	leas	1,s	;ins	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0x30, (char) 0x01,	/*	leax	1,x	;inx	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0x34, (char) 0x02,	/*	pshs	a	;psha	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0x34, (char) 0x04,	/*	pshs	b	;pshb	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0x35, (char) 0x02,	/*	puls	a	;pula	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0x35, (char) 0x04,	/*	puls	b	;pulb	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0x34, (char) 0x04,	/*	pshs	b	;sba	*/
	(char) 0xa0, (char) 0xe0	/*	suba	,s+	*/	}},

    {{	(char) 0x1a, (char) 0x01,	/*	orcc	#0x01	;sec	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0x1a, (char) 0x10,	/*	orcc	#0x10	;sei	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0x1a, (char) 0x02,	/*	orcc	#0x02	;sev	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0x1f, (char) 0x89,	/*	tfr	a,b	;tab	*/
	(char) 0x4d, (char) 0x00	/*	tsta	*/	}},

    {{	(char) 0x1f, (char) 0x8a,	/*	tfr	a,cc	;tap	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0x1f, (char) 0x98,	/*	tfr	b,a	;tba	*/
	(char) 0x5d, (char) 0x00	/*	tstb	*/	}},

    {{	(char) 0x1f, (char) 0xa8,	/*	tfr	cc,a	;tpa	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0x1f, (char) 0x41,	/*	tfr	s,x	;tsx	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0x1f, (char) 0x14,	/*	tfr	x,s	;txs	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0x3c, (char) 0xff,	/*	cwai	#0xFF	;wai	*/
	(char) 0x00, (char) 0x00	}}
};
