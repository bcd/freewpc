/* h8pst.c */

/*
 * (C) Copyright 1994-2000
 * All Rights Reserved
 *
 * Alan R. Baldwin
 * 721 Berkeley St.
 * Kent, Ohio  44240
 */

#include <stdio.h>
#include <setjmp.h>
#include "asxxxx.h"
#include "h8.h"

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

					/*	0 = xxx		*/
	/* H8/3xx */			/*	1 = xxx.b	*/
					/*	2 = xxx.w	*/

    {	NULL,	"adds",		S_OPS,		0,	0x0B00	},
    {	NULL,	"subs",		S_OPS,		0,	0x1B00	},

    {	NULL,	"adds.w",	S_OPS,		2,	0x0B00	},
    {	NULL,	"subs.w",	S_OPS,		2,	0x1B00	},

    {	NULL,	"add",		S_ADD,		0,	0x8008	},
    {	NULL,	"addx",		S_OPX,		0,	0x900E	},
    {	NULL,	"cmp",		S_CMP,		0,	0xA01C	},
    {	NULL,	"subx",		S_OPX,		0,	0xB01E	},
    {	NULL,	"or",		S_OP,		0,	0xC014	},
    {	NULL,	"xor",		S_OP,		0,	0xD015	},
    {	NULL,	"and",		S_OP,		0,	0xE016	},
    {	NULL,	"mov",		S_MOV,		0,	0xF00C	},

    {	NULL,	"add.b",	S_ADD,		1,	0x8008	},
    {	NULL,	"addx.b",	S_OP,		1,	0x900E	},
    {	NULL,	"cmp.b",	S_CMP,		1,	0xA01C	},
    {	NULL,	"subx.b",	S_OP,		1,	0xB01E	},
    {	NULL,	"or.b",		S_OP,		1,	0xC014	},
    {	NULL,	"xor.b",	S_OP,		1,	0xD015	},
    {	NULL,	"and.b",	S_OP,		1,	0xE016	},
    {	NULL,	"mov.b",	S_MOV,		1,	0xF00C	},

    {	NULL,	"add.w",	S_ADD,		2,	0x8009	},
    {	NULL,	"cmp.w",	S_CMP,		2,	0xA01D	},
    {	NULL,	"mov.w",	S_MOV,		2,	0xF00D	},

    {	NULL,	"sub",		S_SUB,		0,	0x0018	},
    {	NULL,	"sub.b",	S_SUB,		1,	0x0018	},
    {	NULL,	"sub.w",	S_SUB,		2,	0x0019	},

    {	NULL,	"orc",		S_CCR,		0,	0x0400	},
    {	NULL,	"xorc",		S_CCR,		0,	0x0500	},
    {	NULL,	"andc",		S_CCR,		0,	0x0600	},
    {	NULL,	"ldc",		S_CCR,		1,	0x0700	},
    {	NULL,	"stc",		S_CCR,		2,	0x0200	},

    {	NULL,	"inc",		S_SOP,		0,	0x0A00	},
    {	NULL,	"daa",		S_SOP,		0,	0x0F00	},
    {	NULL,	"dec",		S_SOP,		0,	0x1A00	},
    {	NULL,	"das",		S_SOP,		0,	0x1F00	},

    {	NULL,	"inc.b",	S_SOP,		1,	0x0A00	},
    {	NULL,	"daa.b",	S_SOP,		1,	0x0F00	},
    {	NULL,	"dec.b",	S_SOP,		1,	0x1A00	},
    {	NULL,	"das.b",	S_SOP,		1,	0x1F00	},

    {	NULL,	"not",		S_SOP,		0,	0x1700	},
    {	NULL,	"neg",		S_SOP,		0,	0x1780	},

    {	NULL,	"not.b",	S_SOP,		1,	0x1700	},
    {	NULL,	"neg.b",	S_SOP,		1,	0x1780	},

    {	NULL,	"nop",		S_INH,		0,	0x0000	},
    {	NULL,	"sleep",	S_INH,		0,	0x0180	},
    {	NULL,	"rts",		S_INH,		0,	0x5470	},
    {	NULL,	"rte",		S_INH,		0,	0x5670	},

    {	NULL,	"mulxu",	S_MLDV,		0,	0x5000	},
    {	NULL,	"divxu",	S_MLDV,		0,	0x5100	},

    {	NULL,	"mulxu.b",	S_MLDV,		1,	0x5000	},
    {	NULL,	"divxu.b",	S_MLDV,		1,	0x5100	},

    {	NULL,	"shll",		S_ROSH,		0,	0x1000	},
    {	NULL,	"shal",		S_ROSH,		0,	0x1080	},
    {	NULL,	"shlr",		S_ROSH,		0,	0x1100	},
    {	NULL,	"shar",		S_ROSH,		0,	0x1180	},
    {	NULL,	"rotxl",	S_ROSH,		0,	0x1200	},
    {	NULL,	"rotl",		S_ROSH,		0,	0x1280	},
    {	NULL,	"rotxr",	S_ROSH,		0,	0x1300	},
    {	NULL,	"rotr",		S_ROSH,		0,	0x1380	},

    {	NULL,	"shll.b",	S_ROSH,		1,	0x1000	},
    {	NULL,	"shal.b",	S_ROSH,		1,	0x1080	},
    {	NULL,	"shlr.b",	S_ROSH,		1,	0x1100	},
    {	NULL,	"shar.b",	S_ROSH,		1,	0x1180	},
    {	NULL,	"rotxl.b",	S_ROSH,		1,	0x1200	},
    {	NULL,	"rotl.b",	S_ROSH,		1,	0x1280	},
    {	NULL,	"rotxr.b",	S_ROSH,		1,	0x1300	},
    {	NULL,	"rotr.b",	S_ROSH,		1,	0x1380	},

    {	NULL,	"pop",		S_PP,		0,	0x6D70	},
    {	NULL,	"push",		S_PP,		0,	0x6DF0	},

    {	NULL,	"pop.w",	S_PP,		2,	0x6D70	},
    {	NULL,	"push.w",	S_PP,		2,	0x6DF0	},

    {	NULL,	"movfpe",	S_MVFPE,	0,	0x6A40	},
    {	NULL,	"movtpe",	S_MVTPE,	0,	0x6AC0	},

    {	NULL,	"movfpe.b",	S_MVFPE,	0,	0x6A40	},
    {	NULL,	"movtpe.b",	S_MVTPE,	0,	0x6AC0	},

    {	NULL,	"eepmov",	S_EEPMOV,	0,	0x7B5C	},

    {	NULL,	"jmp",		S_JXX,		0,	0x5900	},
    {	NULL,	"jsr",		S_JXX,		0,	0x5D00	},

    {	NULL,	"bset",		S_BIT1,		0,	0x6000	},
    {	NULL,	"bnot",		S_BIT1,		0,	0x6100	},
    {	NULL,	"bclr",		S_BIT1,		0,	0x6200	},
    {	NULL,	"btst",		S_BIT1,		0,	0x6300	},

    {	NULL,	"bst",		S_BIT2,		0,	0x6700	},
    {	NULL,	"bist",		S_BIT2,		0,	0x6780	},
    {	NULL,	"bor",		S_BIT2,		0,	0x7400	},
    {	NULL,	"bior",		S_BIT2,		0,	0x7480	},
    {	NULL,	"bxor",		S_BIT2,		0,	0x7500	},
    {	NULL,	"bixor",	S_BIT2,		0,	0x7580	},
    {	NULL,	"band",		S_BIT2,		0,	0x7600	},
    {	NULL,	"biand",	S_BIT2,		0,	0x7680	},
    {	NULL,	"bld",		S_BIT2,		0,	0x7700	},
    {	NULL,	"bild",		S_BIT2,		0,	0x7780	},

    {	NULL,	"bra",		S_BRA,		0,	0x4000	},
    {	NULL,	"bt",		S_BRA,		0,	0x4000	},
    {	NULL,	"brn",		S_BRA,		0,	0x4100	},
    {	NULL,	"bf",		S_BRA,		0,	0x4100	},
    {	NULL,	"bhi",		S_BRA,		0,	0x4200	},
    {	NULL,	"bls",		S_BRA,		0,	0x4300	},
    {	NULL,	"blos",		S_BRA,		0,	0x4300	},
    {	NULL,	"bcc",		S_BRA,		0,	0x4400	},
    {	NULL,	"bhs",		S_BRA,		0,	0x4400	},
    {	NULL,	"bhis",		S_BRA,		0,	0x4400	},
    {	NULL,	"bcs",		S_BRA,		0,	0x4500	},
    {	NULL,	"blo",		S_BRA,		0,	0x4500	},
    {	NULL,	"bne",		S_BRA,		0,	0x4600	},
    {	NULL,	"beq",		S_BRA,		0,	0x4700	},
    {	NULL,	"bvc",		S_BRA,		0,	0x4800	},
    {	NULL,	"bvs",		S_BRA,		0,	0x4900	},
    {	NULL,	"bpl",		S_BRA,		0,	0x4A00	},
    {	NULL,	"bmi",		S_BRA,		0,	0x4B00	},
    {	NULL,	"bge",		S_BRA,		0,	0x4C00	},
    {	NULL,	"blt",		S_BRA,		0,	0x4D00	},
    {	NULL,	"bgt",		S_BRA,		0,	0x4E00	},
    {	NULL,	"ble",		S_BRA,		0,	0x4F00	},
    {	NULL,	"bsr",		S_BRA,		S_END,	0x5500	}
};
