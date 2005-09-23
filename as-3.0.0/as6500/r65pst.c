/* r65pst.c */

/*
 * (C) Copyright 1995-2000
 * All Rights Reserved
 *
 * Alan R. Baldwin
 * 721 Berkeley St.
 * Kent, Ohio  44240
 */

/*
 * With Contributions from
 *
 * Marko Makela
 * Sillitie 10 A
 * 01480 Vantaa
 * Finland
 * Internet: Marko.Makela@Helsinki.Fi
 * EARN/BitNet: msmakela@finuh
 */

#include <stdio.h>
#include <setjmp.h>
#include "asxxxx.h"
#include "r6500.h"

struct	mne	mne[] = {

	/* machine */

    {	NULL,	".setdp",	S_SDP,		0,	0	},
    {	NULL,	".r6500",	S_R6500,	0,	0	},
    {	NULL,	".r65f11",	S_R65F11,	0,	0	},
    {	NULL,	".r65c00",	S_R65C00,	0,	0	},
    {	NULL,	".r65c02",	S_R65C02,	0,	0	},

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

	/* 650X / 651X Family  Instructions */

    {	NULL,	"adc",		S_DOP,		0,	0x60	},
    {	NULL,	"and",		S_DOP,		0,	0x20	},
    {	NULL,	"cmp",		S_DOP,		0,	0xC0	},
    {	NULL,	"eor",		S_DOP,		0,	0x40	},
    {	NULL,	"lda",		S_DOP,		0,	0xA0	},
    {	NULL,	"ora",		S_DOP,		0,	0x00	},
    {	NULL,	"sbc",		S_DOP,		0,	0xE0	},
    {	NULL,	"sta",		S_DOP,		0,	0x80	},

    {	NULL,	"asl",		S_SOP,		0,	0x00	},
    {	NULL,	"lsr",		S_SOP,		0,	0x40	},
    {	NULL,	"rol",		S_SOP,		0,	0x20	},
    {	NULL,	"ror",		S_SOP,		0,	0x60	},
    {	NULL,	"dec",		S_SOP,		0,	0xC0	},
    {	NULL,	"inc",		S_SOP,		0,	0xE0	},

    {	NULL,	"bpl",		S_BRA1,		0,	0x10	},
    {	NULL,	"bmi",		S_BRA1,		0,	0x30	},
    {	NULL,	"bvc",		S_BRA1,		0,	0x50	},
    {	NULL,	"bvs",		S_BRA1,		0,	0x70	},
    {	NULL,	"bcc",		S_BRA1,		0,	0x90	},
    {	NULL,	"bhs",		S_BRA1,		0,	0x90	},
    {	NULL,	"bcs",		S_BRA1,		0,	0xB0	},
    {	NULL,	"blo",		S_BRA1,		0,	0xB0	},
    {	NULL,	"bne",		S_BRA1,		0,	0xD0	},
    {	NULL,	"beq",		S_BRA1,		0,	0xF0	},

    {	NULL,	"bit",		S_BIT,		0,	0x20	},

    {	NULL,	"brk",		S_INH1,		0,	0x00	},
    {	NULL,	"clc",		S_INH1,		0,	0x18	},
    {	NULL,	"cld",		S_INH1,		0,	0xD8	},
    {	NULL,	"cli",		S_INH1,		0,	0x58	},
    {	NULL,	"clv",		S_INH1,		0,	0xB8	},
    {	NULL,	"dex",		S_INH1,		0,	0xCA	},
    {	NULL,	"dey",		S_INH1,		0,	0x88	},
    {	NULL,	"inx",		S_INH1,		0,	0xE8	},
    {	NULL,	"iny",		S_INH1,		0,	0xC8	},
    {	NULL,	"nop",		S_INH1,		0,	0xEA	},
    {	NULL,	"pha",		S_INH1,		0,	0x48	},
    {	NULL,	"php",		S_INH1,		0,	0x08	},
    {	NULL,	"pla",		S_INH1,		0,	0x68	},
    {	NULL,	"plp",		S_INH1,		0,	0x28	},
    {	NULL,	"rti",		S_INH1,		0,	0x40	},
    {	NULL,	"rts",		S_INH1,		0,	0x60	},
    {	NULL,	"sec",		S_INH1,		0,	0x38	},
    {	NULL,	"sed",		S_INH1,		0,	0xF8	},
    {	NULL,	"sei",		S_INH1,		0,	0x78	},
    {	NULL,	"tax",		S_INH1,		0,	0xAA	},
    {	NULL,	"tay",		S_INH1,		0,	0xA8	},
    {	NULL,	"tsx",		S_INH1,		0,	0xBA	},
    {	NULL,	"txa",		S_INH1,		0,	0x8A	},
    {	NULL,	"txs",		S_INH1,		0,	0x9A	},
    {	NULL,	"tya",		S_INH1,		0,	0x98	},

    {	NULL,	"cpx",		S_CP,		0,	0xE0	},
    {	NULL,	"cpy",		S_CP,		0,	0xC0	},

    {	NULL,	"ldx",		S_LDSTX,	0,	0xA0	},
    {	NULL,	"stx",		S_LDSTX,	0,	0x80	},

    {	NULL,	"ldy",		S_LDSTY,	0,	0xA0	},
    {	NULL,	"sty",		S_LDSTY,	0,	0x80	},

    {	NULL,	"jmp",		S_JMP,		0,	0x4C	},

    {	NULL,	"jsr",		S_JSR,		0,	0x20	},

	/* Additional R65F1X series Instructions */
	
    {	NULL,	"bbr0",		S_BB,		0,	0x0F	},
    {	NULL,	"bbr1",		S_BB,		0,	0x1F	},
    {	NULL,	"bbr2",		S_BB,		0,	0x2F	},
    {	NULL,	"bbr3",		S_BB,		0,	0x3F	},
    {	NULL,	"bbr4",		S_BB,		0,	0x4F	},
    {	NULL,	"bbr5",		S_BB,		0,	0x5F	},
    {	NULL,	"bbr6",		S_BB,		0,	0x6F	},
    {	NULL,	"bbr7",		S_BB,		0,	0x7F	},
    {	NULL,	"bbs0",		S_BB,		0,	0x8F	},
    {	NULL,	"bbs1",		S_BB,		0,	0x9F	},
    {	NULL,	"bbs2",		S_BB,		0,	0xAF	},
    {	NULL,	"bbs3",		S_BB,		0,	0xBF	},
    {	NULL,	"bbs4",		S_BB,		0,	0xCF	},
    {	NULL,	"bbs5",		S_BB,		0,	0xDF	},
    {	NULL,	"bbs6",		S_BB,		0,	0xEF	},
    {	NULL,	"bbs7",		S_BB,		0,	0xFF	},

    {	NULL,	"rmb0",		S_MB,		0,	0x07	},
    {	NULL,	"rmb1",		S_MB,		0,	0x17	},
    {	NULL,	"rmb2",		S_MB,		0,	0x27	},
    {	NULL,	"rmb3",		S_MB,		0,	0x37	},
    {	NULL,	"rmb4",		S_MB,		0,	0x47	},
    {	NULL,	"rmb5",		S_MB,		0,	0x57	},
    {	NULL,	"rmb6",		S_MB,		0,	0x67	},
    {	NULL,	"rmb7",		S_MB,		0,	0x77	},
    {	NULL,	"smb0",		S_MB,		0,	0x87	},
    {	NULL,	"smb1",		S_MB,		0,	0x97	},
    {	NULL,	"smb2",		S_MB,		0,	0xA7	},
    {	NULL,	"smb3",		S_MB,		0,	0xB7	},
    {	NULL,	"smb4",		S_MB,		0,	0xC7	},
    {	NULL,	"smb5",		S_MB,		0,	0xD7	},
    {	NULL,	"smb6",		S_MB,		0,	0xE7	},
    {	NULL,	"smb7",		S_MB,		0,	0xF7	},
	
	/* Additional R65C00 series Instructions */
	
    {	NULL,	"bra",		S_BRA2,		0,	0x80	},
	
    {	NULL,	"phx",		S_INH2,		0,	0xDA	},
    {	NULL,	"phy",		S_INH2,		0,	0x5A	},
    {	NULL,	"plx",		S_INH2,		0,	0xFA	},
    {	NULL,	"ply",		S_INH2,		0,	0x7A	},

    {	NULL,	"mul",		S_INH3,		0,	0x02	},

	/* Additional R65C02 series Instructions */

    {	NULL,	"stz",		S_STZ,		0,	0x60	},

    {	NULL,	"trb",		S_TB,		0,	0x10	},
    {	NULL,	"tsb",		S_TB,		S_END,	0x00	}
};
