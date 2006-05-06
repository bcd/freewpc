/* lkrloc.c */

/*
 * (C) Copyright 1989-2000
 * All Rights Reserved
 *
 * Alan R. Baldwin
 * 721 Berkeley St.
 * Kent, Ohio  44240
 *
 *   With enhancements from:
 *
 *	John L. Hartman	(JLH)
 *	jhartman@compuserve.com
 *
 *	Bill McKinnon (BM)
 *	w_mckinnon@conknet.com
 */

#include <stdio.h>
#include <string.h>

#ifdef WIN32
#include <stdlib.h>
#else
#include <alloc.h>
#endif

#include "aslink.h"

/*)Module	lkrloc.c
 *
 *	The module lkrloc.c contains the functions which
 *	perform the relocation calculations.
 *
 *	lkrloc.c contains the following functions:
 *		addr_t	adb_1b()
 *		addr_t	adb_2b()
 *		addr_t	adb_3b()
 *		addr_t	adb_4b()
 *		addr_t	adb_xb()
 *		addr_t	adb_lo()
 *		addr_t	adb_hi()
 *		addr_t	adw_xb()
 *		VOID	erpdmp()
 *		VOID	errdmp()
 *		addr_t	evword()
 *		VOID	prntval()
 *		VOID	rele()
 *		VOID	relerr()
 *		VOID	relerp()
 *		VOID	reloc()
 *		VOID	relp()
 *		VOID	relr()
 *		VOID	relt()
 *
 *	lkrloc.c the local variable errmsg[].
 *
 */

/*)Function	VOID	reloc(c)
 *
 *		int c			process code
 *
 *	The function reloc() calls a particular relocation
 *	function determined by the process code.
 *
 *	local variable:
 *		none
 *
 *	global variables:
 *		int	lkerr		error flag
 *
 *	called functions:
 *		int	fprintf()	c_library
 *		VOID	rele()		lkrloc.c
 *		VOID	relp()		lkrloc.c
 *		VOID	relr()		lkrloc.c
 *		VOId	relt()		lkrloc.c
 *
 *	side effects:
 *		Refer to the called relocation functions.
 *
 */

VOID
reloc(c)
int c;
{
	switch(c) {

	case 'T':
		relt();
		break;

	case 'R':
		relr();
		break;

	case 'P':
		relp();
		break;

	case 'E':
		rele();
		break;

	default:
		fprintf(stderr, "Undefined Relocation Operation\n");
		lkerr++;
		break;

	}
}


/*)Function	VOID	relt()
 *
 *	The function relt() evaluates a T line read by
 *	the linker. Each byte value read is saved in the
 *	rtval[] array, rtflg[] is set, and the number of
 *	evaluations is maintained in rtcnt.
 *
 *		T Line
 *
 *		T xx xx nn nn nn nn nn ...
 *
 *
 *		In:	"T n0 n1 n2 n3 ... nn"
 *
 *		Out:	  0    1    2    ..  rtcnt
 *			+----+----+----+----+----+
 *		  rtval | n0 | n1 | n2 | .. | nn |
 *			+----+----+----+----+----+
 *		  rtflag|  1 |  1 |  1 |  1 |  1 |
 *			+----+----+----+----+----+
 *
 * 	The  T  line contains the assembled code output by the assem-
 *	bler with xx xx being the offset address from the  current  area
 *	base address and nn being the assembled instructions and data in
 *	byte format.
 *
 *	local variable:
 *		none
 *
 *	global variables:
 *		int	rtcnt		number of values evaluated
 *		int	rtflg[]		array of evaluation flags
 *		int	rtval[]		array of evaluation values
 *
 *	called functions:
 *		int	eval()		lkeval.c
 *		int	more()		lklex.c
 *
 *	side effects:
 *		Linker input T line evaluated.
 *
 */

VOID
relt()
{
	rtcnt = 0;
	while (more()) {
		if (rtcnt < NTXT) {
			rtval[rtcnt] = eval();
			rtflg[rtcnt] = 1;
			rtcnt++;
		}
	}
}

/*)Function	VOID	relr()
 *
 *	The function relr() evaluates a R line read by
 *	the linker.  The R line data is combined with the
 *	previous T line data to perform the relocation of
 *	code and data bytes.  The S19 / IHX output and
 *	translation of the LST files to RST files may be
 *	performed.
 *
 *		R Line
 *
 *		R 0 0 nn nn n1 n2 xx xx ...
 *
 * 	The R line provides the relocation information to the linker.
 *	The nn nn value is the current area index, i.e.  which area  the
 *	current  values  were  assembled.  Relocation information is en-
 *	coded in groups of 4 bytes:
 *
 *	1.  n1 is the relocation mode and object format
 *	 	1.  bit 0 word(0x00)/byte(0x01)
 *	 	2.  bit 1 relocatable area(0x00)/symbol(0x02)
 *	 	3.  bit 2 normal(0x00)/PC relative(0x04) relocation
 *	 	4.  bit 3  1-byte(0x00)/2-byte(0x08) byte data
 *	 	5.  bit 4 signed(0x00)/unsigned(0x10) byte data
 *	 	6.  bit 5 normal(0x00)/page '0'(0x20) reference
 *	 	7.  bit 6 normal(0x00)/page 'nnn'(0x40) reference
 *		8.  bit 7 LSB(0x00)/MSB(0x80) byte
 *
 *	2.  n2  is  a byte index into the corresponding (i.e.  pre-
 *	 	ceeding) T line data (i.e.  a pointer to the data to be
 *	 	updated  by  the  relocation).   The T line data may be
 *	 	1-byte or  2-byte  byte  data  format  or  2-byte  word
 *	 	format.
 *
 *	3.  xx xx  is the area/symbol index for the area/symbol be-
 *	 	ing referenced.  the corresponding area/symbol is found
 *		in the header area/symbol lists.
 *
 *	The groups of 4 bytes are repeated for each item requiring relo-
 *	cation in the preceeding T line.
 *
 *	local variable:
 *		areax	**a		pointer to array of area pointers
 *		int	aindex		area index
 *		char	*errmsg[]	array of pointers to error strings
 *		int	error		error code
 *		int	lkerr		error flag
 *		int	mode		relocation mode
 *		adrr_t	paga		paging base area address
 *		addr_t	pags		paging symbol address
 *		addr_t	pc		relocated base address
 *		addr_t	r		PCR relocation value
 *		addr_t	reli		relocation initial value
 *		addr_t	relv		relocation final value
 *		int	rindex		symbol / area index
 *		addr_t	rtbase		base code address
 *		addr_t	rtofst		rtval[] index offset
 *		int	rtp		index into T data
 *		sym	**s		pointer to array of symbol pointers
 *
 *	global variables:
 *		int	a_bytes		T Line Address Bytes
 *		head	*hp		pointer to the head structure
 *		int	oflag		output type flag
 *		rerr	rerr		linker error structure
 *		FILE	*stderr		standard error device
 *		int	uflag		relocation listing flag
 *
 *	called functions:
 *		addr_t	adb_1b()	lkrloc.c
 *		addr_t	adb_2b()	lkrloc.c
 *		addr_t	adb_xb()	lkrloc.c
 *		addr_t	adb_lo()	lkrloc.c
 *		addr_t	adb_hi()	lkrloc.c
 *		addr_t	evword()	lkrloc.c
 *		int	eval()		lkeval.c
 *		int	fprintf()	c_library
 *		VOID	lkout()		lkout.c
 *		VOID	lkulist		lklist.c
 *		int	more()		lklex.c
 *		VOID	relerr()	lkrloc.c
 *		int	symval()	lksym.c
 *
 *	side effects:
 *		The R and T lines are combined to produce
 *		relocated code and data.  Output Sxx / Ixx
 *		and relocated listing files may be produced.
 *
 */

VOID
relr()
{
	register int mode;
	register addr_t reli, relv;
	int aindex, rindex, rtp, error, v;
	addr_t rtbase, rtofst, rtpofst, paga, pags, pc;
	struct areax **a;
	struct sym **s;

	/*
	 * Get area and symbol lists
	 */
	a = hp->a_list;
	s = hp->s_list;

	/*
	 * Verify Area Mode
	 */
	if (eval() != (R_WORD | R_AREA) || eval()) {
		fprintf(stderr, "R input error\n");
		lkerr++;
	}

	/*
	 * Get area pointer
	 */
	aindex = evword();
	if (aindex >= hp->h_narea) {
		fprintf(stderr, "R area error\n");
		lkerr++;
		return;
	}

	/*
	 * Base values
	 */
	rtbase = adb_xb(0, 0);
	rtofst = a_bytes;

	/*
	 * Relocate address
	 */
	pc = adb_xb(a[aindex]->a_addr, 0);

	/*
	 * Do remaining relocations
	 */
	while (more()) {
		error = 0;
		relv = 0;
		rtpofst = rtofst;
		mode = eval();
		rtp = eval();
		rindex = evword();

		/*
		 * R_SYM or R_AREA references
		 */
		if (mode & R_SYM) {
			if (rindex >= hp->h_nglob) {
				fprintf(stderr, "R symbol error\n");
				lkerr++;
				return;
			}
			reli = symval(s[rindex]);
		} else {
			if (rindex >= hp->h_narea) {
				fprintf(stderr, "R area error\n");
				lkerr++;
				return;
			}
			reli = a[rindex]->a_addr;
		}

		/*
		 * R_PCR addressing
		 */
		if (mode & R_PCR) {
			if (mode & R_BYTE) {
				reli -= (pc + (rtp-rtofst) + 1);
			} else {
				reli -= (pc + (rtp-rtofst) + 2);
			}
		}

		/*
		 * Standard Modes
		 */
		if ((mode & R_ECHEK) != R_EXTND) {
			paga = 0;
			pags = 0;
			/*
			 * R_PAG0 or R_PAG addressing
			 */
			if (mode & (R_PAG0|R_PAG)) {
				paga  = sdp.s_area->a_addr;
				pags  = sdp.s_addr;
				reli -= paga + pags;
			}

			/*
			 * R_BYTE or R_WORD operation
			 */
			if (mode & R_BYTE) {
				if (mode & R_BYTX) {
					if (mode & R_MSB) {
						relv = adb_hi(reli, rtp);
					} else {
						relv = adb_lo(reli, rtp);
					}
					rtofst += (a_bytes - 1);
				} else {
					relv = adb_1b(reli, rtp);
				}
			} else {
				relv = adw_xb(2, reli, rtp);
				rtofst += (a_bytes - 2);
			}

			/*
			 * Page Relocation Error Checking
			 */
			if (mode & R_PAG0 && (relv & ~0xFF || paga || pags))
				error = 4;
			if (mode & R_PAG  && (relv & ~0xFF))
				error = 5;
		/*
		 * Extended Modes
		 */
		} else {
			switch(mode & R_EMASK) {
			case R_J11:
				if ((hilo == 0) || (a_bytes < 2)) {
					error = 8;
				}
				/*
				 * JLH: 11 bit jump destination for 8051.
				 * Forms two byte instruction with
				 * op-code bits in the MIDDLE!
				 * rtp points at 3 byte locus:
				 * first two will get the address,
				 * third one has raw op-code
				 */
				relv = adw_xb(2, reli, rtp);

				/*
				 * Calculate absolute destination
				 * relv must be on same 2K page as pc
				 */
				if ((relv & ~0x7ff) !=
				   ((pc + rtp - rtofst) & ~0x7ff)) {
					error = 6;
				}

				rtofst += (a_bytes - 2);

				/*
				 * Merge MSB with op-code,
				 * ignoring top 5 bits of address.
				 * Then hide the op-code.
				 */
 				rtval[rtp + (a_bytes - 2)] =
					rtval[rtp + a_bytes] |
					((rtval[rtp + (a_bytes - 2)] & 0x07)<<5);
				rtflg[rtp + a_bytes] = 0;
				rtofst += 1;
				break;

			case R_J19:
				if ((hilo == 0) || (a_bytes < 3)) {
					error = 8;
				}
				/*
				 * BK: 19 bit jump destination for DS80C390.
				 * Forms four byte instruction with
				 * op-code bits in the MIDDLE!
				 * rtp points at 4 byte locus:
				 * first three will get the address,
				 * fourth one has raw op-code
				 */
				relv = adw_xb(3, reli, rtp);

				/*
				 * Calculate absolute destination
				 * relv must be on same 512K page as pc
				 */
				if ((relv & ~0x7ffff) !=
				   ((pc + rtp - rtofst) & ~0x7ffff)) {
					error = 7;
				}

				rtofst += (a_bytes - 3);

				/*
				 * Merge MSB with op-code,
				 * ignoring top 5 bits of address.
				 * Then hide the op-code.
				 */
				rtval[rtp + (a_bytes - 3)] =
					rtval[rtp + a_bytes] |
					((rtval[rtp + (a_bytes - 3)] & 0x07)<<5);
				rtflg[rtp + a_bytes] = 0;
				rtofst += 1;
				break;

			case R_3BYTE:
				/*
				 * 24 bit destination
				 */
				relv = adb_3b(reli, rtp);
				break;

			case R_4BYTE:
				/*
				 * 32 bit destination
				 */
				relv = adb_4b(reli, rtp);
				break;

			default:
				error = 8;
				break;
			}
		}

		/*
		 * Unsigned Byte Checking
		 */
		if (mode & R_USGN && mode & R_BYTE && relv & ~0xFF)
			error = 1;

		/*
		 * PCR Relocation Error Checking
		 */
		if (mode & R_PCR) {
			v = relv - reli;
			if ((mode & R_BYTE) && (mode & R_BYTX)) {
				if ((v < ~0x7F) || (v > 0x7F))
					error = 2;
			} else {
				if ((v < ~0x7FFF) || (v > 0x7FFF))
					error = 3;
			}
		}

		/*
		 * Error Processing
		 */
		if (error) {
			rerr.aindex = aindex;
			rerr.mode = mode;
			rerr.rtbase = rtbase + rtp - rtpofst;
			rerr.rindex = rindex;
			rerr.rval = relv - reli;
			relerr(errmsg[error-1]);
		}
	}
	if (uflag != 0) {
		lkulist(1);
	}
	if (oflag != 0) {
		lkout(1);
	}
}

char *errmsg[] = {
	"Unsigned Byte error",
	"Byte PCR relocation error",
	"Word PCR relocation error",
	"Page0 relocation error",
	"Page Mode relocation error",
	"2K Page relocation error",
	"512K Page relocation error",
	"Undefined Extended Mode error"
};


/*)Function	VOID	relp()
 *
 *	The function relp() evaluates a P line read by
 *	the linker.  The P line data is combined with the
 *	previous T line data to set the base page address
 *	and test the paging boundary and length.
 *
 *		P Line
 *
 *		P 0 0 nn nn n1 n2 xx xx
 *
 * 	The  P  line provides the paging information to the linker as
 *	specified by a .setdp directive.  The format of  the  relocation
 *	information is identical to that of the R line.  The correspond-
 *	ing T line has the following information:
 *		T xx xx aa aa bb bb
 *
 * 	Where  aa aa is the area reference number which specifies the
 *	selected page area and bb bb is the base address  of  the  page.
 *	bb bb will require relocation processing if the 'n1 n2 xx xx' is
 *	specified in the P line.  The linker will verify that  the  base
 *	address is on a 256 byte boundary and that the page length of an
 *	area defined with the PAG type is not larger than 256 bytes.
 *
 *	local variable:
 *		areax	**a		pointer to array of area pointers
 *		int	aindex		area index
 *		int	mode		relocation mode
 *		addr_t	relv		relocation value
 *		int	rindex		symbol / area index
 *		int	rtp		index into T data
 *		sym	**s		pointer to array of symbol pointers
 *
 *	global variables:
 *		head	*hp		pointer to the head structure
 *		int	lkerr		error flag
 *		sdp	sdp		base page structure
 *		FILE	*stderr		standard error device
 *
 *	called functions:
 *		addr_t	adb_2b()		lkrloc.c
 *		addr_t	evword()	lkrloc.c
 *		int	eval()		lkeval.c
 *		int	fprintf()	c_library
 *		int	more()		lklex.c
 *		int	symval()	lksym.c
 *
 *	side effects:
 *		The P and T lines are combined to set
 *		the base page address and report any
 *		paging errors.
 *
 */

VOID
relp()
{
	register int aindex, rindex;
	int mode, rtp;
	addr_t relv;
	struct areax **a;
	struct sym **s;

	/*
	 * Get area and symbol lists
	 */
	a = hp->a_list;
	s = hp->s_list;

	/*
	 * Verify Area Mode
	 */
	if (eval() != (R_WORD | R_AREA) || eval()) {
		fprintf(stderr, "P input error\n");
		lkerr++;
	}

	/*
	 * Get area pointer
	 */
	aindex = evword();
	if (aindex >= hp->h_narea) {
		fprintf(stderr, "P area error\n");
		lkerr++;
		return;
	}

	/*
	 * Do remaining relocations
	 */
	while (more()) {
		mode = eval();
		rtp = eval();
		rindex = evword();

		/*
		 * R_SYM or R_AREA references
		 */
		if (mode & R_SYM) {
			if (rindex >= hp->h_nglob) {
				fprintf(stderr, "P symbol error\n");
				lkerr++;
				return;
			}
			relv = symval(s[rindex]);
		} else {
			if (rindex >= hp->h_narea) {
				fprintf(stderr, "P area error\n");
				lkerr++;
				return;
			}
			relv = a[rindex]->a_addr;
		}
		adb_xb(relv, rtp);
	}

	/*
	 * Paged values
	 */
	aindex = adb_xb(0,a_bytes);
	if (aindex >= hp->h_narea) {
		fprintf(stderr, "P area error\n");
		lkerr++;
		return;
	}
	sdp.s_areax = a[aindex];
	sdp.s_area = sdp.s_areax->a_bap;
	sdp.s_addr = adb_xb(0,a_bytes*2);
	if (sdp.s_area->a_addr & 0xFF || sdp.s_addr & 0xFF)
		relerp("Page Definition Boundary Error");
}

/*)Function	VOID	rele()
 *
 *	The function rele() closes all open output files
 *	at the end of the linking process.
 *
 *	local variable:
 *		none
 *
 *	global variables:
 *		int	oflag		output type flag
 *		int	uflag		relocation listing flag
 *
 *	called functions:
 *		VOID	iout()		lkout.c
 *		VOID	lkulist()	lklist.c
 *
 *	side effects:
 *		All open output files are closed.
 *
 */

VOID
rele()
{
	if (uflag != 0) {
		lkulist(0);
	}
	if (oflag != 0) {
		lkout(0);
	}
}

/*)Function	addr_t 	evword()
 *
 *	The function evword() combines two byte values
 *	into a single word value.
 *
 *	local variable:
 *		addr_t	v		temporary evaluation variable
 *
 *	global variables:
 *		hilo			byte ordering parameter
 *
 *	called functions:
 *		int	eval()		lkeval.c
 *
 *	side effects:
 *		Relocation text line is scanned to combine
 *		two byte values into a single word value.
 *
 */

addr_t
evword()
{
	register addr_t v;

	if (hilo) {
		v =  (eval() << 8);
		v +=  eval();
	} else {
		v =   eval();
		v += (eval() << 8);
	}
	return(v);
}

/*)Function	addr_t 	adb_1b(v, i)
 *
 *		int	v		value to add to byte
 *		int	i		rtval[] index
 *
 *	The function adb_1b() adds the value of v to
 *	the single byte value contained in rtval[i].
 *	The new value of rtval[i] is returned.
 *
 *	local variable:
 *		none
 *
 *	global variables:
 *		none
 *
 *	called functions:
 *		none
 *
 *	side effects:
 *		The byte value of rtval[] is changed.
 *
 */

addr_t
adb_1b(v, i)
register addr_t v;
register int i;
{
	return(rtval[i] += v);
}

/*)Function	addr_t 	adb_2b(v, i)
 *
 *		int	v		value to add to word
 *		int	i		rtval[] index
 *
 *	The function adb_2b() adds the value of v to the
 *	2 byte value contained in rtval[i] and rtval[i+1].
 *	The new value of rtval[i] / rtval[i+1] is returned.
 *
 *	local variable:
 *		addr_t	j		temporary evaluation variable
 *
 *	global variables:
 *		hilo			byte ordering parameter
 *
 *	called functions:
 *		none
 *
 *	side effects:
 *		The 2 byte value of rtval[] is changed.
 *
 */

addr_t
adb_2b(v, i)
register addr_t v;
register int i;
{
	register addr_t j;

	if (hilo) {
		j = v + (rtval[i] << 8) +
			(rtval[i+1] & 0xff);
		rtval[i] = (j >> 8) & 0xff;
		rtval[i+1] = j & 0xff;
	} else {
		j = v + (rtval[i] & 0xff) +
			(rtval[i+1] << 8);
		rtval[i] = j & 0xff;
		rtval[i+1] = (j >> 8) & 0xff;
	}
	return(j);
}

/*)Function	addr_t 	adb_3b(v, i)
 *
 *		int	v		value to add to word
 *		int	i		rtval[] index
 *
 *	The function adb_3b() adds the value of v to the
 *	three byte value contained in rtval[i], rtval[i+1], and rtval[i+2].
 *	The new value of rtval[i] / rtval[i+1] / rtval[i+2] is returned.
 *
 *	local variable:
 *		addr_t	j		temporary evaluation variable
 *
 *	global variables:
 *		hilo			byte ordering parameter
 *
 *	called functions:
 *		none
 *
 *	side effects:
 *		The 3 byte value of rtval[] is changed.
 *
 */

addr_t
adb_3b(v, i)
register addr_t v;
register int i;
{
	register addr_t j;

	if (hilo) {
		j = v + (((rtval[i] << 16) & 0xff0000) +
			 ((rtval[i+1] << 8 ) & 0xff00) +
			 ((rtval[i+2]) & 0xff));
		rtval[i] = (j >> 16) & 0xff;
		rtval[i+1] = (j >> 8) & 0xff;
		rtval[i+2] = j & 0xff;
	} else {
		j = v + (((rtval[i+2] << 16) & 0xff0000) +
			 ((rtval[i+1] << 8 ) & 0xff00) +
			 ((rtval[i]) & 0xff));
		rtval[i] = j & 0xff;
		rtval[i+1] = (j >> 8) & 0xff;
		rtval[i+2] = (j >> 16) & 0xff;
    }
    return(j);
}

/*)Function	addr_t 	adb_4b(v, i)
 *
 *		int	v		value to add to word
 *		int	i		rtval[] index
 *
 *	The function adb_4b() adds the value of v to the
 *	four byte value contained in rtval[i], ..., rtval[i+3].
 *	The new value of rtval[i], ...,  rtval[i+3] is returned.
 *
 *	local variable:
 *		addr_t	j		temporary evaluation variable
 *
 *	global variables:
 *		hilo			byte ordering parameter
 *
 *	called functions:
 *		none
 *
 *	side effects:
 *		The 4 byte value of rtval[] is changed.
 *
 */

addr_t
adb_4b(v, i)
register addr_t v;
register int i;
{
	register addr_t j;

	if (hilo) {
		j = v + (((rtval[i] << 24) & 0xff000000) +
			 ((rtval[i+1] << 16) & 0xff0000) +
			 ((rtval[i+2] << 8 ) & 0xff00) +
			 ((rtval[i+3]) & 0xff));
		rtval[i] = (j >> 24) & 0xff;
		rtval[i+1] = (j >> 16) & 0xff;
		rtval[i+2] = (j >> 8) & 0xff;
		rtval[i+3] = j & 0xff;
	} else {
		j = v + (((rtval[i+3] << 24) & 0xff000000) +
			 ((rtval[i+2] << 16) & 0xff0000) +
			 ((rtval[i+1] << 8 ) & 0xff00) +
			 ((rtval[i]) & 0xff));
		rtval[i] = j & 0xff;
		rtval[i+1] = (j >> 8) & 0xff;
		rtval[i+2] = (j >> 16) & 0xff;
		rtval[i+3] = (j >> 24) & 0xff;
    }
    return(j);
}

/*)Function	addr_t 	adb_xb(v, i)
 *
 *		int	v		value to add to x-bytes
 *		int	i		rtval[] index
 *
 *	The function adx_x() adds the value of v to
 *	the value contained in rtval[i] for x-bytes.
 *	The new value of rtval[i] for x-bytes is returned.
 *
 *	local variable:
 *		none
 *
 *	global variables:
 *		none
 *
 *	called functions:
 *		addr_t	adb_1b()	lkrloc.c
 *		addr_t	adb_2b()	lkrloc.c
 *		addr_t	adb_3b()	lkrloc.c
 *		addr_t	adb_4b()	lkrloc.c
 *
 *	side effects:
 *		The x-byte value of rtval[] is changed.
 *
 */

addr_t
adb_xb(v, i)
register addr_t v;
register int i;
{
	addr_t j;

	switch(a_bytes){
	case 1:
		j = adb_1b(v, i);
		return(j & 0x80 ? j | ~0x7F : j & 0x7F);
	case 2:
		j = adb_2b(v, i);
		return(j & 0x8000 ? j | ~0x7FFF : j & 0x7FFF);
	case 3:
		j = adb_3b(v, i);
		return(j & 0x800000 ? j | ~0x7FFFFF : j & 0x7FFFFF);
	case 4:
		j = adb_4b(v, i);
		return(j & 0x80000000 ? j | ~0x7FFFFFFF : j & 0x7FFFFFFF);
	default:
		return(0);
	}
	return(0);
}

/*)Function	addr_t 	adb_lo(v, i)
 *
 *		int	v		value to add to byte
 *		int	i		rtval[] index
 *
 *	The function adb_lo() adds the value of v to the
 *	value contained in rtval[i] through rtval[i + a_bytes - 1].
 *	The new value of rtval[i] ... is returned.
 *	The rtflg[] flags are cleared for all rtval[i] ... except
 *	the LSB.
 *
 *	local variable:
 *		addr_t	j		temporary evaluation variable
 *
 *	global variables:
 *		hilo			byte ordering parameter
 *
 *	called functions:
 *		none
 *
 *	side effects:
 *		The value of rtval[] is changed.
 *		The rtflg[] values corresponding to all bytes
 *		except the LSB of the value are cleared to reflect
 *		the fact that the LSB is the selected byte.
 *
 */

addr_t
adb_lo(v, i)
addr_t	v;
int	i;
{
	register addr_t j;
	register int m, n;

	j = adb_xb(v, i);
	/*
	 * LSB is lowest order byte of data
	 */
	m = (hilo ? a_bytes-1 : 0);
	for (n=0; n<a_bytes; n++) {
		if(n != m) rtflg[i+n] = 0;
	}
	return (j);
}

/*)Function	addr_t 	adb_hi(v, i)
 *
 *		int	v		value to add to byte
 *		int	i		rtval[] index
 *
 *	The function adb_hi() adds the value of v to the
 *	value contained in rtval[i] through rtval[i + a_bytes - 1].
 *	The new value of rtval[i] .... is returned.
 *	The LSB rtflg[] is cleared.
 *
 *	local variable:
 *		addr_t	j		temporary evaluation variable
 *
 *	global variables:
 *		hilo			byte ordering parameter
 *
 *	called functions:
 *		none
 *
 *	side effects:
 *		The value of rtval[] is changed.
 *		The rtflg[] values corresponding to all bytes
 *		except the 2nd byte (MSB) are cleared to reflect
 *		the fact that the MSB is the selected byte.
 *
 */

addr_t
adb_hi(v, i)
addr_t	v;
int	i;
{
	register addr_t j;
	register int m, n;

	j = adb_xb(v, i);
	/*
	 * MSB is next lowest order byte of data
	 */
	m = (hilo ? a_bytes-2 : 1);
	for (n=0; n<a_bytes; n++) {
		if(n != m) rtflg[i+n] = 0;
	}
	return (j);
}

/*)Function	addr_t 	adw_xb(x, v, i)
 *
 *		int	x		number of bytes to allow
 *		int	v		value to add to byte
 *		int	i		rtval[] index
 *
 *	The function adw_xb() adds the value of v to the
 *	value contained in rtval[i] through rtval[i + a_bytes - 1].
 *	The new value of rtval[i] .... is returned.
 *	The rtflg[] is cleared for bytes of higher order than x.
 *
 *	local variable:
 *		addr_t	j		temporary evaluation variable
 *
 *	global variables:
 *		hilo			byte ordering parameter
 *
 *	called functions:
 *		none
 *
 *	side effects:
 *		The value of rtval[] is changed.
 *		The rtflg[] values corresponding to all bytes
 *		of higher order than x are cleared to reflect
 *		the fact that x bytes are selected.
 *
 */

addr_t
adw_xb(x, v, i)
int x;
addr_t v;
int i;
{
	register addr_t j;
	register int n;

	j = adb_xb(v, i);
	/*
	 * X LS Bytes
	 */
	i += (hilo ? 0 : x);
	for (n=0; n<(a_bytes-x); n++,i++) {
		rtflg[i] = 0;
	}
	return (j);
}

/*)Function	VOID	relerr(str)
 *
 *		char	*str		error string
 *
 *	The function relerr() outputs the error string to
 *	stderr and to the map file (if it is open).
 *
 *	local variable:
 *		none
 *
 *	global variables:
 *		FILE	*mfp		handle for the map file
 *
 *	called functions:
 *		VOID	errdmp()	lkrloc.c
 *
 *	side effects:
 *		Error message inserted into map file.
 *
 */

VOID
relerr(str)
char *str;
{
	errdmp(stderr, str);
	if (mfp)
		errdmp(mfp, str);
}

/*)Function	VOID	errdmp(fptr, str)
 *
 *		FILE	*fptr		output file handle
 *		char	*str		error string
 *
 *	The function errdmp() outputs the error string str
 *	to the device specified by fptr.  Additional information
 *	is output about the definition and referencing of
 *	the symbol / area error.
 *
 *	local variable:
 *		int	mode		error mode
 *		int	aindex		area index
 *		int	lkerr		error flag
 *		int	rindex		error index
 *		sym	**s		pointer to array of symbol pointers
 *		areax	**a		pointer to array of area pointers
 *		areax	*raxp		error area extension pointer
 *
 *	global variables:
 *		sdp	sdp		base page structure
 *
 *	called functions:
 *		int	fprintf()	c_library
 *		VOID	prntval()	lkrloc.c
 *
 *	side effects:
 *		Error reported.
 *
 */

VOID
errdmp(fptr, str)
FILE *fptr;
char *str;
{
	int mode, aindex, rindex;
	struct sym **s;
	struct areax **a;
	struct areax *raxp;

	a = hp->a_list;
	s = hp->s_list;

	mode = rerr.mode;
	aindex = rerr.aindex;
	rindex = rerr.rindex;

	/*
	 * Print Error
	 */
	fprintf(fptr, "\n?ASlink-Warning-%s", str);
	lkerr++;

	/*
	 * Print symbol if symbol based
	 */
	if (mode & R_SYM) {
		fprintf(fptr, " for symbol  %s\n",
			&s[rindex]->s_id[0]);
	} else {
		fprintf(fptr, "\n");
	}

	/*
	 * Print Ref Info
	 */
/*         11111111112222222222333333333344444444445555555555666666666677777*/
/*12345678901234567890123456789012345678901234567890123456789012345678901234*/
/*        |                 |                 |                 |           */
	fprintf(fptr,
"         file              module            area                   offset\n");
	fprintf(fptr,
"  Refby  %-14.14s    %-14.14s    %-14.14s    ",
			hp->h_lfile->f_idp,
			&hp->m_id[0],
			&a[aindex]->a_bap->a_id[0]);
	prntval(fptr, rerr.rtbase);

	/*
	 * Print Def Info
	 */
	if (mode & R_SYM) {
		raxp = s[rindex]->s_axp;
	} else {
		raxp = a[rindex];
	}
/*         11111111112222222222333333333344444444445555555555666666666677777*/
/*12345678901234567890123456789012345678901234567890123456789012345678901234*/
/*        |                 |                 |                 |           */
	fprintf(fptr,
"  Defin  %-14.14s    %-14.14s    %-14.14s    ",
			raxp->a_bhp->h_lfile->f_idp,
			&raxp->a_bhp->m_id[0],
			&raxp->a_bap->a_id[0]);
	if (mode & R_SYM) {
		prntval(fptr, s[rindex]->s_addr);
	} else {
		prntval(fptr, rerr.rval);
	}
}

/*)Function	VOID	prntval(fptr, v)
 *
 *		FILE	*fptr		output file handle
 *		addr_t	v		value to output
 *
 *	The function prntval() outputs the value v, in the
 *	currently selected radix, to the device specified
 *	by fptr.
 *
 *	local variable:
 *		none
 *
 *	global variables:
 *		int	xflag		current radix
 *
 *	called functions:
 *		int	fprintf()	c_library
 *
 *	side effects:
 *		none
 *
 */

VOID
prntval(fptr, v)
FILE *fptr;
addr_t v;
{
	register char *frmt;

	switch(xflag) {
	default:
	case 0:
		switch(a_bytes) {
		default:
		case 2: frmt = "       %04X\n"; break;
		case 3: frmt = "     %06X\n"; break;
		case 4: frmt = "    %08X\n"; break;
		}
		break;
	case 1:
		switch(a_bytes) {
		default:
		case 2: frmt = "     %06o\n"; break;
		case 3: frmt = "   %08o\n"; break;
		case 4: frmt = "%011o\n"; break;
		}
		break;
	case 2:
		switch(a_bytes) {
		default:
		case 2: frmt = "      %05u\n"; break;
		case 3: frmt = "   %08u\n"; break;
		case 4: frmt = " %010u\n"; break;
		}
		break;
	}
	fprintf(fptr, frmt, v);
}

/*)Function	VOID	relerp(str)
 *
 *		char	*str		error string
 *
 *	The function relerp() outputs the paging error string to
 *	stderr and to the map file (if it is open).
 *
 *	local variable:
 *		none
 *
 *	global variables:
 *		FILE	*mfp		handle for the map file
 *
 *	called functions:
 *		VOID	erpdmp()	lkrloc.c
 *
 *	side effects:
 *		Error message inserted into map file.
 *
 */

VOID
relerp(str)
char *str;
{
	erpdmp(stderr, str);
	if (mfp)
		erpdmp(mfp, str);
}

/*)Function	VOID	erpdmp(fptr, str)
 *
 *		FILE	*fptr		output file handle
 *		char	*str		error string
 *
 *	The function erpdmp() outputs the error string str
 *	to the device specified by fptr.
 *
 *	local variable:
 *		head	*thp		pointer to head structure
 *
 *	global variables:
 *		int	lkerr		error flag
 *		sdp	sdp		base page structure
 *
 *	called functions:
 *		int	fprintf()	c_library
 *		VOID	prntval()	lkrloc.c
 *
 *	side effects:
 *		Error reported.
 *
 */

VOID
erpdmp(fptr, str)
FILE *fptr;
char *str;
{
	register struct head *thp;

	thp = sdp.s_areax->a_bhp;

	/*
	 * Print Error
	 */
	fprintf(fptr, "\n?ASlink-Warning-%s\n", str);
	lkerr++;

	/*
	 * Print PgDef Info
	 */
/*         111111111122222222223333333333444444444455555555556666666666777*/
/*123456789012345678901234567890123456789012345678901234567890123456789012*/
	fprintf(fptr,
"         file              module            pgarea            pgoffset\n");
	fprintf(fptr,
"  PgDef  %-14.14s    %-14.14s    %-14.14s    ",
			thp->h_lfile->f_idp,
			&thp->m_id[0],
			&sdp.s_area->a_id[0]);
	prntval(fptr, sdp.s_area->a_addr + sdp.s_addr);
}
