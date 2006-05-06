/* m12pst.c */

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
#include "m6812.h"

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

	/* 6811 Compatibility */

	/* If this is changed, change struct opdata mc6811[] */

    {	NULL,	"abx",		S_6811,		0,	0	},
    {	NULL,	"aby",		S_6811,		0,	1	},
    {	NULL,	"clc",		S_6811,		0,	2	},
    {	NULL,	"cli",		S_6811,		0,	3	},
    {	NULL,	"clv",		S_6811,		0,	4	},
    {	NULL,	"des",		S_6811,		0,	5	},
    {	NULL,	"ins",		S_6811,		0,	6	},
    {	NULL,	"sec",		S_6811,		0,	7	},
    {	NULL,	"sei",		S_6811,		0,	8	},
    {	NULL,	"sev",		S_6811,		0,	9	},
    {	NULL,	"tap",		S_6811,		0,	10	},
    {	NULL,	"tpa",		S_6811,		0,	11	},
    {	NULL,	"tsx",		S_6811,		0,	12	},
    {	NULL,	"tsy",		S_6811,		0,	13	},
    {	NULL,	"txs",		S_6811,		0,	14	},
    {	NULL,	"tys",		S_6811,		0,	15	},
    {	NULL,	"xgdx",		S_6811,		0,	16	},
    {	NULL,	"xgdy",		S_6811,		0,	17	},

	/* 6812 */

	/* Relative Branches */
	/* op rr */

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
    {	NULL,	"bsr",		S_BRA,		0,	0x07	},

	/* Relative Long Branches */
	/* 18 op qq rr */

    {	NULL,	"lbra",		S_LBRA,		0,	0x20	},
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

	/* Inc/Dec/Tst Register and Branch Instructions */
	/* op lb rr */

    {	NULL,	"dbeq",		S_XBRA,		0,	0x00	},
    {	NULL,	"dbne",		S_XBRA,		0,	0x20	},
    {	NULL,	"ibeq",		S_XBRA,		0,	0x80	},
    {	NULL,	"ibne",		S_XBRA,		0,	0xA0	},
    {	NULL,	"tbeq",		S_XBRA,		0,	0x40	},
    {	NULL,	"tbne",		S_XBRA,		0,	0x60	},

	/* Single Operand - SOP		*/
	/* EXT		op hh ll	*/
	/* IDX		op xb		*/
	/* IDX1		op xb ff	*/
	/* IDX2		op xb ee ff	*/
	/* [D,IDX]	op xb		*/
	/* [IDX2]	op xb ee ff	*/

    {	NULL,	"neg",		S_SOP,		0,	0x60	},
    {	NULL,	"com",		S_SOP,		0,	0x61	},
    {	NULL,	"inc",		S_SOP,		0,	0x62	},
    {	NULL,	"dec",		S_SOP,		0,	0x63	},
    {	NULL,	"lsr",		S_SOP,		0,	0x64	},
    {	NULL,	"rol",		S_SOP,		0,	0x65	},
    {	NULL,	"ror",		S_SOP,		0,	0x66	},
    {	NULL,	"asr",		S_SOP,		0,	0x67	},
    {	NULL,	"asl",		S_SOP,		0,	0x68	},
    {	NULL,	"lsl",		S_SOP,		0,	0x68	},
    {	NULL,	"clr",		S_SOP,		0,	0x69	},
    {	NULL,	"tst",		S_SOP,		0,	0xE7	},

	/* Double Operand - DOP		*/
	/* IMM		op ii		*/
	/* DIR		op dd		*/
	/* EXT		op hh ll	*/
	/* IDX		op xb		*/
	/* IDX1		op xb ff	*/
	/* IDX2		op xb ee ff	*/
	/* [D,IDX]	op xb		*/
	/* [IDX2]	op xb ee ff	*/

    {	NULL,	"suba",		S_DOP,		0,	0x80	},
    {	NULL,	"subb",		S_DOP,		0,	0xC0	},
    {	NULL,	"cmpa",		S_DOP,		0,	0x81	},
    {	NULL,	"cmpb",		S_DOP,		0,	0xC1	},
    {	NULL,	"sbca",		S_DOP,		0,	0x82	},
    {	NULL,	"sbcb",		S_DOP,		0,	0xC2	},
    {	NULL,	"anda",		S_DOP,		0,	0x84	},
    {	NULL,	"andb",		S_DOP,		0,	0xC4	},
    {	NULL,	"bita",		S_DOP,		0,	0x85	},
    {	NULL,	"bitb",		S_DOP,		0,	0xC5	},
    {	NULL,	"lda",		S_DOP,		0,	0x86	},
    {	NULL,	"ldaa",		S_DOP,		0,	0x86	},
    {	NULL,	"ldb",		S_DOP,		0,	0xC6	},
    {	NULL,	"ldab",		S_DOP,		0,	0xC6	},
    {	NULL,	"eora",		S_DOP,		0,	0x88	},
    {	NULL,	"eorb",		S_DOP,		0,	0xC8	},
    {	NULL,	"adca",		S_DOP,		0,	0x89	},
    {	NULL,	"adcb",		S_DOP,		0,	0xC9	},
    {	NULL,	"ora",		S_DOP,		0,	0x8A	},
    {	NULL,	"oraa",		S_DOP,		0,	0x8A	},
    {	NULL,	"orb",		S_DOP,		0,	0xCA	},
    {	NULL,	"orab",		S_DOP,		0,	0xCA	},
    {	NULL,	"adda",		S_DOP,		0,	0x8B	},
    {	NULL,	"addb",		S_DOP,		0,	0xCB	},

	/* Store Instructions - STR	*/
	/* DIR		op dd		*/
	/* EXT		op hh ll	*/
	/* IDX		op xb		*/
	/* IDX1		op xb ff	*/
	/* IDX2		op xb ee ff	*/
	/* [D,IDX]	op xb		*/
	/* [IDX2]	op xb ee ff	*/

    {	NULL,	"staa",		S_STR,		0,	0x4A	},
    {	NULL,	"sta",		S_STR,		0,	0x4A	},
    {	NULL,	"stab",		S_STR,		0,	0x4B	},
    {	NULL,	"stb",		S_STR,		0,	0x4B	},
    {	NULL,	"std",		S_STR,		0,	0x4C	},
    {	NULL,	"sty",		S_STR,		0,	0x4D	},
    {	NULL,	"stx",		S_STR,		0,	0x4E	},
    {	NULL,	"sts",		S_STR,		0,	0x4F	},

	/* Long Register Operand - LONG	*/
	/* IMM		op jj kk	*/
	/* DIR		op dd		*/
	/* EXT		op hh ll	*/
	/* IDX		op xb		*/
	/* IDX1		op xb ff	*/
	/* IDX2		op xb ee ff	*/
	/* [D,IDX]	op xb		*/
	/* [IDX2]	op xb ee ff	*/

    {	NULL,	"addd",		S_LONG,		0,	0xC3	},
    {	NULL,	"subd",		S_LONG,		0,	0x83	},
    {	NULL,	"cpd",		S_LONG,		0,	0x8C	},
    {	NULL,	"cmpd",		S_LONG,		0,	0x8C	},
    {	NULL,	"cpy",		S_LONG,		0,	0x8D	},
    {	NULL,	"cmpy",		S_LONG,		0,	0x8D	},
    {	NULL,	"cpx",		S_LONG,		0,	0x8E	},
    {	NULL,	"cmpx",		S_LONG,		0,	0x8E	},
    {	NULL,	"cps",		S_LONG,		0,	0x8F	},
    {	NULL,	"cmps",		S_LONG,		0,	0x8F	},
    {	NULL,	"ldd",		S_LONG,		0,	0xCC	},
    {	NULL,	"ldy",		S_LONG,		0,	0xCD	},
    {	NULL,	"ldx",		S_LONG,		0,	0xCE	},
    {	NULL,	"lds",		S_LONG,		0,	0xCF	},

	/* JMP and JSR Instructions	*/
	/* DIR		op dd		*/
	/* EXT		op hh ll	*/
	/* IDX		op xb		*/
	/* IDX1		op xb ff	*/
	/* IDX2		op xb ee ff	*/
	/* [D,IDX]	op xb		*/
	/* [IDX2]	op xb ee ff	*/
	/* ---------------------------- */
	/* JMP DIR	ILLEGAL		*/

    {	NULL,	"jmp",		S_JMP,		0,	0x05	},
    {	NULL,	"jsr",		S_JSR,		0,	0x15	},

	/* CALL Instruction		*/
	/* EXT		op hh ll pg	*/
	/* IDX		op xb pg	*/
	/* IDX1		op xb ff pg	*/
	/* IDX2		op xb ee ff pg	*/
	/* [D,IDX]	op xb		*/
	/* [IDX2]	op xb ee ff	*/

    {	NULL,	"call",		S_CALL,		0,	0x4A	},

	/* Load Effective Address Instructions	*/
	/* IDX		op xb			*/
	/* ID1		op xb ff		*/
	/* ID2		op xb ee ff		*/

    {	NULL,	"leay",		S_LEA,		0,	0x19	},
    {	NULL,	"leax",		S_LEA,		0,	0x1A	},
    {	NULL,	"leas",		S_LEA,		0,	0x1B	},

	/* EMACS Instruction		*/
	/* EXT		18 op hh ll	*/

    {	NULL,	"emacs",	S_EMACS,	0,	0x12	},

	/* Min / Max Operations 	*/
	/* IDX		op xb		*/
	/* IDX1		op xb ff	*/
	/* IDX2		op xb ee ff	*/
	/* [D,IDX]	op xb		*/
	/* [IDX2]	op xb ee ff	*/

    {	NULL,	"maxa",		S_EMNMX,	0,	0x18	},
    {	NULL,	"mina",		S_EMNMX,	0,	0x19	},
    {	NULL,	"maxm",		S_EMNMX,	0,	0x1C	},
    {	NULL,	"minm",		S_EMNMX,	0,	0x1D	},
    {	NULL,	"emaxd",	S_EMNMX,	0,	0x1A	},
    {	NULL,	"emind",	S_EMNMX,	0,	0x1B	},
    {	NULL,	"emaxm",	S_EMNMX,	0,	0x1E	},
    {	NULL,	"eminm",	S_EMNMX,	0,	0x1F	},

	/* Move byte/word Operations 		*/
	/* IMM - EXT	18 op ii hh ll		*/
	/* IMM - IDX	18 op xb ii		*/
	/* EXT - EXT	18 op hh ll hh ll	*/
	/* EXT - IDX	18 op xb hh ll		*/
	/* IDX - EXT	18 op xb hh ll		*/
	/* IDX - IDX	18 op xb xb		*/

    {	NULL,	"movb",		S_MOVB,		0,	0x08	},
    {	NULL,	"movw",		S_MOVW,		0,	0x00	},

	/* Register <--> Register transfer instructions	*/
	/* INH		op eb				*/

    {	NULL,	"sex",		S_SEX,		0,	0xB7	},
    {	NULL,	"tfr",		S_TFR,		0,	0xB7	},
    {	NULL,	"exg",		S_EXG,		0,	0xB7	},

	/* Table Lookup Instructions	*/
	/* IDX		18 op xb	*/

    {	NULL,	"tbl",		S_TBL,		0,	0x3D	},
    {	NULL,	"etbl",		S_TBL,		0,	0x3F	},

	/* TRAP Instructions		*/
	/* INH		18 0x30 - 0x39	*/
	/* INH		18 0x40 - 0xFF	*/

    {	NULL,	"trap",		S_TRAP,		0,	0x18	},

	/* Bit SET / CLR Instructions	*/
	/* DIR		op dd mm	*/
	/* EXT		op hh ll mm	*/
	/* IDX		op xb mm	*/
	/* IDX1		op xb ff mm	*/
	/* IDX2		op xb ee ff mm	*/

    {	NULL,	"bset",		S_BIT,		0,	0x0C	},
    {	NULL,	"bclr",		S_BIT,		0,	0x0D	},

	/* Branch on				*/
	/* Bit SET / CLR Instructions		*/
	/* DIR		op dd mm rr		*/
	/* EXT		op hh ll mm rr		*/
	/* IDX		op xb mm rr		*/
	/* IDX1		op xb ff mm rr		*/
	/* IDX2		op xb ee ff mm rr	*/

    {	NULL,	"brset",	S_BRBIT,	0,	0x0E	},
    {	NULL,	"brclr",	S_BRBIT,	0,	0x0F	},

	/* Condition Code Instructions	*/
	/* IMM		op ii		*/

    {	NULL,	"andcc",	S_CC,		0,	0x10	},
    {	NULL,	"orcc",		S_CC,		0,	0x14	},

	/* Page1 Inherent Instructions	*/
	/* INH		op		*/

    {	NULL,	"asla",		S_INH,		0,	0x48	},
    {	NULL,	"aslb",		S_INH,		0,	0x58	},
    {	NULL,	"asld",		S_INH,		0,	0x59	},
    {	NULL,	"asra",		S_INH,		0,	0x47	},
    {	NULL,	"asrb",		S_INH,		0,	0x57	},
    {	NULL,	"bgnd",		S_INH,		0,	0x00	},
    {	NULL,	"clra",		S_INH,		0,	0x87	},
    {	NULL,	"clrb",		S_INH,		0,	0xC7	},
    {	NULL,	"coma",		S_INH,		0,	0x41	},
    {	NULL,	"comb",		S_INH,		0,	0x51	},
    {	NULL,	"deca",		S_INH,		0,	0x43	},
    {	NULL,	"decb",		S_INH,		0,	0x53	},
    {	NULL,	"dex",		S_INH,		0,	0x09	},
    {	NULL,	"dey",		S_INH,		0,	0x03	},
    {	NULL,	"ediv",		S_INH,		0,	0x11	},
    {	NULL,	"emul",		S_INH,		0,	0x13	},
    {	NULL,	"inca",		S_INH,		0,	0x42	},
    {	NULL,	"incb",		S_INH,		0,	0x52	},
    {	NULL,	"inx",		S_INH,		0,	0x08	},
    {	NULL,	"iny",		S_INH,		0,	0x02	},
    {	NULL,	"lsla",		S_INH,		0,	0x48	},
    {	NULL,	"lslb",		S_INH,		0,	0x58	},
    {	NULL,	"lsld",		S_INH,		0,	0x59	},
    {	NULL,	"lsra",		S_INH,		0,	0x44	},
    {	NULL,	"lsrb",		S_INH,		0,	0x54	},
    {	NULL,	"lsrd",		S_INH,		0,	0x49	},
    {	NULL,	"mem",		S_INH,		0,	0x01	},
    {	NULL,	"mul",		S_INH,		0,	0x12	},
    {	NULL,	"nega",		S_INH,		0,	0x40	},
    {	NULL,	"negb",		S_INH,		0,	0x50	},
    {	NULL,	"nop",		S_INH,		0,	0xA7	},
    {	NULL,	"psha",		S_INH,		0,	0x36	},
    {	NULL,	"pshb",		S_INH,		0,	0x37	},
    {	NULL,	"pshc",		S_INH,		0,	0x39	},
    {	NULL,	"pshd",		S_INH,		0,	0x3B	},
    {	NULL,	"pshx",		S_INH,		0,	0x34	},
    {	NULL,	"pshy",		S_INH,		0,	0x35	},
    {	NULL,	"pula",		S_INH,		0,	0x32	},
    {	NULL,	"pulb",		S_INH,		0,	0x33	},
    {	NULL,	"pulc",		S_INH,		0,	0x38	},
    {	NULL,	"puld",		S_INH,		0,	0x3A	},
    {	NULL,	"pulx",		S_INH,		0,	0x30	},
    {	NULL,	"puly",		S_INH,		0,	0x31	},
    {	NULL,	"rola",		S_INH,		0,	0x45	},
    {	NULL,	"rolb",		S_INH,		0,	0x55	},
    {	NULL,	"rora",		S_INH,		0,	0x46	},
    {	NULL,	"rorb",		S_INH,		0,	0x56	},
    {	NULL,	"rtc",		S_INH,		0,	0x0A	},
    {	NULL,	"rti",		S_INH,		0,	0x0B	},
    {	NULL,	"rts",		S_INH,		0,	0x3D	},
    {	NULL,	"swi",		S_INH,		0,	0x3F	},
    {	NULL,	"tsta",		S_INH,		0,	0x97	},
    {	NULL,	"tstb",		S_INH,		0,	0xD7	},
    {	NULL,	"wai",		S_INH,		0,	0x3E	},
    {	NULL,	"wavr",		S_INH,		0,	0x3C	},

	/* Page2 Inherent Instructions	*/
	/* INH		18 op		*/

    {	NULL,	"aba",		S_INH2,		0,	0x06	},
    {	NULL,	"cba",		S_INH2,		0,	0x17	},
    {	NULL,	"daa",		S_INH2,		0,	0x07	},
    {	NULL,	"edivs",	S_INH2,		0,	0x14	},
    {	NULL,	"emuls",	S_INH2,		0,	0x13	},
    {	NULL,	"fdiv",		S_INH2,		0,	0x11	},
    {	NULL,	"idiv",		S_INH2,		0,	0x10	},
    {	NULL,	"idivs",	S_INH2,		0,	0x15	},
    {	NULL,	"rev",		S_INH2,		0,	0x3A	},
    {	NULL,	"revw",		S_INH2,		0,	0x3B	},
    {	NULL,	"sba",		S_INH2,		0,	0x16	},
    {	NULL,	"stop",		S_INH2,		0,	0x3E	},
    {	NULL,	"tab",		S_INH2,		0,	0x0E	},
    {	NULL,	"tba",		S_INH2,		0,	0x0F	},
    {	NULL,	"wav",		S_INH2,		0,	0x3C	},

	/* Alternate PSH / PUL Form	*/
	/* INH		op		*/

    {	NULL,	"pul",		S_PUL,		0,	0x30	},
    {	NULL,	"psh",		S_PSH,		S_END,	0x30	},
};

struct opdata mc6811[] = {

    {{	(char) 0x1A, (char) 0xE5,	/*	leax	b,x	;abx	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0x19, (char) 0xED,	/*	leay	b,y	;aby	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0x10, (char) 0xFE,	/*	andcc	#0xFE	;clc	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0x10, (char) 0xEF,	/*	andcc	#0xEF	;cli	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0x10, (char) 0xFD,	/*	andcc	#0xFD	;clv	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0x1B, (char) 0x9F,	/*	leas	-1,s	;des	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0x1B, (char) 0x81,	/*	leas	1,s	;ins	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0x14, (char) 0x01,	/*	orcc	#0x01	;sec	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0x14, (char) 0x10,	/*	orcc	#0x10	;sei	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0x14, (char) 0x02,	/*	orcc	#0x02	;sev	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0xB7, (char) 0x02,	/*	tfr	a,cc	;tap	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0xB7, (char) 0x20,	/*	tfr	cc,a	;tpa	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0xB7, (char) 0x75,	/*	tfr	s,x	;tsx	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0xB7, (char) 0x76,	/*	tfr	s,y	;tsy	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0xB7, (char) 0x57,	/*	tfr	x,s	;txs	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0xB7, (char) 0x67,	/*	tfr	y,s	;tys	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0xB7, (char) 0xC5,	/*	exg	d,x	;xgdx	*/
	(char) 0x00, (char) 0x00	}},

    {{	(char) 0xB7, (char) 0xC6,	/*	exg	d,y	;xgdy	*/
	(char) 0x00, (char) 0x00	}}
};
