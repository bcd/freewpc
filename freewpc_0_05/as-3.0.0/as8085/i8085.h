/* i8085.h */

/*
 * (C) Copyright 1989-2000
 * All Rights Reserved
 *
 * Alan R. Baldwin
 * 721 Berkeley St.
 * Kent, Ohio  44240
 */

/*)BUILD
	$(PROGRAM) =	AS8085
	$(INCLUDE) = {
		ASXXXX.H
		I8085.H
	}
	$(FILES) = {
		I85EXT.C
		I85MCH.C
		I85PST.C
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
 * Symbol types.
 */
#define	S_INH	50		/* One byte */
#define	S_RST	51		/* Restart */
#define	S_ADI	52		/* One byte immediate */
#define	S_ADD	53		/* One reg. */
#define	S_JMP	54		/* Jumps, calls, etc. */
#define	S_INR	55		/* Reg. dest. */
#define	S_LXI	56		/* Word immediate */
#define	S_LDAX	57		/* B or D */
#define	S_INX	58		/* Word reg. dest. */
#define	S_PUSH	59		/* Push, pop */
#define	S_MOV	60		/* Mov */
#define	S_MVI	61		/* Mvi */
#define	S_REG	62		/* Registers */

/*
 * Other
 */
#define	S_FLAG	70

/*
 * Registers.
 */
#define	B	0
#define	C	1
#define	D	2
#define	E	3
#define	H	4
#define	L	5
#define	M	6
#define	A	7
#define	SP	8
#define	PSW	9

	/* machine dependent functions */

#ifdef	OTHERSYSTEM
	
	/* 85mch.c */
extern	int		comma(void);
extern	VOID		machine(struct mne *mp);
extern	VOID		minit(void);
extern	VOID		out3(int a, int b);
extern	int		reg(void);
extern	int		regpair(int r, int s);

#else

	/* 85mch.c */
extern	int		comma();
extern	VOID		machine();
extern	VOID		minit();
extern	VOID		out3();
extern	int		reg();
extern	int		regpair();

#endif

