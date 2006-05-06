/* asxxxx.h */

/*
 * (C) Copyright 1989-2000
 * All Rights Reserved
 *
 * Alan R. Baldwin
 * 721 Berkeley St.
 * Kent, Ohio  44240
 *
 *   With enhancements from
 *
 *	John L. Hartman	(JLH)
 *	jhartman@compuserve.com
 *
 *	Bill McKinnon (BM)
 *	w_mckinnon@conknet.com
 */

#define	VERSION	"V03.00"

/*)Module	asxxxx.h
 *
 *	The module asxxxx.h contains the definitions for constants,
 *	structures, global variables, and ASxxxx functions
 *	contained in the ASxxxx.c files.  The two functions
 *	and three global variables from the machine dependent
 *	files are also defined.
 */

/*
 *	 compiler/operating system specific definitions
 */

/* DECUS C void definition */
/* File/extension seperator */

#ifdef	decus
#define	VOID	char
#define	FSEPX	'.'
#endif

/* PDOS C void definition */
/* File/extension seperator */

#ifdef	PDOS
#define	VOID	char
#define	FSEPX	':'
#endif

/* Default void definition */
/* File/extension seperator */

#ifndef	VOID
#define	VOID	void
#define	FSEPX	'.'
#define	OTHERSYSTEM
#endif

/*
 * Error definitions
 */
#define	ER_NONE		0	/* No error */
#define	ER_WARNING	1	/* Warning */
#define	ER_ERROR	2	/* Assembly error */
#define	ER_FATAL	3	/* Fatal error */

/*
 * Assembler definitions.
 */
#define	LFTERM	'('		/* Left expression delimeter */
#define	RTTERM	')'		/* Right expression delimeter */

#define NCPS	80		/* Characters per symbol */
#define	HUGE	1000		/* A huge number */
#define NERR	3		/* Errors per line */
#define NINPUT	128		/* Input buffer size */
#define NCODE	128		/* Listing code buffer size */
#define NTITL	80		/* Title buffer size */
#define	NSBTL	80		/* SubTitle buffer size */
#define	NHASH	64		/* Buckets in hash table */
#define	HMASK	077		/* Hash mask */
#define	NLPP	60		/* Lines per page */
#define	MAXFIL	6		/* Maximum command line input files */
#define	MAXINC	6		/* Maximum nesting of include files */
#define	MAXIF	10		/* Maximum nesting of if/else/endif */
#define	FILSPC	80		/* Chars. in filespec */

#define NLIST	0		/* No listing */
#define SLIST	1		/* Source only */
#define ALIST	2		/* Address only */
#define	BLIST	3		/* Address only with allocation */
#define CLIST	4		/* Code */
#define	ELIST	5		/* Equate only */

#define	dot	sym[0]		/* Dot, current loc */
#define	dca	area[0]		/* Dca, default code area */


typedef	unsigned int addr_t;

/*
 *	The area structure contains the parameter values for a
 *	specific program or data section.  The area structure
 *	is a linked list of areas.  The initial default area
 *	is "_CODE" defined in asdata.c, the next area structure
 *	will be linked to this structure through the structure
 *	element 'struct area *a_ap'.  The structure contains the
 *	area name, area reference number ("_CODE" is 0) determined
 *	by the order of .area directives, area size determined
 *	from the total code and/or data in an area, area fuzz is
 *	a variable used to track pass to pass changes in the
 *	area size caused by variable length instruction formats,
 *	and area flags which specify the area's relocation type.
 */
struct	area
{
	struct	area *a_ap;	/* Area link */
	char *	a_id;		/* Area Name */
	int	a_ref;		/* Ref. number */
	addr_t	a_size;		/* Area size */
	addr_t	a_fuzz;		/* Area fuzz */
	int	a_flag;		/* Area flags */
};

/*
 *	The "A_" area constants define values used in
 *	generating the assembler area output data.
 *
 * Area flags
 *
 *	   7     6     5     4     3     2     1     0
 *	+-----+-----+-----+-----+-----+-----+-----+-----+
 *	|     |     |     | PAG | ABS | OVR |     |     |
 *	+-----+-----+-----+-----+-----+-----+-----+-----+
 */

#define	A_CON	000		/* Concatenating */
#define	A_OVR	004		/* Overlaying */
#define	A_REL	000		/* Relocatable */
#define	A_ABS	010		/* absolute */
#define	A_NOPAG	000		/* Non-Paged */
#define	A_PAG	020		/* Paged */

/*
 *	The "R_" relocation constants define values used in
 *	generating the assembler relocation output data for
 *	areas, symbols, and code.
 *
 * Relocation flags
 *
 *	   7     6     5     4     3     2     1     0
 *	+-----+-----+-----+-----+-----+-----+-----+-----+
 *	| MSB | PAGn| PAG0| USGN| BYT2| PCR | SYM | BYT |
 *	+-----+-----+-----+-----+-----+-----+-----+-----+
 */

#define	R_WORD	0000		/* 16 bit */
#define	R_BYTE	0001		/*  8 bit */

#define	R_AREA	0000		/* Base type */
#define	R_SYM	0002

#define	R_NORM	0000		/* PC adjust */
#define	R_PCR	0004

#define	R_BYT1	0000		/* Byte count for R_BYTE = 1 */
#define	R_BYTX	0010		/* Byte count for R_BYTE = X */

#define	R_SGND	0000		/* Signed Byte */
#define	R_USGN	0020		/* Unsigned Byte */

#define	R_NOPAG	0000		/* Page Mode */
#define	R_PAG0	0040		/* Page '0' */
#define	R_PAG	0100		/* Page 'nnn' */

#define	R_LSB	0000		/* low byte */
#define	R_MSB	0200		/* high byte */

/*
 *	Additional "R_" functionality is required to support
 *	some microprocesssor architectures.   The 'illegal'
 *	"R_" mode of R_WORD | R_BYTX is used as a designator
 *	of the extended R_ modes.  The extended modes replace
 *	the PAGING modes and are being added in an adhoc manner
 *	as follows:
 *
 * Extended Mode relocation flags
 *
 *	   7     6     5     4     3     2     1     0
 *	+-----+-----+-----+-----+-----+-----+-----+-----+
 *	| MSB |  x  |  x  | USGN|  1  | PCR | SYM |  0  |
 *	+-----+-----+-----+-----+-----+-----+-----+-----+
 */

#define	R_ECHEK	0011		/* Extended Mode Check Bits */
#define	R_EXTND	0010		/* Extended Mode Code */
#define	R_EMASK	0151		/* Extended Mode Mask */

/* #define R_AREA 0000 */	/* Base type */
/* #define R_SYM  0002 */

/* #define R_NORM 0000 */	/* PC adjust */
/* #define R_PCR  0004 */

/* #define R_SGND 0000 */	/* Signed value */
/* #define R_USGN 0020 */	/* Unsigned value */

/* #define R_LSB  0000 */	/* output low byte */
/* #define R_MSB  0200 */	/* output high byte */

#define	R_J11	0010		/* JLH: 11 bit JMP and CALL (8051) */
#define R_J19   0050		/* BM:	19 bit JMP and CALL (DS80C390) */
#define R_3BYTE 0110		/* 	3-Byte */
#define R_4BYTE 0150		/*	4-Byte */

/*
 * Listing Control Flags
 */

#define	R_HIGH	0010000		/* High Byte */
#define	R_BYT3	0020000		/* 3rd  Byte */
#define	R_BYT4	0040000		/* 4th  Byte */
#define	R_RELOC	0100000		/* Relocation */

#define	R_DEF	00		/* Global def. */
#define	R_REF	01		/* Global ref. */
#define	R_REL	00		/* Relocatable */
#define	R_ABS	02		/* Absolute */
#define	R_GBL	00		/* Global */
#define	R_LCL	04		/* Local */

/*
 *	The mne structure is a linked list of the assembler
 *	mnemonics and directives.  The list of mnemonics and
 *	directives contained in the device dependent file
 *	xxxpst.c are hashed and linked into NHASH lists in
 *	module assym.c by syminit().  The structure contains
 *	the mnemonic/directive name, a subtype which directs
 *	the evaluation of this mnemonic/directive, a flag which
 *	is used to detect the end of the mnemonic/directive
 *	list in xxxpst.c, and a value which is normally
 *	associated with the assembler mnemonic base instruction
 *	value.
 */
struct	mne
{
	struct	mne *m_mp;	/* Hash link */
	char	*m_id;		/* Mnemonic (JLH) */
	char	m_type;		/* Mnemonic subtype */
	char	m_flag;		/* Mnemonic flags */
	addr_t	m_valu;		/* Value */
};

/*
 *	The sym structure is a linked list of symbols defined
 *	in the assembler source files.  The first symbol is "."
 *	defined in asdata.c.  The entry 'struct tsym *s_tsym'
 *	links any temporary symbols following this symbol and
 *	preceeding the next normal symbol.  The structure also
 *	contains the symbol's name, type (USER or NEW), flag
 *	(global, assigned, and multiply defined), a pointer
 *	to the area structure defining where the symbol is
 *	located, a reference number assigned by outgsd() in
 *	asout.c, and the symbols address relative to the base
 *	address of the area where the symbol is located.
 */
struct	sym
{
	struct	sym  *s_sp;	/* Hash link */
	struct	tsym *s_tsym;	/* Temporary symbol link */
	char	*s_id;		/* Symbol (JLH) */
	char	s_type;		/* Symbol subtype */
	char	s_flag;		/* Symbol flags */
	struct	area *s_area;	/* Area line, 0 if absolute */
	int	s_ref;		/* Ref. number */
	addr_t	s_addr;		/* Address */
};

#define	S_GBL		01	/* Global */
#define	S_ASG		02	/* Assigned */
#define	S_MDF		04	/* Mult. def */
#define	S_END		010	/* End mark for ___pst files */

#define	S_NEW		0	/* New name */
#define	S_USER		1	/* User name */
				/* unused slot */
				/* unused slot */
				/* unused slot */

#define	S_DATA		5	/* .byte, .word, .3byte, .4byte */
#define	S_ASCII		6	/* .ascii */
#define	S_ASCIZ		7	/* .asciz */
#define	S_BLK		8	/* .blkb or .blkw */
#define	S_INCL		9	/* .include */
#define	S_DAREA		10	/* .area */
#define	S_ATYP		11	/* .area type */
#define	S_AREA		12	/* .area name */
#define	S_GLOBL		13	/* .globl */
#define	S_PAGE		14	/* .page */
#define	S_TITLE		15	/* .title */
#define	S_SBTL		16	/* .sbttl */
#define	S_IF		17	/* .if */
#define	S_ELSE		18	/* .else */
#define	S_ENDIF		19	/* .endif */
#define	S_EVEN		20	/* .even */
#define	S_ODD		21	/* .odd */
#define	S_RADIX		22	/* .radix */
#define	S_ORG		23	/* .org */
#define	S_MODUL		24	/* .module */
#define	S_ASCIS		25	/* .ascis */
#define	S_ERROR		26	/* .assume or .error */
#define	S_BITS		27	/* .8bit, .16bit, .24bit, .32bit */


/*
 *	The tsym structure is a linked list of temporary
 *	symbols defined in the assembler source files following
 *	a normal symbol.  The structure contains the temporary
 *	symbols number, a flag (multiply defined), a pointer to the
 *	area structure defining where the temporary structure
 *	is located, and the temporary symbol's address relative
 *	to the base address of the area where the symbol
 *	is located.
 */
struct	tsym
{
	struct	tsym *t_lnk;	/* Link to next */
	char t_num;		/* 0-255$ */
	char t_flg;		/* flags */
	struct	area *t_area;	/* Area */
	addr_t	t_addr;		/* Address */
};

/*
 *	External Definitions for all Global Variables
 */

extern	int	aserr;		/*	ASxxxx error counter
				 */
extern	jmp_buf	jump_env;	/*	compiler dependent structure
				 *	used by setjmp() and longjmp()
				 */
extern	int	inpfil;		/*	count of assembler
				 *	input files specified
				 */
extern	int	incfil;		/*	current file handle index
				 *	for include files
				 */
extern	int	cfile;		/*	current file handle index
				 *	of input assembly files
				 */
extern	int	flevel;		/*	IF-ELSE-ENDIF flag will be non
				 *	zero for false conditional case
				 */
extern	int	tlevel;		/*	current conditional level
				 */
extern	int	ifcnd[MAXIF+1];	/*	array of IF statement condition
				 *	values (0 = FALSE) indexed by tlevel
				 */
extern	int	iflvl[MAXIF+1];	/*	array of IF-ELSE-ENDIF flevel
				 *	values indexed by tlevel
				 */
extern	char	afn[FILSPC];	/*	current input file specification
				 */
extern	int	afp;		/*	current input file path length
				 */
extern	char	afntmp[FILSPC];	/*	temporary input file specification
				 */
extern	int	afptmp;		/*	temporary input file path length
				 */
extern	char
	srcfn[MAXFIL][FILSPC];	/*	array of source file names
				 */
extern	int
	srcfp[MAXFIL];		/*	array of source file path lengths
				 */
extern	int
	srcline[MAXFIL];	/*	current source file line
				 */
extern	char
	incfn[MAXINC][FILSPC];	/*	array of include file names
				 */
extern	int
	incfp[MAXINC];		/*	array of include file path lengths
				 */
extern	int
	incline[MAXINC];	/*	current include file line
				 */
extern	int	radix;		/*	current number conversion radix:
				 *	2 (binary), 8 (octal), 10 (decimal),
				 *	16 (hexadecimal)
				 */
extern	int	line;		/*	current assembler source
				 *	line number
				 */
extern	int	page;		/*	current page number
				 */
extern	int	lop;		/*	current line number on page
				 */
extern	int	pass;		/*	assembler pass number
				 */
extern	int	lflag;		/*	-l, generate listing flag
				 */
extern	int	gflag;		/*	-g, make undefined symbols global flag
				 */
extern	int	aflag;		/*	-a, make all symbols global flag
				 */
extern	int	oflag;		/*	-o, generate relocatable output flag
				 */
extern	int	sflag;		/*	-s, generate symbol table flag
				 */
extern	int	pflag;		/*	-p, enable listing pagination
				 */
extern	int	wflag;		/*	-w, enable wide format listing
				 */
extern	int	zflag;		/*	-z, enable symbol case sensitivity
				 */
extern	int	xflag;		/*	-x, listing radix flag
				 */
extern	int	fflag;		/*	-f(f), relocations flagged flag
				 */
extern	int	a_bytes;	/*	REL file T Line address length
				 */
extern	addr_t	a_mask;		/*	Address Mask
				 */
extern	addr_t	s_mask;		/*	Sign Mask
				 */
extern	addr_t	v_mask;		/*	Value Mask
				 */
extern	addr_t	laddr;		/*	address of current assembler line,
				 *	equate, or value of .if argument
				 */
extern	addr_t	fuzz;		/*	tracks pass to pass changes in the
				 *	address of symbols caused by
				 *	variable length instruction formats
				 */
extern	int	lmode;		/*	listing mode
				 */
extern	struct	area	area[];	/*	array of 1 area
				 */
extern	struct	area *areap;	/*	pointer to an area structure
				 */
extern	struct	sym	sym[];	/*	array of 1 symbol
				 */
extern	struct	sym *symp;	/*	pointer to a symbol structure
				 */
extern	struct	sym *symhash[NHASH]; /*	array of pointers to NHASH
				      *	linked symbol lists
				      */
extern	struct	mne *mnehash[NHASH]; /*	array of pointers to NHASH
				      *	linked mnemonic/directive lists
				      */
extern	char	*ep;		/*	pointer into error list
				 *	array eb[NERR]
				 */
extern	char	eb[NERR];	/*	array of generated error codes
				 */
extern	char	*ip;		/*	pointer into the assembler-source
				 *	text line in ib[]
				 */
extern	char	ib[NINPUT];	/*	assembler-source text line
				 */
extern	char	*cp;		/*	pointer to assembler output
				 *	array cb[]
				 */
extern	char	cb[NCODE];	/*	array of assembler output values
				 */
extern	int	*cpt;		/*	pointer to assembler relocation type
				 *	output array cbt[]
				 */
extern	int	cbt[NCODE];	/*	array of assembler relocation types
				 *	describing the data in cb[]
				 */
extern	char	tb[NTITL];	/*	Title string buffer
				 */
extern	char	stb[NSBTL];	/*	Subtitle string buffer
				 */
extern	char	erb[NINPUT+4];	/*	Error string buffer
				 */
extern	char	symtbl[];	/*	string "Symbol Table"
				 */
extern	char	aretbl[];	/*	string "Area Table"
				 */
extern	char	module[NCPS+2];	/*	module name string
				 */
extern	FILE	*lfp;		/*	list output file handle
				 */
extern	FILE	*ofp;		/*	relocation output file handle
				 */
extern	FILE	*tfp;		/*	symbol table output file handle
				 */
extern	FILE	*sfp[MAXFIL];	/*	array of assembler-source file handles
				 */
extern	FILE	*ifp[MAXINC];	/*	array of include-file file handles
				 */
extern	char	ctype[128];	/*	array of character types, one per
				 *	ASCII character
				 */
extern	char	ccase[128];	/*	an array of characters which 
				 *	perform the case translation function
				 */
/*
 * Definitions for Character Types
 */
#define	SPACE	'\000'
#define ETC	'\000'
#define	LETTER	'\001'
#define	DIGIT	'\002'
#define	BINOP	'\004'
#define	RAD2	'\010'
#define	RAD8	'\020'
#define	RAD10	'\040'
#define	RAD16	'\100'
#define	ILL	'\200'

#define	DGT2	DIGIT|RAD16|RAD10|RAD8|RAD2
#define	DGT8	DIGIT|RAD16|RAD10|RAD8
#define	DGT10	DIGIT|RAD16|RAD10
#define	LTR16	LETTER|RAD16

/*
 *	The exp structure is used to return the evaluation
 *	of an expression.  The structure supports three valid
 *	cases:
 *	(1)	The expression evaluates to a constant,
 *		mode = S_USER, flag = 0, addr contains the
 *		constant, and base = NULL.
 *	(2)	The expression evaluates to a defined symbol
 *		plus or minus a constant, mode = S_USER,
 *		flag = 0, addr contains the constant, and
 *		base = pointer to area symbol.
 *	(3)	The expression evaluates to a external
 *		global symbol plus or minus a constant,
 *		mode = S_NEW, flag = 1, addr contains the
 *		constant, and base = pointer to symbol.
 */
struct	expr
{
	char	e_mode;		/* Address mode */
	char	e_flag;		/* Symbol flag */
	addr_t	e_addr;		/* Address */
	union	{
		struct area *e_ap;
		struct sym  *e_sp;
	} e_base;		/* Rel. base */
	char	e_rlcf;		/* Rel. flags */
};

/* C Library functions */
/* for reference only
extern	VOID		exit();
extern	int		fclose();
extern	char *		fgets();
extern	FILE *		fopen();
extern	int		fprintf();
extern	VOID		longjmp();
extern	VOID *		malloc();
extern	int		printf();
extern	char		putc();
extern	int		rewind();
extern	int		setjmp();
extern	int		strcmp();
extern	char *		strcpy();
extern	int		strlen();
extern	char *		strncpy();
extern	char *		strrchr();
*/

/* Machine independent functions */

#ifdef	OTHERSYSTEM

/* asmain.c */
extern	FILE *		afile(char *fn, char *ft, int wf);
extern	VOID		afilex(char *fn, char *ft);
extern	VOID		asexit(int i);
extern	VOID		asmbl(void);
extern	int		fndidx(char *str);
extern	int		main(int argc, char *argv[]);
extern	VOID		newdot(struct area *nap);
extern	VOID		phase(struct area *ap, addr_t a);
extern	char *		usetxt[];
extern	VOID		usage(int n);

/* aslex.c */
extern	char		endline(void);
extern	int		get(void);
extern	VOID		getid(char *id, int c);
extern	int		getline(void);
extern	int		getmap(int d);
extern	int		getnb(void);
extern	VOID		getst(char *id, int c);
extern	int		more(void);
extern	VOID		unget(int c);

/* assym.c */
extern	struct	area *	alookup(char *id);
extern	struct	mne *	mlookup(char *id);
extern	int		hash(char *p, int cflag);
extern	struct	sym *	lookup(char *id);
extern	char *		new(unsigned int n);
extern	struct	sym *	slookup(char *id);
extern	char *		strsto(char *str);
extern	int		symeq(char *p1, char *p2, int cflag);
extern	VOID		syminit(void);
extern	VOID		symglob(void);
extern	VOID		allglob(void);

/* assubr.c */
extern	VOID		aerr(void);
extern	VOID		diag(void);
extern	VOID		err(int c);
extern	char *		errors[];
extern	char *		geterr(int c);
extern	VOID		qerr(void);
extern	VOID		rerr(void);

/* asexpr.c */
extern	VOID		abscheck(struct expr *esp);
extern	addr_t		absexpr(void);
extern	VOID		clrexpr(struct expr *esp);
extern	int		digit(int c, int r);
extern	VOID		exprmasks(int n);
extern	int		is_abs(struct expr *esp);
extern	VOID		expr(struct expr *esp, int n);
extern	int		oprio(int c);
extern	VOID		term(struct expr *esp);

/* aslist.c */
extern	VOID		list(void);
extern	VOID		list1(char *wp, int *wpt, int nb, int n, int f);
extern	VOID		list2(int t);
extern	VOID		lstsym(FILE *fp);
extern	VOID		slew(FILE *fp, int flag);

/* asout.c */
extern	int		lobyte(int v);
extern	int		hibyte(int v);
extern	int		thrdbyte(int v);
extern	int		frthbyte(int v);
extern	VOID		out(char *p, int n);
extern	VOID		outarea(struct area *ap);
extern	VOID		outdp(struct area *carea, struct expr *esp);
extern	VOID		outall(void);
extern	VOID		outdot(void);
extern	VOID		outbuf(char *s);
extern	VOID		outchk(int nt, int nr);
extern	VOID		outgsd(void);
extern	VOID		outsym(struct sym *sp);
extern	VOID		outab(int v);
extern	VOID		outaw(int v);
extern	VOID		outa3b(int v);
extern	VOID		outa4b(int v);
extern	VOID		outaxb(int i, int v);
extern	VOID		outatxb(int i, int v);
extern	VOID		outrb(struct expr *esp, int r);
extern	VOID		outrw(struct expr *esp, int r);
extern	VOID		outr3b(struct expr *esp, int r);
extern	VOID		outr4b(struct expr *esp, int r);
extern	VOID		outrxb(int i, struct expr *esp, int r);
extern	VOID		outr11(struct expr *esp, int op);	/* JLH */
extern	VOID		outr19(struct expr *esp, int op);	/* BM */
extern	VOID		out_lb(int v, int t);
extern	VOID		out_lw(int v, int t);
extern	VOID		out_l3b(int v, int t);
extern	VOID		out_l4b(int v, int t);
extern	VOID		out_lxb(int i, int v, int t);
extern	VOID		out_rw(int n);
extern	VOID		out_txb(int i, int v);

/* Machine dependent variables */

extern	char *		cpu;
extern	char *		dsft;
extern	int		hilo;
extern	struct	mne	mne[];

/* Machine dependent functions */

extern	VOID		machine(struct mne *mp);
extern	VOID		minit(void);

/* asxcnv.c */

/* Defined under asmain.c above
extern	VOID		asexit(int i);
extern	int		main(int argc, char *argv[]);
extern	char *		usetxt[];
extern	VOID		usage(int n);
*/
extern	VOID		linout(char *str, unsigned int n);

/* asxscn.c */

/* Defined under asmain.c above
extern	VOID		asexit(int i);
extern	int		main(int argc, char *argv[]);
extern	char *		usetxt[];
extern	VOID		usage(int n);
*/
extern	int		dgt(int rdx, char *str, int n);

#else

/* asmain.c */
extern	FILE *		afile();
extern	VOID		afilex();
extern	VOID		asexit();
extern	VOID		asmbl();
extern	int		fndidx();
extern	int		main();
extern	VOID		newdot();
extern	VOID		phase();
extern	char *		usetxt[];
extern	VOID		usage();

/* aslex.c */
extern	char		endline();
extern	int		get();
extern	VOID		getid();
extern	int		getline();
extern	int		getmap();
extern	int		getnb();
extern	VOID		getst();
extern	int		more();
extern	VOID		unget();

/* assym.c */
extern	struct	area *	alookup();
extern	struct	mne *	mlookup();
extern	int		hash();
extern	struct	sym *	lookup();
extern	char *		new();
extern	struct	sym *	slookup();
extern	char *		strsto();
extern	int		symeq();
extern	VOID		syminit();
extern	VOID		symglob();
extern	VOID		allglob();

/* assubr.c */
extern	VOID		aerr();
extern	VOID		diag();
extern	VOID		err();
extern	char *		errors[];
extern	char *		geterr();
extern	VOID		qerr();
extern	VOID		rerr();

/* asexpr.c */
extern	VOID		abscheck();
extern	addr_t		absexpr();
extern	VOID		clrexpr();
extern	int		digit();
extern	VOID		exprmasks();
extern	int		is_abs();
extern	VOID		expr();
extern	int		oprio();
extern	VOID		term();

/* aslist.c */
extern	VOID		list();
extern	VOID		list1();
extern	VOID		list2();
extern	VOID		lstsym();
extern	VOID		slew();

/* asout.c */
extern	int		lobyte();
extern	int		hibyte();
extern	int		thrdbyte();
extern	int		frthbyte();
extern	VOID		out();
extern	VOID		outarea();
extern	VOID		outdp();
extern	VOID		outall();
extern	VOID		outdot();
extern	VOID		outbuf();
extern	VOID		outchk();
extern	VOID		outgsd();
extern	VOID		outsym();
extern	VOID		outab();
extern	VOID		outaw();
extern	VOID		outa3b();
extern	VOID		outa4b();
extern	VOID		outaxb();
extern	VOID		outatxb();
extern	VOID		outrb();
extern	VOID		outrw();
extern	VOID		outr3b();
extern	VOID		outr4b();
extern	VOID		outrxb();
extern	VOID		outr11();	/* JLH */
extern	VOID		outr19();	/* BM */
extern	VOID		out_lb();
extern	VOID		out_lw();
extern	VOID		out_l3b();
extern	VOID		out_l4b();
extern	VOID		out_lxb();
extern	VOID		out_rw();
extern	VOID		out_txb();

/* Machine dependent variables */

extern	char *		cpu;
extern	char *		dsft;
extern	int		hilo;
extern	struct	mne	mne[];

/* Machine dependent functions */

extern	VOID		machine();
extern	VOID		minit();

/* asxcnv.c */

/* Defined under asmain.c above
extern	VOID		asexit();
extern	int		main();
extern	char *		usetxt[];
extern	VOID		usage();
*/
extern	VOID		linout();

/* asxscn.c */

/* Defined under asmain.c above
extern	VOID		asexit();
extern	int		main();
extern	char *		usetxt[];
extern	VOID		usage();
*/
extern	int		dgt();

#endif

