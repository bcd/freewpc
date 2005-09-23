/* xxxpst.c */

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
#include "yyyy.h"

struct	mne	mne[] = {

	/* machine */

    {	NULL,	".setdp",	S_SDP,		0,	0	},/* 68xx Machines */

	/* system */

    {	NULL,	"CON",		S_ATYP,		0,	A_CON	},
    {	NULL,	"OVR",		S_ATYP,		0,	A_OVR	},
    {	NULL,	"REL",		S_ATYP,		0,	A_REL	},
    {	NULL,	"ABS",		S_ATYP,		0,  A_ABS|A_OVR	},
    {	NULL,	"NOPAG",	S_ATYP,		0,	A_NOPAG	},
    {	NULL,	"PAG",		S_ATYP,		0,	A_PAG	},

    {	NULL,	".byte",	S_BYTE,		0,	0	},
    {	NULL,	".db",		S_BYTE,		0,	0	},
    {	NULL,	".word",	S_WORD,		0,	0	},
    {	NULL,	".dw",		S_WORD,		0,	0	},
    {	NULL,	".3byte",	S_3BYTE,	0,	0	},/* >= 24-Bit Addressing */
    {	NULL,	".4byte",	S_4BYTE,	0,	0	},/*    32-Bit Addressing */
    {	NULL,	".quad",	S_4BYTE,	0,	0	},/*    32-Bit Addressing */
    {	NULL,	".ascii",	S_ASCII,	0,	0	},
    {	NULL,	".asciz",	S_ASCIZ,	0,	0	},
    {	NULL,	".blkb",	S_BLK,		0,	1	},
    {	NULL,	".ds",		S_BLK,		0,	1	},
    {	NULL,	".blkw",	S_BLK,		0,	2	},
    {	NULL,	".blk3",	S_BLK,		0,	3	},
    {	NULL,	".blk4",	S_BLK,		0,	4	},
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
    {	NULL,	".16bit",	S_BITS,		0,	2	},/* 16-Bit Addressing */
    {	NULL,	".24bit",	S_BITS,		0,	3	},/* 24-Bit Addressing */
    {	NULL,	".32bit",	S_BITS,		0,	4	},/* 32-Bit Addressing */

	/* machine */

    {	NULL,	"?",		?,		S_END,	?	} /* End of List */
};
