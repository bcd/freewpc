/* m04pst.c */

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
#include "m6804.h"

struct	mne	mne[] = {

	/* machine */

    {	NULL,	".setdp",	S_SDP,		0,	0	},

	/* system */

    {	NULL,	"CON",		S_ATYP,		0,	A_CON	},
    {	NULL,	"OVR",		S_ATYP,		0,	A_OVR	},
    {	NULL,	"REL",		S_ATYP,		0,	A_REL	},
    {	NULL,	"ABS",		S_ATYP,		0,	A_ABS|A_OVR	},
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

	/* 6804 */

    {	NULL,	"jmp",		S_TYP1,		0,	0x9000	},
    {	NULL,	"jsr",		S_TYP1,		0,	0x8000	},

    {	NULL,	"sta",		S_TYP2,		0,	0xE1	},
    {	NULL,	"staa",		S_TYP2,		0,	0xE1	},
    {	NULL,	"inc",		S_TYP2,		0,	0xE6	},
    {	NULL,	"dec",		S_TYP2,		0,	0xE7	},

    {	NULL,	"lda",		S_TYP3,		0,	0xE0	},
    {	NULL,	"ldaa",		S_TYP3,		0,	0xE0	},
    {	NULL,	"add",		S_TYP3,		0,	0xE2	},
    {	NULL,	"adda",		S_TYP3,		0,	0xE2	},
    {	NULL,	"sub",		S_TYP3,		0,	0xE3	},
    {	NULL,	"suba",		S_TYP3,		0,	0xE3	},
    {	NULL,	"cmp",		S_TYP3,		0,	0xE4	},
    {	NULL,	"cmpa",		S_TYP3,		0,	0xE4	},
    {	NULL,	"and",		S_TYP3,		0,	0xE5	},
    {	NULL,	"anda",		S_TYP3,		0,	0xE5	},

    {	NULL,	"clrx",		S_CLR,		0,	0x80	},
    {	NULL,	"clry",		S_CLR,		0,	0x81	},

    {	NULL,	"ldxi",		S_LD,		0,	0x80	},
    {	NULL,	"ldyi",		S_LD,		0,	0x81	},

    {	NULL,	"mvi",		S_MVI,		0,	0xB0	},

    {	NULL,	"asla",		S_APOST,	0,	0xFA	},
    {	NULL,	"clra",		S_APOST,	0,	0xFB	},
    {	NULL,	"inca",		S_APOST,	0,	0xFE	},
    {	NULL,	"deca",		S_APOST,	0,	0xFF	},

    {	NULL,	"bap",		S_BPM,		0,	0xC7	},
    {	NULL,	"bam",		S_BPM,		0,	0xCF	},
    {	NULL,	"bxpl",		S_BXPM,		0,	0xC7	},
    {	NULL,	"bxmi",		S_BXPM,		0,	0xCF	},
    {	NULL,	"bypl",		S_BYPM,		0,	0xC7	},
    {	NULL,	"bymi",		S_BYPM,		0,	0xCF	},

    {	NULL,	"brclr",	S_BTB,		0,	0xC0	},
    {	NULL,	"brset",	S_BTB,		0,	0xC8	},
    {	NULL,	"bclr",		S_BSC,		0,	0xD0	},
    {	NULL,	"bset",		S_BSC,		0,	0xD8	},

    {	NULL,	"bcc",		S_BRA,		0,	0x40	},
    {	NULL,	"bhs",		S_BRA,		0,	0x40	},
    {	NULL,	"bcs",		S_BRA,		0,	0x60	},
    {	NULL,	"blo",		S_BRA,		0,	0x60	},
    {	NULL,	"bne",		S_BRA,		0,	0x00	},
    {	NULL,	"beq",		S_BRA,		0,	0x20	},

    {	NULL,	"nop",		S_INH,		0,	0x20	},
    {	NULL,	"inx",		S_INH,		0,	0xA8	},
    {	NULL,	"incx",		S_INH,		0,	0xA8	},
    {	NULL,	"iny",		S_INH,		0,	0xA9	},
    {	NULL,	"incy",		S_INH,		0,	0xA9	},
    {	NULL,	"txa",		S_INH,		0,	0xAC	},
    {	NULL,	"tya",		S_INH,		0,	0xAD	},
    {	NULL,	"rrb",		S_INH,		0,	0xB1	},
    {	NULL,	"rti",		S_INH,		0,	0xB2	},
    {	NULL,	"rts",		S_INH,		0,	0xB3	},
    {	NULL,	"coma",		S_INH,		0,	0xB4	},
    {	NULL,	"rola",		S_INH,		0,	0xB5	},
    {	NULL,	"stop",		S_INH,		0,	0xB6	},
    {	NULL,	"wait",		S_INH,		0,	0xB7	},
    {	NULL,	"dex",		S_INH,		0,	0xB8	},
    {	NULL,	"decx",		S_INH,		0,	0xB8	},
    {	NULL,	"dey",		S_INH,		0,	0xB9	},
    {	NULL,	"decy",		S_INH,		0,	0xB9	},
    {	NULL,	"tax",		S_INH,		0,	0xBC	},
    {	NULL,	"tay",		S_INH,		S_END,	0xBD	}
};
