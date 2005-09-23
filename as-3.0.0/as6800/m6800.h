/* m6800.h */

/*
 * (C) Copyright 1989-2000
 * All Rights Reserved
 *
 * Alan R. Baldwin
 * 721 Berkeley St.
 * Kent, Ohio  44240
 */

/*)BUILD
	$(PROGRAM) =	AS6800
	$(INCLUDE) = {
		ASXXXX.H
		M6800.H
	}
	$(FILES) = {
		M00EXT.C
		M00MCH.C
		M00ADR.C
		M00PST.C
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
#define	S_X	32
#define	S_IMMED	33
#define	S_DIR	34
#define	S_EXT	35
#define	S_INDX	36

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

/*
 * Set Direct Pointer
 */
#define	S_SDP	80


	/* machine dependent functions */

#ifdef	OTHERSYSTEM

	/* m00adr.c */
extern	struct	adsym	abx[];
extern	int		addr(struct expr *esp);
extern	int		admode(struct adsym *sp);
extern	int		any(int c, char *str);
extern	int		srch(char *str);

	/* m00mch.c */
extern	VOID		machine(struct mne *mp);
extern	int		mchpcr(struct expr *esp);
extern	VOID		minit(void);
extern	int		comma(void);

#else

	/* m00adr.c */
extern	struct	adsym	abx[];
extern	int		addr();
extern	int		admode();
extern	int		any();
extern	int		srch();

	/* m00mch.c */
extern	VOID		machine();
extern	int		mchpcr();
extern	VOID		minit();
extern	int		comma();

#endif

