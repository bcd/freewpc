/* m6816.h */

/*
 * (C) Copyright 1991-2000
 * All Rights Reserved
 *
 * Alan R. Baldwin
 * 721 Berkeley St.
 * Kent, Ohio  44240
 */

/*)BUILD
	$(PROGRAM) =	AS6816
	$(INCLUDE) = {
		ASXXXX.H
		M6816.H
	}
	$(FILES) = {
		M16EXT.C
		M16MCH.C
		M16ADR.C
		M16PST.C
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

/*
 * Paging Values
 */
#define	PAGE1	0x17
#define	PAGE2	0x27
#define	PAGE3	0x37

/*
 * Addressing types
 */
#define	T_IMM	0x01
#define	T_EXT	0x02
#define	T_INDX	0x04
#define	T_E_I	0x08
#define	T_IM8	0x10
#define	T_IM16	0x20
#define	T_IND8	0x40
#define	T_IND16	0x80

/*
 * 6816 Instruction types
 */
#define	S_IMMA	40
#define	S_IM16	41
#define	S_BIT	42
#define	S_BITW	43
#define	S_BRBT	44
#define	S_LDED	45
#define	S_MAC	46
#define S_PSHM	47
#define	S_PULM	48
#define	S_JXX	49
#define	S_MOVB	50
#define	S_MOVW	51
#define	S_CMP	52
#define	S_STOR	53
#define	S_LOAD	54
#define	S_SOPW	55
#define	S_SOP	56
#define	S_DOPE	57
#define	S_DOPD	58
#define	S_DOP	59
#define	S_INH27	60
#define	S_INH37	61
#define	S_LBRA	62
#define	S_LBSR	63
#define	S_BRA	64
#define S_BSR	65

/*
 * Set Direct Pointer
 */
#define	S_SDP	80


struct adsym
{
	char	a_str[4];	/* addressing string */
	int	a_val;		/* addressing mode value */
};

extern struct adsym xyz[];
extern struct adsym e[];
extern struct adsym pshm[];
extern struct adsym pulm[];

	/* machine dependent functions */

#ifdef	OTHERSYSTEM
	
	/* m16adr.c */
extern	int		addr(struct expr *esp);
extern	int		admode(struct adsym *sp);
extern	int		any(int c, char *str);
extern	int		srch(char *str);

	/* m16mch.c */
extern	VOID		machine(struct mne *mp);
extern	VOID		mchubyt(struct expr *e1);
extern	int		mchindx(int t1, struct expr *e1);
extern	int		mchimm(struct expr *e1);
extern	int		mchcon(struct expr *e1);
extern	int		mchpcr(struct expr *esp);
extern	VOID		minit(void);
extern	int		setbit(int b);
extern	int		getbit(void);
extern	int		comma(void);

#else

	/* m16adr.c */
extern	int		addr();
extern	int		admode();
extern	int		any();
extern	int		srch();

	/* m16mch.c */
extern	VOID		machine();
extern	VOID		mchubyt();
extern	int		mchindx();
extern	int		mchimm();
extern	int		mchcon();
extern	int		mchpcr();
extern	VOID		minit();
extern	int		setbit();
extern	int		getbit();
extern	int		comma();

#endif

