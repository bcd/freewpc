/* m6811.h */

/*
 * (C) Copyright 1989-2000
 * All Rights Reserved
 *
 * Alan R. Baldwin
 * 721 Berkeley St.
 * Kent, Ohio  44240
 */

/*)BUILD
	$(PROGRAM) =	AS6811
	$(INCLUDE) = {
		ASXXXX.H
		M6811.H
	}
	$(FILES) = {
		M11EXT.C
		M11MCH.C
		M11ADR.C
		M11PST.C
		ASMAIN.C
		ASLEX.C
		ASSYM.C
		ASSUBR.C
		ASEXPR.C
		ASDATA.C
		ASLIST.C
		ASOUT.C
	}
	$(STACK) = 3000
*/

struct adsym
{
	char	a_str[2];	/* addressing string */
	int	a_val;		/* addressing mode value */
};

/*
 * Registers
 */
#define	A	0x00
#define	B	0x10
#define	X	0x20

/*
 * Addressing types
 */
#define	S_A	30
#define	S_B	31
#define	S_D	32
#define	S_X	33
#define	S_Y	34
#define	S_IMMED	35
#define	S_DIR	36
#define	S_EXT	37
#define	S_INDX	38
#define	S_INDY	39

/*
 * Instruction types
 */
#define	S_INH	60
#define	S_INH2	61
#define	S_PUL	62
#define	S_BRA	63
#define	S_TYP1	64
#define	S_TYP2	65
#define	S_TYP3	66
#define	S_TYP4	67
#define	S_TYP5	68
#define	S_TYP6	69
#define	S_PG2	70
#define	S_PG3	71
#define	S_STCLR	72
#define	S_BTB	73

/*
 * Set Direct Pointer
 */
#define	S_SDP	80

/*
 * Pages
 */
#define	PAGE1	0x00
#define	PAGE2	0x18
#define	PAGE3	0x1A
#define	PAGE4	0xCD


	/* machine dependent functions */

#ifdef	OTHERSYSTEM
	
	/* m11adr.c */
extern	struct	adsym	abdxy[];
extern	int		addr(struct expr *esp);
extern	int		admode(struct adsym *sp);
extern	int		any(int c, char *str);
extern	int		srch(char *str);

	/* m11mch.c */
extern	VOID		machine(struct mne *mp);
extern	int		mchpcr(struct expr *esp);
extern	VOID		minit(void);
extern	int		comma(void);

#else

	/* m11adr.c */
extern	struct	adsym	abdxy[];
extern	int		addr();
extern	int		admode();
extern	int		any();
extern	int		srch();

	/* m11mch.c */
extern	VOID		machine();
extern	int		mchpcr();
extern	VOID		minit();
extern	int		comma();

#endif

