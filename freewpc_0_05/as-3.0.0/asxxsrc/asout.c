/* asout.c */

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

#include <stdio.h>
#include <setjmp.h>
#include <string.h>

#ifdef WIN32
#include <stdlib.h>
#else
#include <alloc.h>
#endif

#include "asxxxx.h"


/*)Module	asout.c
 *
 *	The module asout.c contains all the functions used to
 *	generate the .REL assembler output file.
 *
 *
 * 	The  assemblers' output object file is an ascii file containing
 *	the information needed by the linker  to  bind  multiple  object
 *	modules into a complete loadable memory image.  
 *
 *	The object module contains the following designators:  
 *
 *		[XDQ][HL][234]
 *			X	 Hexidecimal radix
 *			D	 Decimal radix
 *			Q	 Octal radix
 *	
 *			H	 Most significant byte first
 *			L	 Least significant byte first
 *	
 *			2	 16-Bit Relocatable Addresses/Data
 *			3	 24-Bit Relocatable Addresses/Data
 *			4	 32-Bit Relocatable Addresses/Data
 *
 *		H	 Header 
 *		M	 Module
 *		A	 Area
 *		S	 Symbol
 *		T	 Object code
 *		R	 Relocation information
 *		P	 Paging information
 *
 *
 *	(1)	Radix Line
 *
 * 	The  first  line  of  an object module contains the [XDQ][HL][234]
 *	format specifier (i.e.  XH2 indicates  a  hexidecimal  file  with
 *	most significant byte first and 16-bit addresses) for the
 *	following designators.  
 *
 *
 *	(2)	Header Line
 *
 *		H aa areas gg global symbols 
 *
 * 	The  header  line  specifies  the number of areas(aa) and the
 *	number of global symbols(gg) defined or referenced in  this  ob-
 *	ject module segment.  
 *
 *
 *	(3)	Module Line 
 *
 *		M name 
 *
 * 	The  module  line  specifies  the module name from which this
 *	header segment was assembled.  The module line will  not  appear
 *	if the .module directive was not used in the source program.  
 *
 *
 *	(4)	Symbol Line 
 *
 *		S string Defnnnn 
 *
 *			or 
 *
 *		S string Refnnnn 
 *
 * 	The  symbol line defines (Def) or references (Ref) the symbol
 *	'string' with the value nnnn.  The defined value is relative  to
 *	the  current area base address.  References to constants and ex-
 *	ternal global symbols will always appear before the  first  area
 *	definition.  References to external symbols will have a value of
 *	zero.  
 *
 *
 *	(5)	Area Line 
 *
 *		A label size ss flags ff 
 *
 * 	The  area  line  defines the area label, the size (ss) of the
 *	area in bytes, and the area flags (ff).  The area flags  specify
 *	the ABS, REL, CON, OVR, and PAG parameters:  
 *
 *		OVR/CON (0x04/0x00 i.e.  bit position 2) 
 *
 *		ABS/REL (0x08/0x00 i.e.  bit position 3) 
 *
 *		PAG (0x10 i.e.  bit position 4) 
 *
 *
 *	(6)	T Line 
 *
 *		T xx xx nn nn nn nn nn ...  
 *
 * 	The  T  line contains the assembled code output by the assem-
 *	bler with xx xx being the offset address from the  current  area
 *	base address and nn being the assembled instructions and data in
 *	byte format.  
 *
 *
 *	(7)	R Line 
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
 *	 	4.  bit  3  1-byte(0x00)/n-byte(0x08) byte format
 *	 	5.  bit 4 signed(0x00)/unsigned(0x10) byte data 
 *	 	6.  bit 5 normal(0x00)/page '0'(0x20) reference 
 *	 	7.  bit 6 normal(0x00)/page 'nnn'(0x40) reference 
 *		8.  bit 7 normal(0x00)/MSB of value
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
 *
 *	The groups of 4 bytes are repeated for each item requiring relo-
 *	cation in the preceeding T line.  
 *
 *
 *	(8)	P Line 
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
 * 	The  linker  defaults any direct page references to the first
 *	area defined in the input REL file.  All ASxxxx assemblers  will
 *	specify the _CODE area first, making this the default page area. 
 *
 *
 *	asout.c contains the following functions:
 *		int	lobyte();
 *		int	hibyte();
 *		int	thrdbyte();
 *		int	frthbyte();
 *		VOID	out();
 *		VOID	outarea();
 *		VOID	outdp();
 *		VOID	outall();
 *		VOID	outdot();
 *		VOID	outbuf();
 *		VOID	outchk();
 *		VOID	outgsd();
 *		VOID	outsym();
 *		VOID	outab();
 *		VOID	outaw();
 *		VOID	outa3b();
 *		VOID	outa4b();
 *		VOID	outaxb();
 *		VOID	outatxb();
 *		VOID	outrb();
 *		VOID	outrw();
 *		VOID	outr3b();
 *		VOID	outr4b();
 *		VOID	outrxb();
 *		VOID	outr11();	JLH
 *		VOID	out_lb();
 *		VOID	out_lw();
 *		VOID	out_l3b();
 *		VOID	out_l4b();
 *		VOID	out_lxb();
 *		VOID	out_rw();
 *		VOID	out_txb();
 *
 *	The module asout.c contains the following local variables:
 *		int	rel[]		relocation data for code/data array
 *		int *	relp		pointer to rel array
 *		int	txt[]		assembled code/data array
 *		int *	txtp		pointer to txt array
 */

#define	 NTXT	16
#define	 NREL	16

char	 txt[NTXT];
char	 rel[NREL];

char	*txtp = &txt[0];
char	*relp = &rel[0];

/*)Function	VOID	outab(v)
 *)Function	VOID	outaw(v)
 *)Function	VOID	outa3b(v)
 *)Function	VOID	outa4b(v)
 *
 *		int	v		assembler data
 *
 *	Dispatch to output routine of 1 to 4 absolute bytes.
 *
 *	local variables:
 *		none
 *
 *	global variables:
 *		none
 *
 *	functions called:
 *		VOID	outatxb()	asout.c
 *
 *	side effects:
 *		Absolute data is processed.
 */

VOID
outab(v)
int v;
{
	outaxb(1, v);
}

VOID
outaw(v)
int v;
{
	outaxb(2, v);
}

VOID
outa3b(v)
int v;
{
	outaxb(3, v);
}

VOID
outa4b(v)
int v;
{
	outaxb(4, v);
}

/*)Function	VOID	outaxb(i, v)
 *
 *		int	i		output byte count
 *		int	v		assembler data
 *
 *	The function outaxb() processes 1 to 4 bytes of
 *	assembled data in absolute format.
 *
 *	local variables:
 *		none
 *
 *	global variables:
 *		sym	dot		defined as sym[0]
 *		int	oflag		-o, generate relocatable output flag
 *		int	pass		assembler pass number
 *
 *	functions called:
 *		VOID	outatxb()	asout.c
 *		VOID	outchk()	asout.c
 *		VOID	out_lxb()	asout.c
 *
 *	side effects:
 *		The current assembly address is incremented by i.
 */

VOID
outaxb(i, v)
int i;
int v;
{
	if (pass == 2) {
		out_lxb(i, v, 0);
		if (oflag) {
			outchk(i, 0);
			outatxb(i, v);
		}
	}
	dot.s_addr += i;
}

/*)Function	VOID	outatxb(i, v)
 *
 *		int	i		number of bytes to process
 *		int	v		assembler data
 *
 *	The function outatxb() outputs i bytes
 *
 *	local variables:
 *		none
 *
 *	global variables:
 *		int	hilo		byte order
 *		char	*txtp		T line output pointer
 *
 *	functions called:
 *		int	lobyte()	asout.c
 *		int	hibyte()	asout.c
 *		int	thrdbyte()	asout.c
 *		int	frthbyte()	asout.c
 *
 *	side effects:
 *		i bytes are placed into the T Line Buffer.
 */

VOID
outatxb(i, v)
int i;
int v;
{
	if (hilo) {
		if (i >= 4) *txtp++ = frthbyte(v);
		if (i >= 3) *txtp++ = thrdbyte(v);
		if (i >= 2) *txtp++ = hibyte(v);
		if (i >= 1) *txtp++ = lobyte(v);
	} else {
		if (i >= 1) *txtp++ = lobyte(v);
		if (i >= 2) *txtp++ = hibyte(v);
		if (i >= 3) *txtp++ = thrdbyte(v);
		if (i >= 4) *txtp++ = frthbyte(v);
	}
}

/*)Function	VOID	outrb(esp, r)
 *)Function	VOID	outrw(esp, r)
 *)Function	VOID	outr3b(esp, r)
 *)Function	VOID	outr4b(esp, r)
 *
 *		expr *	esp		pointer to expr structure
 *		int	r		relocation mode
 *
 *	Dispatch functions for processing relocatable data.
 *
 *	local variables:
 *		none
 *
 *	global variables:
 *		none
 *		
 *	functions called:
 *		int	outrxb()	asout.c
 *
 *	side effects:
 *	        relocatable data processed
 */

VOID
outrb(esp, r)
register struct expr *esp;
int r;
{
	outrxb(1, esp, r);
}

VOID
outrw(esp, r)
register struct expr *esp;
int r;
{
	outrxb(2, esp, r);
}

VOID
outr3b(esp, r)
register struct expr *esp;
int r;
{
	outrxb(3, esp, r);
}

VOID
outr4b(esp, r)
register struct expr *esp;
int r;
{
	outrxb(4, esp, r);
}

/*)Function	VOID	outrxb(i, esp, r)
 *
 *		int	i		output byte count
 *		expr *	esp		pointer to expr structure
 *		int	r		relocation mode
 *
 *	The function outrxb() processes 1 to 4 bytes of generated code
 *	in either absolute or relocatable format dependent upon
 *	the data contained in the expr structure esp.  If the
 *	.REL output is enabled then the appropriate information
 *	is loaded into the txt and rel buffers.
 *
 *	local variables:
 *		int	n		symbol/area reference number
 *
 *	global variables:
 *		int	a_bytes		T Line byte count
 *		sym	dot		defined as sym[0]
 *		int	oflag		-o, generate relocatable output flag
 *		int	pass		assembler pass number
 *		int *	relp		pointer to rel array
 *		
 *	functions called:
 *		int	hibyte()	asout.c
 *		int	lobyte()	asout.c
 *		VOID	outatxb()	asout.c
 *		VOID	outchk()	asout.c
 *		VOID	out_lb()	asout.c
 *		VOID	out_lw()	asout.c
 *		VOID	out_lxb()	asout.c
 *		VOID	out_rw()	asout.c
 *		VOID	out_txb()	asout.c
 *
 *	side effects:
 *		R and T Lines updated.  Listing updated.
 *		The current assembly address is incremented by i.
 */

VOID
outrxb(i, esp, r)
int i;
register struct expr *esp;
int r;
{
	register int n;

	if (pass == 2) {
		if (esp->e_flag==0 && esp->e_base.e_ap==NULL) {
			out_lxb(i,esp->e_addr,0);
			if (oflag) {
				outchk(i,0);
				outatxb(i,esp->e_addr);
			}
		} else {
			if (i == 1) {
				r |= R_BYTE | R_BYTX | esp->e_rlcf;
				if (r & R_MSB) {
					out_lb(hibyte(esp->e_addr),r|R_RELOC|R_HIGH);
				} else {
					out_lb(lobyte(esp->e_addr),r|R_RELOC);
				}
			} else {
				switch(i) {
				default:
				case 2: r |= R_WORD; break;
				case 3: r |= R_3BYTE; break;
				case 4: r |= R_4BYTE; break;
				}
				r |= esp->e_rlcf;
				out_lxb(i,esp->e_addr,r|R_RELOC);
			}
			if (oflag) {
				outchk(a_bytes,4);
				out_txb(a_bytes,esp->e_addr);
				if (esp->e_flag) {
					n = esp->e_base.e_sp->s_ref;
					r |= R_SYM;
				} else {
					n = esp->e_base.e_ap->a_ref;
				}
				*relp++ = r;
				*relp++ = txtp - txt - a_bytes;
				out_rw(n);
			}
		}
	}
	dot.s_addr += i;
}

/*)Function	VOID	outdp(carea, esp)
 *
 *		area *	carea		pointer to current area strcuture
 *		expr *	esp		pointer to expr structure
 *
 *	The function outdp() flushes the output buffer and
 *	outputs paging information to the .REL file.
 *
 *	local variables:
 *		int	n		symbol/area reference number
 *		int	r		relocation mode
 *		int *	relp		pointer to rel array
 *		int *	txtp		pointer to txt array
 *
 *	global variables:
 *		int	a_bytes		T Line byte count
 *		int	oflag		-o, generate relocatable output flag
 *		int	pass		assembler pass number
 *		
 *	functions called:
 *		VOID	outbuf()	asout.c
 *		VOID	outchk()	asout.c
 *		VOID	out_rw()	asout.c
 *		VOID	out_txb()	asout.c
 *
 *	side effects:
 *		Output buffer flushed to .REL fiel.
 *		Paging information dumped to .REL file.
 */

VOID
outdp(carea, esp)
register struct area *carea;
register struct expr *esp;
{
	register int n, r;

	if (oflag && pass==2) {
		outchk(HUGE,HUGE);
		out_txb(a_bytes,carea->a_ref);
		out_txb(a_bytes,esp->e_addr);
		if (esp->e_flag || esp->e_base.e_ap!=NULL) {
			r = R_WORD;
			if (esp->e_flag) {
				n = esp->e_base.e_sp->s_ref;
				r |= R_SYM;
			} else {
				n = esp->e_base.e_ap->a_ref;
			}
			*relp++ = r;
			*relp++ = txtp - txt - a_bytes;
			out_rw(n);
		}
		outbuf("P");
	}
}

/*)Function	VOID	outall()
 *
 *	The function outall() will output any bufferred assembled
 *	data and relocation information (during pass 2 if the .REL
 *	output has been enabled).
 *
 *	local variables:
 *		none
 *
 *	global variables:
 *		int	oflag		-o, generate relocatable output flag
 *		int	pass		assembler pass number
 *
 *	functions called:
 *		VOID	outbuf()	asout.c
 *
 *	side effects:
 *		assembled data and relocation buffers will be cleared.
 */

VOID
outall()
{
	if (oflag && pass==2)
		outbuf("R");
}

/*)Function	VOID	outdot()
 *
 *	The function outdot() outputs information about the
 *	current program counter value (during pass 2 if the .REL
 *	output has been enabled).
 *
 *	local variables:
 *		none
 *
 *	global variables:
 *		int	oflag		-o, generate relocatable output flag
 *		int	pass		assembler pass number
 *
 *	functions called:
 *		int	fprintf()	c_library
 *		VOID	out()		asout.c
 *
 *	side effects:
 *		assembled data and relocation buffers will be cleared.
 */

VOID
outdot()
{
	if (oflag && pass==2) {
		fprintf(ofp, "T");
		out(txt,(int) (txtp-txt));
		fprintf(ofp, "\n");
		fprintf(ofp, "R");
		out(rel,(int) (relp-rel));
		fprintf(ofp, "\n");
		txtp = txt;
		relp = rel;
	}
}

/*)Function	outchk(nt, nr)
 *
 *		int	nr		number of additional relocation words
 *		int	nt		number of additional data words
 *
 *	The function outchk() checks the data and relocation buffers
 *	for space to insert the nt data words and nr relocation words.
 *	If space is not available then output the current data and
 *	initialize the data buffers to receive the new data.
 *
 *	local variables:
 *		area *	ap		pointer to an area structure
 *		int *	relp		pointer to rel array
 *		int *	txtp		pointer to txt array
 *
 *	global variables:
 *		int	a_bytes		T Line byte count
 *		sym	dot		defined as sym[0]
 *
 *	functions called:
 *		VOID	outbuf()	asout.c
 *		VOID	out_rw()	asout.c
 *		VOID	out_txb()	asout.c
 *
 *	side effects:
 *		Data and relocation buffers may be emptied and initialized.
 */

VOID
outchk(nt, nr)
int nt;
int nr;
{
	register struct area *ap;

	if (txtp+nt >= &txt[NTXT] || relp+nr >= &rel[NREL]) {
		outbuf("R");
	}
	if (txtp == txt) {
		out_txb(a_bytes,dot.s_addr);
		if ((ap = dot.s_area) != NULL) {
			*relp++ = R_WORD|R_AREA;
			*relp++ = 0;
			out_rw(ap->a_ref);
		}
	}
}

/*)Function	VOID	outbuf(s)
 *
 *		char *	s		"R" or "P" or ("I" illegal)
 *
 *	The function outbuf() will output any bufferred data
 *	and relocation information to the .REL file.  The output
 *	buffer pointers and counters are initialized.
 *
 *	local variables:
 *		int	rel[]		relocation data for code/data array
 *		int *	relp		pointer to rel array
 *		int	txt[]		assembled code/data array
 *		int *	txtp		pointer to txt array
 *
 *	global variables:
 *		FILE *	ofp		relocation output file handle
 *
 *	functions called:
 *		int	fprintf()	c_library
 *		VOID	out()		asout.c
 *
 *	side effects:
 *		All bufferred data written to .REL file and
 *		buffer pointers and counters initialized.
 */

VOID
outbuf(s)
char *s;
{
	if (txtp > &txt[a_bytes]) {
		fprintf(ofp, "T");
		out(txt,(int) (txtp-txt));
		fprintf(ofp, "\n");
		fprintf(ofp, s);
		out(rel,(int) (relp-rel));
		fprintf(ofp, "\n");
	}
	txtp = txt;
	relp = rel;
}

/*)Function	VOID	outgsd()
 *
 *	The function outgsd() performs the following:
 *	(1)	outputs the .REL file radix
 *	(2)	outputs the header specifying the number
 *		of areas and global symbols
 *	(3)	outputs the module name
 *	(4)	set the reference number and output a symbol line
 *		for all external global variables and absolutes
 *	(5)	output an area name, set reference number and output
 *		a symbol line for all global relocatables in the area.
 *		Repeat this proceedure for all areas.
 *
 *	local variables:
 *		area *	ap		pointer to an area structure
 *		sym *	sp		pointer to a sym structure
 *		int	i		loop counter
 *		int	j		loop counter
 *		int	c		string character value
 *		int	narea		number of code areas
 *		char *	ptr		string pointer
 *		int	nglob		number of global symbols
 *		int	rn		symbol reference number
 *
 *	global variables:
 *		area *	areap		pointer to an area structure
 *		int	hilo		byte order
 *		char	module[]	module name string
 *		sym * symhash[]		array of pointers to NHASH
 *					linked symbol lists
 *		int	xflag		-x, listing radix flag
 *
 *	functions called:
 *		int	fprintf()	c_library
 *		VOID	outarea()	asout.c
 *		VOID	outsym()	asout.c
 *
 *	side effects:
 *		All symbols are given reference numbers, all symbol
 *		and area information is output to the .REL file.
 */

VOID
outgsd()
{
	register struct area *ap;
	register struct sym  *sp;
	register int i, j;
	char *ptr;
	int narea, nglob, rn;

	/*
	 * Number of areas
	 */
	narea = areap->a_ref + 1;

	/*
	 * Number of global references/absolutes
	 */
	nglob = 0;
	for (i = 0; i < NHASH; ++i) {
		sp = symhash[i];
		while (sp) {
			if (sp->s_flag&S_GBL)
				++nglob;
			sp = sp->s_sp;
		}
	}

	/*
	 * Output Radix and number of areas and symbols
	 */
	if (xflag == 0) {
		fprintf(ofp, "X%c%d\n", hilo ? 'H' : 'L', a_bytes);
		fprintf(ofp, "H %X areas %X global symbols\n", narea, nglob);
	} else
	if (xflag == 1) {
		fprintf(ofp, "Q%c%d\n", hilo ? 'H' : 'L', a_bytes);
		fprintf(ofp, "H %o areas %o global symbols\n", narea, nglob);
	} else
	if (xflag == 2) {
		fprintf(ofp, "D%c%d\n", hilo ? 'H' : 'L', a_bytes);
		fprintf(ofp, "H %u areas %u global symbols\n", narea, nglob);
	}		

	/*
	 * Module name
	 */
	if (module[0]) {
		fprintf(ofp, "M ");
		ptr = &module[0];
		fprintf(ofp, "%s\n", ptr);
	}

	/*
	 * Global references and absolutes.
	 */
	rn = 0;
	for (i=0; i<NHASH; ++i) {
		sp = symhash[i];
		while (sp) {
			if (sp->s_area==NULL && sp->s_flag&S_GBL) {
				sp->s_ref = rn++;
				outsym(sp);
			}
			sp = sp->s_sp;
		}
	}

	/*
	 * Global relocatables.
	 */
	for (i=0; i<narea; ++i) {
		ap = areap;
		while (ap->a_ref != i)
			ap = ap->a_ap;
		outarea(ap);
		for (j=0; j<NHASH; ++j) {
			sp = symhash[j];
			while (sp) {
				if (sp->s_area==ap && sp->s_flag&S_GBL) {
					sp->s_ref = rn++;
					outsym(sp);
				}
				sp = sp->s_sp;
			}
		}
	}
}

/*)Function	VOID	outarea(ap)
 *
 *		area *	ap		pointer to an area structure
 *
 *	The function outarea()	outputs the A line to the .REL
 *	file.  The A line contains the area's name, size, and
 *	attributes.
 *
 *	local variables:
 *		char *	ptr		pointer to area id string
 *
 *	global variables:
 *		FILE *	ofp		relocation output file handle
 *		int	xflag		-x, listing radix flag
 *
 *	functions called:
 *		int	fprintf()	c_library
 *
 *	side effects:
 *		The A line is sent to the .REL file.
 */

VOID
outarea(ap)
register struct area *ap;
{
	register char *ptr;

	fprintf(ofp, "A ");
	ptr = &ap->a_id[0];
	fprintf(ofp, "%s", ptr);
	if (xflag == 0) {
		fprintf(ofp, " size %X flags %X\n", ap->a_size, ap->a_flag);
	} else
	if (xflag == 1) {
		fprintf(ofp, " size %o flags %o\n", ap->a_size, ap->a_flag);
	} else
	if (xflag == 2) {
		fprintf(ofp, " size %u flags %u\n", ap->a_size, ap->a_flag);
	}
}

/*)Function	VOID	outsym(sp)
 *
 *		sym *	sp		pointer to a sym structure
 *
 *	The function outsym() outputs the S line to the .REL
 *	file.  The S line contains the symbols name and whether the
 *	the symbol is defined or referenced.
 *
 *	local variables:
 *		char *	ptr		pointer to symbol id string
 *		int	s_addr		(int) truncated to 2-bytes
 *
 *	global variables:
 *		int	a_bytes		argument size in bytes
 *		FILE *	ofp		relocation output file handle
 *		int	xflag		-x, listing radix flag
 *
 *	functions called:
 *		int	fprintf()	c_library
 *
 *	side effects:
 *		The S line is sent to the .REL file.
 */

VOID
outsym(sp)
register struct sym *sp;
{
	register char *frmt, *ptr;
	register int s_addr;

	/*
	 * Truncate (int) to N-Bytes
	 */
	s_addr = sp->s_addr & a_mask;

	fprintf(ofp, "S ");
	ptr = &sp->s_id[0];	/* JLH */
	fprintf(ofp, "%s", ptr);
	fprintf(ofp, " %s", sp->s_type==S_NEW ? "Ref" : "Def");
	switch(xflag) {
	default:
	case 0:
		switch(a_bytes) {
		default:
		case 2:	frmt = "%04X\n"; break;
		case 3:	frmt = "%06X\n"; break;
		case 4:	frmt = "%08X\n"; break;
		}
		break;

	case 1:
		switch(a_bytes) {
		default:
		case 2:	frmt = "%06o\n"; break;
		case 3:	frmt = "%08o\n"; break;
		case 4:	frmt = "%011o\n"; break;
		}
		break;

	case 2:
		switch(a_bytes) {
		default:
		case 2:	frmt = "%05u\n"; break;
		case 3:	frmt = "%08u\n"; break;
		case 4:	frmt = "%010u\n"; break;
		}
		break;
	}
	fprintf(ofp, frmt, s_addr);
}

/*)Function	VOID	out(p, n)
 *
 *		int	n		number of words to output
 *		int *	p		pointer to data words
 *
 *	The function out() outputs the data words to the .REL file
 *	int the specified radix.
 *
 *	local variables:
 *		none
 *
 *	global variables:
 *		FILE *	ofp		relocation output file handle
 *		int	xflag		-x, listing radix flag
 *
 *	functions called:
 *		int	fprintf()	c_library
 *
 *	side effects:
 *		Data is sent to the .REL file.
 */

VOID
out(p, n)
register char *p;
register int n;
{
	while (n--) {
		if (xflag == 0) {
			fprintf(ofp, " %02X", (*p++)&0377);
		} else
		if (xflag == 1) {
			fprintf(ofp, " %03o", (*p++)&0377);
		} else
		if (xflag == 2) {
			fprintf(ofp, " %03u", (*p++)&0377);
		}
	}
}

/*)Function	VOID	out_lb(v, t)
 *
 *		int	v		assembled data
 *		int	t		relocation type
 *
 *	The function out_lb() copies the assembled data and
 *	its relocation type to the list data buffers.
 *
 *	local variables:
 *		none
 *
 *	global variables:
 *		int *	cp		pointer to assembler output array cb[]
 *		int *	cpt		pointer to assembler relocation type
 *					output array cbt[]
 *
 *	functions called:
 *		none
 *
 *	side effects:
 *		Pointers to data and relocation buffers incremented by 1.
 */

VOID
out_lb(v, t)
register int v, t;
{
	if (cp < &cb[NCODE]) {
		*cp++ = v;
		*cpt++ = t;
	}
}

/*)Function	VOID	out_lw(v, t)
 *)Function	VOID	out_l3b(v, t)
 *)Function	VOID	out_l4b(v, t)
 *
 *		int	v		assembled data
 *		int	t		relocation type
 *
 *	Dispatch functions for processing listing data.
 *
 *	local variables:
 *		none
 *
 *	global variables:
 *		none
 *
 *	functions called:
 *		none
 *
 *	side effects:
 *		Listing data processed.
 */

VOID
out_lw(v, t)
register int v, t;
{
	out_lxb(2, v, t);
}

VOID
out_l3b(v, t)
register int v, t;
{
	out_lxb(3, v, t);
}

VOID
out_l4b(v, t)
register int v, t;
{
	out_lxb(4, v, t);
}

/*)Function	VOID	out_lxb(i, v, t)
 *
 *		int	i		output byte count
 *		int	v		assembled data
 *		int	t		relocation type
 *
 *	Dispatch function for list processing.
 *
 *	local variables:
 *		none
 *
 *	global variables:
 *		int	hilo		byte order
 *
 *	functions called:
 *		VOID	out_lb()	asout.c
 *		VOID	out_lw()	asout.c
 *
 *	side effects:
 *		i list bytes are processed.
 */

VOID
out_lxb(i, v, t)
register int i, v, t;
{
	if (hilo) {
		if (i >= 4) out_lb(frthbyte(v),t&R_RELOC ? t|R_BYT4 : 0);
		if (i >= 3) out_lb(thrdbyte(v),t&R_RELOC ? t|R_BYT3 : 0);
		if (i >= 2) out_lb(hibyte(v),t&R_RELOC ? t|R_HIGH : 0);
		if (i >= 1) out_lb(lobyte(v),t);
	} else {
		if (i >= 1) out_lb(lobyte(v),t);
		if (i >= 2) out_lb(hibyte(v),t&R_RELOC ? t|R_HIGH : 0);
		if (i >= 3) out_lb(thrdbyte(v),t&R_RELOC ? t|R_BYT3 : 0);
		if (i >= 4) out_lb(frthbyte(v),t&R_RELOC ? t|R_BYT4 : 0);
	}
}

/*)Function	VOID	out_rw(v)
 *
 *		int	v		assembled data
 *
 *	The function out_rw() outputs the relocation (R)
 *	data word as two bytes ordered according to hilo.
 *
 *	local variables:
 *		int *	relp		pointer to rel array
 *
 *	global variables:
 *		int	hilo		byte order
 *
 *	functions called:
 *		int	lobyte()	asout.c
 *		int	hibyte()	asout.c
 *
 *	side effects:
 *		Pointer to relocation buffer incremented by 2.
 */

VOID
out_rw(v)
register int v;
{
	if (hilo) {
		*relp++ = hibyte(v);
		*relp++ = lobyte(v);
	} else {
		*relp++ = lobyte(v);
		*relp++ = hibyte(v);
	}
}

/*)Function	VOID	out_txb(i, v)
 *
 *		int	i		T Line byte count
 *		int	v		data word
 *
 *	The function out_txb() outputs the text (T)
 *	as a_bytes bytes ordered according to hilo.
 *
 *	local variables:
 *		int *	txtp		pointer to txt array
 *
 *	global variables:
 *		int	hilo		byte order
 *
 *	functions called:
 *		int	lobyte()	asout.c
 *		int	hibyte()	asout.c
 *		int	thrdbyte()	asout.c
 *		int	frthbyte()	asout.c
 *
 *	side effects:
 *		T Line buffer updated.
 */

VOID
out_txb(i, v)
register int i, v;
{
	if (hilo) {
		if (i >= 4) *txtp++ = frthbyte(v);
		if (i >= 3) *txtp++ = thrdbyte(v);
		if (i >= 2) *txtp++ = hibyte(v);
		if (i >= 1) *txtp++ = lobyte(v);
	} else {
		if (i >= 1) *txtp++ = lobyte(v);
		if (i >= 2) *txtp++ = hibyte(v);
		if (i >= 3) *txtp++ = thrdbyte(v);
		if (i >= 4) *txtp++ = frthbyte(v);
	}
}

/*)Function	int	lobyte(v)
 *)Function	int	hibyte(v)
 *)Function	int	thrdbyte(v)
 *)Function	int	frthbyte(v)
 *
 *		int	v		assembled data
 *
 *	These functions return the 1st, 2nd, 3rd, or 4th byte
 *	of integer v.
 *
 *	local variables:
 *		none
 *
 *	global variables:
 *		none
 *
 *	functions called:
 *		none
 *
 *	side effects:
 *		none
 */

int
lobyte(v)
int v;
{
	return (v&0377);
}

int
hibyte(v)
int v;
{
	return ((v>>8)&0377);
}

int
thrdbyte(v)
int v;
{
	return ((v>>16)&0377);
}

int
frthbyte(v)
int v;
{
	return ((v>>24)&0377);
}

/*)Function	VOID	outr11(esp, op)
 *
 *		expr *	esp		pointer to expr structure
 *		int	op		opcode
 *
 *	The function outr11() processes a word of generated code
 *	in either absolute or relocatable format dependent upon
 *	the data contained in the expr structure esp.  If the
 *	.REL output is enabled then the appropriate information
 *	is loaded into the txt and rel buffers.  The code is output
 *	in a special format to the linker to allow relocation and
 *	merging of the opcode and an 11 bit paged address as required
 *	by the 8051 architecture.
 *
 *	This function based on code by
 *
 *		John L. Hartman
 *		jhartman@compuserve.com
 *
 *	local variables:
 *		int	n		symbol/area reference number
 *		int	r		relocation mode
 *
 *	global variables:
 *		sym	dot		defined as sym[0]
 *		int	oflag		-o, generate relocatable output flag
 *		int	pass		assembler pass number
 *		int *	relp		pointer to rel array
 *		int *	txtp		pointer to txt array
 *		
 *	functions called:
 *		VOID	outchk()	asout.c
 *		VOID	out_rw()	asout.c
 *		VOID	out_txb()	asout.c
 *
 *	side effects:
 *		The current assembly address is incremented by 2.
 */

VOID
outr11(esp, op)
register struct expr *esp;
int op;
{
	register int n, r;

	if (pass == 2) {
		if (esp->e_flag==0 && esp->e_base.e_ap==NULL) {
			/*
			 * Absolute Destination
			 *
			 * Use the global symbol '.__.ABS.'
			 * of value zero and force the assembler
			 * to use this absolute constant as the
			 * base value for the relocation.
			 */
			esp->e_flag = 1;
			esp->e_base.e_sp = &sym[1];
		}
		/*
		 * Relocatable Destination.  Build THREE
		 * byte output: relocatable word, followed
		 * by op-code.  Linker will combine them.
		 */
		r = R_J11 | esp->e_rlcf;
		n = ((esp->e_addr & 0x0700) >> 3) | (op & 0x1F);
		n = (n << 8) | (esp->e_addr & 0xFF);
		out_lxb(2,n,r|R_RELOC);
		if (oflag) {
			outchk(a_bytes+1, 4);
			out_txb(a_bytes,esp->e_addr);
			*txtp++ = op;

			if (esp->e_flag) {
				n = esp->e_base.e_sp->s_ref;
				r |= R_SYM;
			} else {
				n = esp->e_base.e_ap->a_ref;
			}
			*relp++ = r;
			*relp++ = txtp - txt - (a_bytes + 1);
			out_rw(n);
		}
	}
	dot.s_addr += 2;
}

/*)Function	VOID	outr19(esp, op)
 *
 *		expr *	esp		pointer to expr structure
 *		int	op		opcode
 *
 *	The function outr19() processes 3 bytes of generated code
 *	in either absolute or relocatable format dependent upon
 *	the data contained in the expr structure esp.  If the
 *	.REL output is enabled then the appropriate information
 *	is loaded into the txt and rel buffers.  The code is output
 *	in a special format to the linker to allow relocation and
 *	merging of the opcode and a 19 bit paged address as required
 *	by the DS80C390 architecture.
 *
 *	This function based on code by
 *
 *		Bill McKinnon (BM)
 *		w_mckinnon@conknet.com
 *
 *		John L. Hartman
 *		jhartman@compuserve.com
 *
 *	local variables:
 *		int	n		symbol/area reference number
 *		int	r		relocation mode
 *
 *	global variables:
 *		sym	dot		defined as sym[0]
 *		int	oflag		-o, generate relocatable output flag
 *		int	pass		assembler pass number
 *		int *	relp		pointer to rel array
 *		int *	txtp		pointer to txt array
 *		
 *	functions called:
 *		VOID	outchk()	asout.c
 *		VOID	out_rw()	asout.c
 *		VOID	out_txb()	asout.c
 *
 *	side effects:
 *		The current assembly address is incremented by 3.
 */

VOID
outr19(esp, op)
register struct expr *esp;
int op;
{
	register int n, r;

	if (pass == 2) {
		if (esp->e_flag==0 && esp->e_base.e_ap==NULL) {
			/*
			 * Absolute Destination
			 *
			 * Use the global symbol '.__.ABS.'
			 * of value zero and force the assembler
			 * to use this absolute constant as the
			 * base value for the relocation.
			 */
			esp->e_flag = 1;
			esp->e_base.e_sp = &sym[1];
		}
		/*
		 * Relocatable Destination.  Build THREE
		 * byte output: relocatable word, followed
		 * by op-code.  Linker will combine them.
		 */
		r = R_J19 | esp->e_rlcf;
		n = ((esp->e_addr & 0x070000) >> 11) | (op & 0x1F);
		n = (n << 16) | (esp->e_addr & 0xFFFF);
		out_lxb(3,n,r|R_RELOC);
		if (oflag) {
			outchk(a_bytes+1, 4);
			out_txb(a_bytes,esp->e_addr);
			*txtp++ = op;

			if (esp->e_flag) {
				n = esp->e_base.e_sp->s_ref;
				r |= R_SYM;
			} else {
				n = esp->e_base.e_ap->a_ref;
			}
			*relp++ = r;
			*relp++ = txtp - txt - (a_bytes + 1);
			out_rw(n);
		}
	}
	dot.s_addr += 3;
}

