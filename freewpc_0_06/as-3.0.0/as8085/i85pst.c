/* i85pst.c */

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
#include "i8085.h"

struct	mne	mne[] = {

	/* machine */

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

	/* 8080/8085 */

    {	NULL,	"b",		S_REG,		0,	B	},
    {	NULL,	"c",		S_REG,		0,	C	},
    {	NULL,	"d",		S_REG,		0,	D	},
    {	NULL,	"e",		S_REG,		0,	E	},
    {	NULL,	"h",		S_REG,		0,	H	},
    {	NULL,	"l",		S_REG,		0,	L	},
    {	NULL,	"m",		S_REG,		0,	M	},
    {	NULL,	"a",		S_REG,		0,	A	},
    {	NULL,	"sp",		S_REG,		0,	SP	},
    {	NULL,	"psw",		S_REG,		0,	PSW	},

    {	NULL,	"xthl",		S_INH,		0,	0343	},
    {	NULL,	"sphl",		S_INH,		0,	0371	},
    {	NULL,	"pchl",		S_INH,		0,	0351	},
    {	NULL,	"xchg",		S_INH,		0,	0353	},
    {	NULL,	"ret",		S_INH,		0,	0311	},
    {	NULL,	"rc",		S_INH,		0,	0330	},
    {	NULL,	"rnc",		S_INH,		0,	0320	},
    {	NULL,	"rz",		S_INH,		0,	0310	},
    {	NULL,	"rnz",		S_INH,		0,	0300	},
    {	NULL,	"rp",		S_INH,		0,	0360	},
    {	NULL,	"rm",		S_INH,		0,	0370	},
    {	NULL,	"rpe",		S_INH,		0,	0350	},
    {	NULL,	"rpo",		S_INH,		0,	0340	},
    {	NULL,	"rim",		S_INH,		0,	0040	},
    {	NULL,	"rlc",		S_INH,		0,	0007	},
    {	NULL,	"rrc",		S_INH,		0,	0017	},
    {	NULL,	"ral",		S_INH,		0,	0027	},
    {	NULL,	"rar",		S_INH,		0,	0037	},
    {	NULL,	"cma",		S_INH,		0,	0057	},
    {	NULL,	"stc",		S_INH,		0,	0067	},
    {	NULL,	"cmc",		S_INH,		0,	0077	},
    {	NULL,	"daa",		S_INH,		0,	0047	},
    {	NULL,	"ei",		S_INH,		0,	0373	},
    {	NULL,	"di",		S_INH,		0,	0363	},
    {	NULL,	"nop",		S_INH,		0,	0000	},
    {	NULL,	"hlt",		S_INH,		0,	0166	},
    {	NULL,	"sim",		S_INH,		0,	0060	},

    {	NULL,	"rst",		S_RST,		0,	0307	},

    {	NULL,	"in",		S_ADI,		0,	0333	},
    {	NULL,	"out",		S_ADI,		0,	0323	},
    {	NULL,	"adi",		S_ADI,		0,	0306	},
    {	NULL,	"aci",		S_ADI,		0,	0316	},
    {	NULL,	"sui",		S_ADI,		0,	0326	},
    {	NULL,	"sbi",		S_ADI,		0,	0336	},
    {	NULL,	"ani",		S_ADI,		0,	0346	},
    {	NULL,	"xri",		S_ADI,		0,	0356	},
    {	NULL,	"ori",		S_ADI,		0,	0366	},
    {	NULL,	"cpi",		S_ADI,		0,	0376	},

    {	NULL,	"add",		S_ADD,		0,	0200	},
    {	NULL,	"adc",		S_ADD,		0,	0210	},
    {	NULL,	"sub",		S_ADD,		0,	0220	},
    {	NULL,	"sbb",		S_ADD,		0,	0230	},
    {	NULL,	"ana",		S_ADD,		0,	0240	},
    {	NULL,	"xra",		S_ADD,		0,	0250	},
    {	NULL,	"ora",		S_ADD,		0,	0260	},
    {	NULL,	"cmp",		S_ADD,		0,	0270	},

    {	NULL,	"sta",		S_JMP,		0,	0062	},
    {	NULL,	"lda",		S_JMP,		0,	0072	},
    {	NULL,	"shld",		S_JMP,		0,	0042	},
    {	NULL,	"lhld",		S_JMP,		0,	0052	},
    {	NULL,	"jmp",		S_JMP,		0,	0303	},
    {	NULL,	"jc",		S_JMP,		0,	0332	},
    {	NULL,	"jnc",		S_JMP,		0,	0322	},
    {	NULL,	"jz",		S_JMP,		0,	0312	},
    {	NULL,	"jnz",		S_JMP,		0,	0302	},
    {	NULL,	"jp",		S_JMP,		0,	0362	},
    {	NULL,	"jm",		S_JMP,		0,	0372	},
    {	NULL,	"jpe",		S_JMP,		0,	0352	},
    {	NULL,	"jpo",		S_JMP,		0,	0342	},
    {	NULL,	"call",		S_JMP,		0,	0315	},
    {	NULL,	"cc",		S_JMP,		0,	0334	},
    {	NULL,	"cnc",		S_JMP,		0,	0324	},
    {	NULL,	"cz",		S_JMP,		0,	0314	},
    {	NULL,	"cnz",		S_JMP,		0,	0304	},
    {	NULL,	"cp",		S_JMP,		0,	0364	},
    {	NULL,	"cm",		S_JMP,		0,	0374	},
    {	NULL,	"cpe",		S_JMP,		0,	0354	},
    {	NULL,	"cpo",		S_JMP,		0,	0344	},

    {	NULL,	"inr",		S_INR,		0,	0004	},
    {	NULL,	"dcr",		S_INR,		0,	0005	},

    {	NULL,	"lxi",		S_LXI,		0,	0001	},

    {	NULL,	"ldax",		S_LDAX,		0,	0012	},
    {	NULL,	"stax",		S_LDAX,		0,	0002	},

    {	NULL,	"inx",		S_INX,		0,	0003	},
    {	NULL,	"dcx",		S_INX,		0,	0013	},
    {	NULL,	"dad",		S_INX,		0,	0011	},

    {	NULL,	"push",		S_PUSH,		0,	0305	},
    {	NULL,	"pop",		S_PUSH,		0,	0301	},

    {	NULL,	"mov",		S_MOV,		0,	0100	},

    {	NULL,	"mvi",		S_MVI,		S_END,	0006	}
};
