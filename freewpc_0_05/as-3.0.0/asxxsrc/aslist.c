/* aslist.c */

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
 */

#include <stdio.h>
#include <setjmp.h>
#include <string.h>

#ifdef WIN32
#include <stdlib.h>
#else
#include <alloc.h>
#endif

#include "asxxxx.h"

/*)Module	aslist.c
 *
 *	The module aslist.c contains all the functions used
 *	to generate the assembler list and symbol output files.
 *
 *	aslist.c contains the following functions:
 *		VOID	list()
 *		VOID	list1()
 *		VOID	list2()
 *		VOID	slew()
 *		VOID	lstsym()
 *
 *	The module aslist.c contains no local/static variables
 */

/*)Function	VOID	list()
 *
 *	The function list() generates the listing output
 *	which includes the input source, line numbers,
 *	and generated code.  Numerical output may be selected
 *	as hexadecimal, decimal, or octal.
 * 
 *	local variables:
 *		int *	wp		pointer to the assembled data bytes
 *		int *	wpt		pointer to the data byte mode
 *		int	n		number of bytes listed per line
 *		int	nb		computed number of assembled bytes
 *		int	l_addr		laddr (int) truncated to 2-bytes
 *
 *	global variables:
 *		int	cb[]		array of assembler output values
 *		int	cbt[]		array of assembler relocation types
 *					describing the data in cb[]
 *		int *	cp		pointer to assembler output array cb[]
 *		int *	cpt		pointer to assembler relocation type
 *					output array cbt[]
 *		char	eb[]		array of generated error codes
 *		char *	ep		pointer into error list
 *					array eb[]
 *		char	ib[]		assembler-source text line
 *		addr_t	laddr		address of current assembler line,
 *				 	equate, or value of .if argument
 *		FILE *	lfp		list output file handle
 *		int	line		current assembler source line number
 *		int	lmode		listing mode
 *		int	xflag		-x, listing radix flag
 *
 *	functions called:
 *		int	fprintf()	c_library
 *		VOID	list1()		aslist.c
 *		int	putc()		c_library
 *		VOID	slew()		asslist.c
 *
 *	side effects:
 *		Listing or symbol output updated.
 */

/* The Output Formats
| Tabs- |       |       |       |       |       |
          11111111112222222222333333333344444-----
012345678901234567890123456789012345678901234-----
   |    |               |     | |
ee XXXX xx xx xx xx xx xx LLLLL *************	HEX(16)
ee 000000 ooo ooo ooo ooo LLLLL *************	OCTAL(16)
ee  DDDDD ddd ddd ddd ddd LLLLL *************	DECIMAL(16)
                     XXXX
		   OOOOOO
		    DDDDD

| Tabs- |       |       |       |       |       |
          11111111112222222222333333333344444-----
012345678901234567890123456789012345678901234-----
     |       |                  |     | |
ee    XXXXXX xx xx xx xx xx xx xx LLLLL *********	HEX(24)
ee   OO000000 ooo ooo ooo ooo ooo LLLLL *********	OCTAL(24)
ee   DDDDDDDD ddd ddd ddd ddd ddd LLLLL *********	DECIMAL(24)
                           XXXXXX
			 OOOOOOOO
			 DDDDDDDD

| Tabs- |       |       |       |       |       |
          11111111112222222222333333333344444-----
012345678901234567890123456789012345678901234-----
  |          |                  |     | |
ee  XXXXXXXX xx xx xx xx xx xx xx LLLLL *********	HEX(32)
eeOOOOO000000 ooo ooo ooo ooo ooo LLLLL *********	OCTAL(32)
ee DDDDDDDDDD ddd ddd ddd ddd ddd LLLLL *********	DECIMAL(32)
                         XXXXXXXX
		      OOOOOOOOOOO
		       DDDDDDDDDD
*/

VOID
list()
{
	register char *frmt, *wp;
	register int *wpt;
	register int n, nb;
	register int l_addr;

	if (lfp == NULL || lmode == NLIST)
		return;

	/*
	 * Get Correct Line Number
	 */
	if (incfil >= 0) {
		line = incline[incfil];
		if (line == 0) {
			if (incfil > 0) {
				line = incline[incfil-1];
			} else {
				line = srcline[cfile];
			}
		}
	} else {
		line = srcline[cfile];
	}

	/*
	 * Move to next line.
	 */
	slew(lfp, pflag);

	/*
	 * Output a maximum of NERR error codes with listing.
	 */
	while (ep < &eb[NERR])
		*ep++ = ' ';
	fprintf(lfp, "%.2s", eb);

	/*
	 * Source listing only option.
	 */
	if (lmode == SLIST) {
		switch(a_bytes) {
		default:
		case 2: frmt = "%24s%5u %s\n"; break;
		case 3:
		case 4: frmt = "%32s%5u %s\n"; break;
		}
		fprintf(lfp, frmt, "", line, ib);
		return;
	}
	if (lmode == ALIST) {
		outchk(HUGE,HUGE);
	}

	/*
	 * Truncate (int) to N-Bytes
	 */
	l_addr = laddr & a_mask;

	/*
	 * HEX output Option.
	 */
	if (xflag == 0) {		/* HEX */
		/*
		 * Equate only
		 */
		if (lmode == ELIST) {
			switch(a_bytes) {
			default:
			case 2: frmt = "%19s%04X"; break;
			case 3: frmt = "%25s%06X"; break;
			case 4: frmt = "%23s%08X"; break;
			}
			fprintf(lfp, frmt, "", l_addr);
			fprintf(lfp, " %5u %s\n", line, ib);
			return;
		}

		/*
		 * Address (with allocation)
		 */
		switch(a_bytes) {
		default:
		case 2: frmt = " %04X"; break;
		case 3: frmt = "    %06X"; break;
		case 4: frmt = "  %08X"; break;
		}
		fprintf(lfp, frmt, l_addr);
		if (lmode == ALIST || lmode == BLIST) {
			switch(a_bytes) {
			default:
			case 2: frmt = "%19s%5u %s\n"; break;
			case 3:
			case 4: frmt = "%22s%5u %s\n"; break;
			}
			fprintf(lfp, frmt, "", line, ib);
			outdot();
			return;
		}
		wp = cb;
		wpt = cbt;
		nb = (int) (cp - cb);

		/*
		 * Bytes per Line and Spacing
		 */
		switch(a_bytes) {
		default:
		case 2: n = 6; frmt = "%7s"; break;
		case 3:
		case 4: n = 7; frmt = "%12s"; break;
		}

		/*
		 * First line of output for this source line with data.
		 */
		list1(wp, wpt, nb, n, 1);
		fprintf(lfp, " %5u %s\n", line, ib);

		/*
		 * Subsequent lines of output if more data.
		 */
		while ((nb - n) > 0) {
			nb -= n;
			wp += n;
			wpt += n;
			slew(lfp, 0);
			fprintf(lfp, frmt, "");
			list1(wp, wpt, nb, n, 0);
			putc('\n', lfp);
		}
	} else
	/*
	 * OCTAL output Option.
	 */
	if (xflag == 1) {		/* OCTAL */
		/*
		 * Equate only
		 */
		if (lmode == ELIST) {
			switch(a_bytes) {
			default:
			case 2: frmt = "%17s%06o"; break;
			case 3: frmt = "%23s%08o"; break;
			case 4: frmt = "%20s%011o"; break;
			}
			fprintf(lfp, frmt, "", l_addr);
			fprintf(lfp, " %5u %s\n", line, ib);
			return;
		}

		/*
		 * Address (with allocation)
		 */
		switch(a_bytes) {
		default:
		case 2: frmt = " %06o"; break;
		case 3: frmt = "   %08o"; break;
		case 4: frmt = "%011o"; break;
		}
		fprintf(lfp, frmt, l_addr);
		if (lmode == ALIST || lmode == BLIST) {
			switch(a_bytes) {
			default:
			case 2: frmt = "%17s%5u %s\n"; break;
			case 3:
			case 4: frmt = "%21s%5u %s\n"; break;
			}
			fprintf(lfp, frmt, "", line, ib);
			outdot();
			return;
		}
		wp = cb;
		wpt = cbt;
		nb = (int) (cp - cb);

		/*
		 * Bytes per Line and Spacing
		 */
		switch(a_bytes) {
		default:
		case 2: n = 4; frmt = "%9s"; break;
		case 3:
		case 4: n = 5; frmt = "%13s"; break;
		}

		/*
		 * First line of output for this source line with data.
		 */
		list1(wp, wpt, nb, n, 1);
		fprintf(lfp, " %5u %s\n", line, ib);

		/*
		 * Subsequent lines of output if more data.
		 */
		while ((nb - n) > 0) {
			nb -= n;
			wp += n;
			wpt += n;
			slew(lfp, 0);
			fprintf(lfp, frmt, "");
			list1(wp, wpt, nb, n, 0);
			putc('\n', lfp);
		}
	} else
	/*
	 * DECIMAL output Option.
	 */
	if (xflag == 2) {		/* DECIMAL */
		/*
		 * Equate only
		 */
		if (lmode == ELIST) {
			switch(a_bytes) {
			default:
			case 2: frmt = "%18s%05u"; break;
			case 3: frmt = "%23s%08u"; break;
			case 4: frmt = "%21s%010u"; break;
			}
			fprintf(lfp, frmt, "", l_addr);
			fprintf(lfp, " %5u %s\n", line, ib);
			return;
		}

		/*
		 * Address (with allocation)
		 */
		switch(a_bytes) {
		default:
		case 2: frmt = "  %05u"; break;
		case 3: frmt = "   %08u"; break;
		case 4: frmt = " %010u"; break;
		}
		fprintf(lfp, frmt, l_addr);
		if (lmode == ALIST || lmode == BLIST) {
			switch(a_bytes) {
			default:
			case 2: frmt = "%17s%5u %s\n"; break;
			case 3:
			case 4: frmt = "%21s%5u %s\n"; break;
			}
			fprintf(lfp, frmt, "", line, ib);
			outdot();
			return;
		}
		wp = cb;
		wpt = cbt;
		nb = (int) (cp - cb);

		/*
		 * Bytes per Line and Spacing
		 */
		switch(a_bytes) {
		default:
		case 2: n = 4; frmt = "%9s"; break;
		case 3:
		case 4: n = 5; frmt = "%13s"; break;
		}

		/*
		 * First line of output for this source line with data.
		 */
		list1(wp, wpt, nb, n, 1);
		fprintf(lfp, " %5u %s\n", line, ib);

		/*
		 * Subsequent lines of output if more data.
		 */
		while ((nb - n) > 0) {
			nb -= n;
			wp += n;
			wpt += n;
			slew(lfp, 0);
			fprintf(lfp, frmt, "");
			list1(wp, wpt, nb, n, 0);
			putc('\n', lfp);
		}
	}
}

/*)Function	VOID	list1(wp, wpt, nw, n, f)
 *
 *		int	f		fill blank fields (1)
 *		int	n		number of bytes listed per line
 *		int	nb		number of data bytes
 *		int *	wp		pointer to data bytes
 *		int *	wpt		pointer to data byte mode
 *
 *	local variables:
 *		int	i		loop counter
 *
 *	global variables:
 *		int	xflag		-x, listing radix flag
 *
 *	functions called:
 *		VOID	list2()		asslist.c
 *		int	fprintf()	c_library
 *
 *	side effects:
 *		Data formatted and output to listing.
 */

VOID
list1(wp, wpt, nb, n, f)
register char *wp;
register int *wpt, nb, n, f;
{
	register int i;

	/*
	 * HEX output Option.
	 */
	if (xflag == 0) {		/* HEX */
		/*
		 * Bound number of words to HEX maximum per line.
		 */
		if (nb > n)
			nb = n;

		/*
		 * Output bytes.
		 */
		for (i=0; i<nb; ++i) {
			list2(*wpt++);
			fprintf(lfp, "%02X", (*wp++)&0377);
		}

		/*
		 * Output blanks if required.
		 */
		if (f) {
			while (i++ < n) {
				fprintf(lfp, "   ");
			}
		}
	} else
	/*
	 * OCTAL output Option.
	 */
	if (xflag == 1) {		/* OCTAL */
		/*
		 * Bound number of words to OCTAL maximum per line.
		 */
		if (nb > n)
			nb = n;

		/*
		 * Output bytes.
		 */
		for (i=0; i<nb; ++i) {
			list2(*wpt++);
			fprintf(lfp, "%03o", (*wp++)&0377);
		}

		/*
		 * Output blanks if required.
		 */
		if (f) {
			while (i++ < n) {
				fprintf(lfp, "    ");
			}
		}
	} else
	/*
	 * DECIMAL output Option.
	 */
	if (xflag == 2) {		/* DECIMAL */
		/*
		 * Bound number of words to DECIMAL maximum per line.
		 */
		if (nb > n)
			nb = n;

		/*
		 * Output bytes.
		 */
		for (i=0; i<nb; ++i) {
			list2(*wpt++);
			fprintf(lfp, "%03u", (*wp++)&0377);
		}

		/*
		 * Output blanks if required.
		 */
		if (f) {
			while (i++ < n) {
				fprintf(lfp, "    ");
			}
		}
	}
}

/*)Function	VOID	list2(wpt)
 *
 *		int *	wpt		pointer to relocation mode
 *
 *	The function list2() outputs the selected
 *	relocation flag as specified by fflag.
 *
 *	local variables:
 *		int	c		relocation flag character
 *		int	t		relocation mode
 *
 *	global variables:
 *		int	fflag		-f(f), relocations flagged flag
 *
 *	functions called:
 *		int	putc()		c_library
 *
 *	side effects:
 *		Relocation flag output to listing file.
 */

VOID
list2(t)
register int t;
{
	register int c;

	c = ' ';

	/*
	 * Designate a relocatable word by `.
	 */
	if (fflag == 1) {
		if (t & R_RELOC) {
			c = '`';
		}
	} else
	/*
	 * Designate a relocatable word by its mode:
	 *	page0 or paged		*
	 *	unsigned		u (v) (U) (V)
	 *	operand offset		p (q) (P) (Q)
	 *	relocatable symbol	r (s) (R) (S)
	 */
	if (fflag >= 2) {
		if (t & R_RELOC) {
			if ((t & (R_PAG0|R_PAG)) && ((t & R_ECHEK) != R_EXTND)) {
				c = '*';
			} else if (t & R_USGN) {
				c = 'u';
			} else if (t & R_PCR) {
				c = 'p';
			} else {
				c = 'r';
			}
			if (t & R_HIGH || t & R_BYT4) c += 1;
			if (t & R_BYT3 || t & R_BYT4) c &= ~0x20;
		}
	}

	/*
	 * Output the selected mode.
	 */
	putc(c, lfp);
}

/*)Function	VOID	slew(fp, flag)
 *
 *		FILE *	fp		file handle for listing
 *		int	flag		enable pagination
 *
 *	The function slew() increments the page line count.
 *	If the page overflows and pagination is enabled:
 *		1)	put out a page skip,
 *		2)	a title,
 *		3)	a subtitle,
 *		4)	and reset the line count.
 *
 *	local variables:
 *		none
 *
 *	global variables:
 *		char	cpu[]		cpu type string
 *		int	lop		current line number on page
 *		int	page		current page number
 *		char	stb[]		Subtitle string buffer
 *		char	tb[]		Title string buffer
 *
 *	functions called:
 *		int	fprintf()	c_library
 *
 *	side effects:
 *		Increments page line counter, on overflow
 *		a new page header is output to the listing file.
 */

VOID
slew(fp,flag)
FILE *fp;
int flag;
{
	register char *frmt;

	if ((lop++ >= NLPP) && flag) {
		fprintf(fp, "\fASxxxx Assembler %s  (%s), page %u.\n",
			VERSION, cpu, ++page);
		switch(xflag) {
		default:
		case 0:	frmt = "Hexidecimal [%d-Bits]\n"; break;
		case 1:	frmt = "Octal [%d-Bits]\n"; break;
		case 2:	frmt = "Decimal [%d-Bits]\n"; break;
		}
		fprintf(fp, frmt, 8 * a_bytes);
		fprintf(fp, "%s\n", tb);
		fprintf(fp, "%s\n\n", stb);
		lop = 6;
	}
}

/*)Function	VOID	lstsym(fp)
 *
 *		FILE *	fp		file handle for output
 *
 *	The function lstsym() outputs alphabetically
 *	sorted symbol and area tables.
 *
 *	local variables:
 *		int	c		temporary
 *		int	i		loop counter
 *		int	j		temporary
 *		int	k		temporary
 *		char *	ptr		pointer to an id string
 *		int	nmsym		number of symbols
 *		int	narea		number of areas
 *		sym *	sp		pointer to symbol structure
 *		sym **	p		pointer to an array of
 *					pointers to symbol structures
 *		area *	ap		pointer to an area structure
 *
 *	global variables:
 *		area *	areap		pointer to an area structure
 *		char	aretbl[]	string "Area Table"
 *		sym	dot		defined as sym[0]
 *		char	stb[]		Subtitle string buffer
 *		sym * symhash[]		array of pointers to NHASH
 *					linked symbol lists
 *		char	symtbl[]	string "Symbol Table"
 *		FILE *	tfp		symbol table output file handle
 *		int	wflag		-w, wide listing flag
 *		int	xflag		-x, listing radix flag
 *
 *	functions called:
 *		int	fprintf()	c_library
 *		int	putc()		c_library
 *		VOID	slew()		aslist.c
 *		int	strcmp()	c_library
 *		char *	strcpy()	c_library
 *
 *	side effects:
 *		Symbol and area tables output.
 */

VOID
lstsym(fp)
FILE *fp;
{
	register int c, i, j, k;
	register char *frmt, *ptr;
	int nmsym, narea;
	struct sym *sp;
	struct sym **p;
	struct area *ap;

	/*
	 * Symbol Table Header
	 */
	strcpy(stb, &symtbl[0]);
	lop = NLPP;
	if (fp == tfp)
		page = 0;
	slew(fp, 1);

	/*
	 * Find number of symbols
	 */
	nmsym = 0;
	for (i=0; i<NHASH; i++) {
		sp = symhash[i];
		while (sp) {
			if (sp != &dot)
				++nmsym;
			sp = sp->s_sp;
		}
	}
	if (nmsym == 0)
		goto atable;

	/*
	 * Allocate space for an array of pointers to symbols
	 * and load array.
	 */
	if ((p = (struct sym **) malloc(sizeof((struct sym *) sp)*nmsym))
		== NULL) {
		fprintf(fp, "Insufficient space to build Symbol Table.\n");
		return;
	}
	nmsym = 0;
	for (i=0; i<NHASH; i++) {
		sp = symhash[i];
		while (sp) {
			if (sp != &dot)
				p[nmsym++] = sp;
			sp = sp->s_sp;
		}
	}

	/*
	 * Bubble Sort on Symbol Table Array
	 */
	j = 1;
	c = nmsym - 1;
	while (j) {
		j = 0;
		for (i=0; i<c; ++i) {
			if (strcmp(&p[i]->s_id[0],&p[i+1]->s_id[0]) > 0) {
				j = 1;
				sp = p[i+1];
				p[i+1] = p[i];
				p[i] = sp;
			}
		}
	}

	/*
	 * Symbol Table Output
	 */
	for (i=0; i<nmsym;) {
		sp = p[i];
		if (sp->s_area) {
			j = sp->s_area->a_ref;
			switch(xflag) {
			default:
			case 0:	frmt = " %2X "; break;
			case 1:	frmt = "%3o "; break;
			case 2:	frmt = "%3u "; break;
			}
			fprintf(fp, frmt, j);
		} else {
			fprintf(fp, "    ");
		}

		ptr = &sp->s_id[0];
		if (wflag) {
			fprintf(fp, "%-55.55s", ptr );	/* JLH */
		} else {
			fprintf(fp, "%-14.14s", ptr);
		}
		if (sp->s_flag & S_ASG) {
			fprintf(fp, " = ");
		} else {
			fprintf(fp, "   ");
		}
		if (sp->s_type == S_NEW) {
			switch(a_bytes) {
			default:
			case 2:
				switch(xflag) {
				default:
				case 0:	frmt = "  **** "; break;
				case 1:	frmt = "****** "; break;
				case 2:	frmt = " ***** "; break;
				}
				break;

			case 3:
				switch(xflag) {
				default:
				case 0:	frmt = "  ****** "; break;
				case 1:	frmt = "******** "; break;
				case 2:	frmt = "******** "; break;
				}
				break;

			case 4:
				switch(xflag) {
				default:
				case 0:	frmt = "   ******** "; break;
				case 1:	frmt = "*********** "; break;
				case 2:	frmt = " ********** "; break;
				}
				break;

			}
			fprintf(fp, frmt);
		} else {
			j = sp->s_addr & 0xFFFF;
			switch(a_bytes) {
			default:
			case 2:
				switch(xflag) {
				default:
				case 0:	frmt = "  %04X "; break;
				case 1:	frmt = "%06o "; break;
				case 2:	frmt = " %05u "; break;
				}
				break;

			case 3:
				switch(xflag) {
				default:
				case 0:	frmt = "  %06X "; break;
				case 1:	frmt = "%08o "; break;
				case 2:	frmt = "%08u "; break;
				}
				break;

			case 4:
				switch(xflag) {
				default:
				case 0:	frmt = "   %08X "; break;
				case 1:	frmt = "%011o "; break;
				case 2:	frmt = " %010u "; break;
				}
				break;
			}
			fprintf(fp, frmt, j);
		}

		j = 0;
		if (sp->s_flag & S_GBL) {
			putc('G', fp);
			++j;
		}
		if (sp->s_area != NULL) {
			putc('R', fp);
			++j;
		}
		if (sp->s_type == S_NEW) {
			putc('X', fp);
			++j;
		}
		if (wflag) {
			putc('\n', fp);		/* JLH */
			slew(fp, 0);
			++i;
		} else {
			if (++i % 2 == 0) {
				putc('\n', fp);
				slew(fp, pflag);
			} else
			if (i < nmsym) {
				while (j++ < 4)
					putc(' ', fp);
				fprintf(fp, "| ");
			}
		}
	}
	putc('\n', fp);

	/*
	 * Area Table Header
	 */

atable:
	strcpy(stb, &aretbl[0]);
	lop = NLPP;
	slew(fp, 1);

	/*
	 * Area Table Output
	 */
	narea = 0;
	ap = areap;
	while (ap) {
		++narea;
		ap = ap->a_ap;
	}
	for (i=0; i<narea; ++i) {
		ap = areap;
		for (j=i+1; j<narea; ++j)
			ap = ap->a_ap;
		j = ap->a_ref;
		switch(xflag) {
		default:
		case 0:	frmt = "  %2X "; break;
		case 1:	frmt = " %3o "; break;
		case 2:	frmt = " %3u "; break;
		}
		fprintf(fp, frmt, j);

		ptr = &ap->a_id[0];
		if (wflag) {
			fprintf(fp, "%-35.35s", ptr );
		} else {
			fprintf(fp, "%-14.14s", ptr);
		}

		j = ap->a_size & 0xFFFF;
		k = ap->a_flag;
		switch(a_bytes) {
		default:
		case 2:
			switch(xflag) {
			default:
			case 0:	frmt = "   size %4X   flags %3X\n"; break;
			case 1:	frmt = "   size %6o   flags %3o\n"; break;
			case 2:	frmt = "   size %5u   flags %3u\n"; break;
			}
			break;

		case 3:
			switch(xflag) {
			default:
			case 0:	frmt = "   size %6X   flags %3X\n"; break;
			case 1:	frmt = "   size %8o   flags %3o\n"; break;
			case 2:	frmt = "   size %8u   flags %3u\n"; break;
			}
			break;

		case 4:
			switch(xflag) {
			default:
			case 0:	frmt = "   size %8X   flags %3X\n"; break;
			case 1:	frmt = "   size %11o   flags %3o\n"; break;
			case 2:	frmt = "   size %10u   flags %3u\n"; break;
			}
			break;
		}
		fprintf(fp, frmt, j, k);
	}		
}
