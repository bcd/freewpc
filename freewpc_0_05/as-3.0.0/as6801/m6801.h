/* m6801.h */

/*
 * (C) Copyright 1989-2000
 * All Rights Reserved
 *
 * Alan R. Baldwin
 * 721 Berkeley St.
 * Kent, Ohio  44240
 */

/*)BUILD
	$(PROGRAM) =	AS6801
	$(INCLUDE) = {
		ASXXXX.H
		M6801.H
	}
	$(FILES) = {
		M01EXT.C
		M01MCH.C
		M01ADR.C
		M01PST.C
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
#define	S_IMMED	34
#define	S_DIR	35
#define	S_EXT	36
#define	S_INDX	37

/*
 * Instruction types
 */
#define	S_INH	60
#define	S_PUL	61
#define	S_BRA	62
#define	S_TYP1	63
#define	S_TYP2	64
#define	S_TYP3	65
#define	S_TYP4	66
#define	S_TYP5	67
#define S_INH63	68
#define	S_TYP63	69

/*
 * Set Direct Pointer
 */
#define	S_SDP	80

/*
 * HD6303 Option
 */
#define	S_HD63	81


	/* machine dependent functions */

#ifdef	OTHERSYSTEM

	/* m01adr.c */
extern	struct	adsym	abdx[];
extern	int		addr(struct expr *esp);
extern	int		admode(struct adsym *sp);
extern	int		any(int c, char *str);
extern	int		srch(char *str);

	/* m01mch.c */
extern	VOID		machine(struct mne *mp);
extern	int		mchpcr(struct expr *esp);
extern	VOID		minit(void);
extern	int		comma(void);

#else

	/* m01adr.c */
extern	struct	adsym	abdx[];
extern	int		addr();
extern	int		admode();
extern	int		any();
extern	int		srch();

	/* m01mch.c */
extern	VOID		machine();
extern	int		mchpcr();
extern	VOID		minit();
extern	int		comma();

#endif

