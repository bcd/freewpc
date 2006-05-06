/* lkmain.c */

/*
 * (C) Copyright 1989-2000
 * All Rights Reserved
 *
 * Alan R. Baldwin
 * 721 Berkeley St.
 * Kent, Ohio  44240
 */

#include <stdio.h>
#include <string.h>

#ifdef WIN32
#include <stdlib.h>
#else
#include <alloc.h>
#endif

#include "aslink.h"

/*)Module	lkmain.c
 *
 *	The module lkmain.c contains the functions which
 *	(1) input the linker options, parameters, and specifications
 *	(2) perform a two pass link
 *	(3) produce the appropriate linked data output and/or
 *	    link map file and/or relocated listing files.
 *
 *	lkmain.c contains the following functions:
 *		FILE *	afile()
 *		VOID	bassav()
 *		VOID	gblsav()
 *		VOID	link()
 *		VOID	lkexit()
 *		int	fndidx()
 *		int	main()
 *		VOID	map()
 *		int	parse()
 *		VOID	doparse()
 *		VOID	setbas()
 *		VOID	setgbl()
 *		VOID	usage()
 *
 *	lkmain.c contains the following local variables:
 *		char *	usetext[]	array of pointers to the
 *					command option tect lines
 *
 */

/*)Function	int	main(argc,argv)
 *
 *		int	argc		number of command line arguments + 1
 *		char *	argv[]		array of pointers to the command line
 *					arguments
 *
 *	The function main() evaluates the command line arguments to
 *	determine if the linker parameters are to input through 'stdin'
 *	or read from a command file.  The functiond getline() and parse()
 *	are to input and evaluate the linker parameters.  The linking process
 *	proceeds by making the first pass through each .rel file in the order
 *	presented to the linker.  At the end of the first pass the setbase(),
 *	lnkarea(), setgbl(), and symdef() functions are called to evaluate
 *	the base address terms, link all areas, define global variables,
 *	and look for undefined symbols.  Following these routines a linker
 *	map file may be produced and the linker output files may be opened.
 *	The second pass through the .rel files will output the linked data
 *	in one of the four supported formats.
 *
 *	local variables:
 *		char *	frmt		temporary format specifier
 *		int	c		character from argument string
 *		int	i		loop counter
 *		int	j		loop counter
 *		int	k		loop counter
 *
 *	global variables:
 *				 	text line in ib[]
 *		lfile	*cfp		The pointer *cfp points to the
 *				 	current lfile structure
 *		char	ctype[]		array of character types, one per
 *				 	ASCII character
 *		lfile	*filep	 	The pointer *filep points to the
 *				 	beginning of a linked list of
 *				 	lfile structures.
 *		head	*hp		Pointer to the current
 *				 	head structure
 *		char	ib[NINPUT]	.rel file text line
 *		char	*ip		pointer into the .rel file
 *		lfile	*linkp		pointer to first lfile structure
 *				 	containing an input .rel file
 *				 	specification
 *		int	lkerr		error flag
 *		int	mflag		Map output flag
 *		int	oflag		Output file type flag
 *		FILE	*ofp		Output file handle
 *				 	for word formats
 *		FILE	*ofph		Output file handle
 *				 	for high byte format
 *		FILE	*ofpl		Output file handle
 *				 	for low byte format
 *		int	pass		linker pass number
 *		int	pflag		print linker command file flag
 *		int	radix		current number conversion radix
 *		FILE	*sfp		The file handle sfp points to the
 *				 	currently open file
 *		lfile	*startp		asmlnk startup file structure
 *		FILE *	stdout		c_library
 *
 *	functions called:
 *		FILE *	afile()		lkmain.c
 *		int	fclose()	c_library
 *		int	fprintf()	c_library
 *		int	getline()	lklex.c
 *		VOID	library()	lklibr.c
 *		VOID	link()		lkmain.c
 *		VOID	lkexit()	lkmain.c
 *		VOID	lnkarea()	lkarea.c
 *		VOID	map()		lkmain.c
 *		VOID	new()		lksym.c
 *		int	parse()		lkmain.c
 *		VOID	reloc()		lkreloc.c
 *		VOID	search()	lklibr.c
 *		VOID	setbas()	lkmain.c
 *		VOID	setgbl()	lkmain.c
 *		char *	sprintf()	c_library
 *		VOID	symdef()	lksym.c
 *		VOID	usage()		lkmain.c
 *
 *	side effects:
 *		Completion of main() completes the linking process
 *		and may produce a map file (.map) and/or a linked
 *		data files (.ihx or .s19) and/or one or more
 *		relocated listing files (.rst).
 */

int
main(argc, argv)
int argc;
char *argv[];
{
	register int c, i, j, k;
	register char *frmt;

	fprintf(stdout, "\n");

	startp = (struct lfile *) new (sizeof (struct lfile));
	startp->f_idp = "";

	pflag = 1;

	for(i=1; i<argc; i++) {
		ip = ib;
		if(argv[i][0] == '-') {
			j = i;
			k = 1;
			while((c = argv[j][k]) != '\0') {
				ip = ib;
				sprintf(ip, "-%c", c);
				switch(c) {

				/*
				 * Options with arguments
				 */
				case 'b':
				case 'B':

				case 'g':
				case 'G':

				case 'k':
				case 'K':

				case 'l':
				case 'L':

				case 'f':
				case 'F':
					strcat(ip, " ");
					strcat(ip, argv[++i]);
					break;

				/*
				 * Preprocess these commands
				 */
				case 'n':
				case 'N':
					pflag = 0;
					break;

				case 'p':
				case 'P':
					pflag = 1;
					break;

				/*
				 * Options without arguments
				 */
				default:
					break;
				}
				if(pflag)
					fprintf(stdout, "ASlink >> %s\n", ip);
				parse();
				k++;
			}
		} else {
			strcpy(ip, argv[i]);
			if(pflag)
				fprintf(stdout, "ASlink >> %s\n", ip);
			parse();
		}
	}

	if (linkp == NULL)
		usage(ER_FATAL);

	syminit();
	for (pass=0; pass<2; ++pass) {
		cfp = NULL;
		sfp = NULL;
		filep = linkp;
		hp = NULL;
		radix = 10;

		while (getline()) {
			ip = ib;
			link();
		}
		if (pass == 0) {
			/*
			 * Search libraries for global symbols
			 */
			search();
			/*
			 * Set area base addresses.
			 */
			setbas();
			/*
			 * Link all area addresses.
			 */
			lnkarea();
			/*
			 * Process global definitions.
			 */
			setgbl();
			/*
			 * Check for undefined globals.
			 */
			symdef(stderr);
			/*
			 * Output Link Map.
			 */
			if (mflag)
				map();
			/*
			 * Open output file
			 */
			if (oflag == 1) {
				switch(a_bytes) {
				default:
				case 2: frmt = "ihx"; break;
				case 3:
				case 4: frmt = "i86"; break;
				}
				ofp = afile(linkp->f_idp, frmt, 1);
				if (ofp == NULL) {
					lkexit(ER_FATAL);
				}
			} else
			if (oflag == 2) {
				switch(a_bytes) {
				default:
				case 2: frmt = "s19"; break;
				case 3: frmt = "s28"; break;
				case 4: frmt = "s37"; break;
				}
				ofp = afile(linkp->f_idp, frmt, 1);
				if (ofp == NULL) {
					lkexit(ER_FATAL);
				}
			}
		} else {
			/*
			 * Link in library files
			 */
			library();
			reloc('E');
		}
	}
	lkexit(lkerr ? ER_ERROR : ER_NONE);
	return(0);
}

/*)Function	VOID	lkexit(i)
 *
 *			int	i	exit code
 *
 *	The function lkexit() explicitly closes all open
 *	files and then terminates the program.
 *
 *	local variables:
 *		none
 *
 *	global variables:
 *		FILE *	mfp		file handle for .map
 *		FILE *	ofp		file handle for .ihx/.s19
 *		FILE *	rfp		file hanlde for .rst
 *		FILE *	sfp		file handle for .rel
 *		FILE *	tfp		file handle for .lst
 *
 *	functions called:
 *		int	fclose()	c_library
 *		VOID	exit()		c_library
 *
 *	side effects:
 *		All files closed. Program terminates.
 */

VOID
lkexit(i)
int i;
{
	if (mfp != NULL) fclose(mfp);
	if (ofp != NULL) fclose(ofp);
	if (rfp != NULL) fclose(rfp);
	if (sfp != NULL) { if (sfp != stdin) fclose(sfp); }
	if (tfp != NULL) fclose(tfp);
	exit(i);
}

/*)Function	link()
 *
 *	The function link() evaluates the directives for each line of
 *	text read from the .rel file(s).  The valid directives processed
 *	are:
 *		X, D, Q, H, M, A, S, T, R, and P.
 *
 *	local variables:
 *		int	c		first non blank character of a line
 *
 *	global variables:
 *		head	*headp		The pointer to the first
 *				 	head structure of a linked list
 *		head	*hp		Pointer to the current
 *				 	head structure
 *		sdp	sdp		Base Paged structure
 *		int	a_bytes		T Line address bytes
 *		int	hilo		Byte ordering
 *		int	pass		linker pass number
 *		int	radix		current number conversion radix
 *
 *	functions called:
 *		char	endline()	lklex.c
 *		int	get()		lklex.c
 *		VOID	module()	lkhead.c
 *		VOID	newarea()	lkarea.c
 *		VOID	newhead()	lkhead.c
 *		sym *	newsym()	lksym.c
 *		VOID	reloc()		lkreloc.c
 *
 *	side effects:
 *		Head, area, and symbol structures are created and
 *		the radix is set as the .rel file(s) are read.
 */

VOID
link()
{
	register int c;

	if ((c=endline()) == 0) { return; }
	switch (c) {

	case 'X':
		radix = 16;
		break;

	case 'D':
		radix = 10;
		break;

	case 'Q':
		radix = 8;
		break;

	case 'H':
		if (pass == 0) {
			newhead();
		} else {
			if (hp == 0) {
				hp = headp;
			} else {
				hp = hp->h_hp;
			}
		}
		sdp.s_area = NULL;
		sdp.s_areax = NULL;
		sdp.s_addr = 0;
		break;

	case 'M':
		if (pass == 0)
			module();
		break;

	case 'A':
		if (pass == 0)
			newarea();
		if (sdp.s_area == NULL) {
			sdp.s_area = areap;
			sdp.s_areax = areap->a_axp;
			sdp.s_addr = 0;
		}
		break;

	case 'S':
		if (pass == 0)
			newsym();
		break;

	case 'T':
	case 'R':
	case 'P':
		if (pass == 0)
			break;
		reloc(c);
		break;

	default:
		break;
	}
	if (c == 'X' || c == 'D' || c == 'Q') {
		while ((c = get()) != 0) {
			switch(c) {
			case 'H':
				hilo = 1;
				break;

			case 'L':
				hilo = 0;
				break;

			case '2':
				a_bytes = 2;
				break;

			case '3':
				a_bytes = 3;
				break;

			case '4':
				a_bytes = 4;
				break;

			default:
				break;
			}
		}
		switch(a_bytes) {
		default:
			a_bytes = 2;
		case 2:
			a_mask = 0x0000FFFF;
			s_mask = 0x00008000;
			v_mask = 0x00007FFF;
			break;

		case 3:
			a_mask = 0x00FFFFFF;
			s_mask = 0x00800000;
			v_mask = 0x007FFFFF;
			break;

		case 4:
			a_mask = 0xFFFFFFFF;
			s_mask = 0x80000000;
			v_mask = 0x7FFFFFFF;
			break;
		}
	}
}

/*)Function	VOID	map()
 *
 *	The function map() opens the output map file and calls the various
 *	routines to
 *	(1) output the variables in each area,
 *	(2) list the files processed with module names,
 *	(3) list the libraries file processed,
 *	(4) list base address definitions,
 *	(5) list global variable definitions, and
 *	(6) list any undefined variables.
 *
 *	local variables:
 *		int 	i		counter
 *		head *	hdp		pointer to head structure
 *		lbfile *lbfh		pointer to library file structure
 *
 *	global variables:
 *		area	*ap		Pointer to the current
 *				 	area structure
 *		area	*areap		The pointer to the first
 *				 	area structure of a linked list
 *		base	*basep		The pointer to the first
 *				 	base structure
 *		base	*bsp		Pointer to the current
 *				 	base structure
 *		lfile	*filep	 	The pointer *filep points to the
 *				 	beginning of a linked list of
 *				 	lfile structures.
 *		globl	*globlp		The pointer to the first
 *				 	globl structure
 *		globl	*gsp		Pointer to the current
 *				 	globl structure
 *		head	*headp		The pointer to the first
 *				 	head structure of a linked list
 *		lbfile	*lbfhead	The pointer to the first
 *					lbfile structure of a linked list
 *		lfile	*linkp		pointer to first lfile structure
 *				 	containing an input REL file
 *				 	specification
 *		int	lop		current line number on page
 *		FILE	*mfp		Map output file handle
 *		int	page		current page number
 *
 *	functions called:
 *		FILE *	afile()		lkmain.c
 *		int	fprintf()	c_library
 *		VOID	lkexit()	lkmain.c
 *		VOID	lstarea()	lklist.c
 *		VOID	newpag()	lklist.c
 *		VOID	symdef()	lksym.c
 *
 *	side effects:
 *		The map file is created.
 */

VOID
map()
{
	register int i;
	register struct head *hdp;
	register struct lbfile *lbfh;

	/*
	 * Open Map File
	 */
	mfp = afile(linkp->f_idp, "map", 1);
	if (mfp == NULL) {
		lkexit(ER_FATAL);
	}

	/*
	 * Output Map Area Lists
	 */
	page = 0;
	lop  = NLPP;
	ap = areap;
	while (ap) {
		lstarea(ap);
		ap = ap->a_ap;
	}
	/*
	 * List Linked Files
	 */
	newpag(mfp);
	fprintf(mfp,
"\nFiles Linked                              [ module(s) ]\n\n");
	hdp = headp;
	filep = linkp;
	while (filep) {
		fprintf(mfp, "%-40.40s  [ ", filep->f_idp);
		i = 0;
		while ((hdp != NULL) && (hdp->h_lfile == filep)) {
			if (i)
				fprintf(mfp, ",\n%44s", "");
			fprintf(mfp, "%-.32s", hdp->m_id);
			hdp = hdp->h_hp;
			i++;
		}
		if (i)
			fprintf(mfp, " ]");
		fprintf(mfp, "\n");
		filep = filep->f_flp;
	}
	fprintf(mfp, "\n");
	/*
	 * List Linked Libraries
	 */
	if (lbfhead != NULL) {
		fprintf(mfp,
"\nLibraries Linked                          [ object file ]\n\n");
		for (lbfh=lbfhead; lbfh; lbfh=lbfh->next) {
			fprintf(mfp, "%-40.40s  [ %-.32s ]\n",
				lbfh->libspc, lbfh->relfil);
		}
		fprintf(mfp, "\n");
	}
	/*
	 * List Base Address Definitions
	 */
	if (basep) {
		newpag(mfp);
		fprintf(mfp, "\nUser Base Address Definitions\n\n");
		bsp = basep;
		while (bsp) {
			fprintf(mfp, "%s\n", bsp->b_strp);
			bsp = bsp->b_base;
		}
	}
	/*
	 * List Global Definitions
	 */
	if (globlp) {
		newpag(mfp);
		fprintf(mfp, "\nUser Global Definitions\n\n");
		gsp = globlp;
		while (gsp) {
			fprintf(mfp, "%s\n", gsp->g_strp);
			gsp = gsp->g_globl;
		}
	}
	fprintf(mfp, "\n\f");
	symdef(mfp);
}

/*)Function	int	parse()
 *
 *	The function parse() evaluates all command line or file input
 *	linker directives and updates the appropriate variables.
 *
 *	local variables:
 *		int	c		character value
 *		int	idx		string index
 *		int	sv_type		save type of processing
 *		char	*p;		string pointer
 *		char	fid[]		file id string
 *
 *	global variables:
 *		char	ctype[]		array of character types, one per
 *				 	ASCII character
 *		lfile	*lfp		pointer to current lfile structure
 *				 	being processed by parse()
 *		lfile	*linkp		pointer to first lfile structure
 *				 	containing an input REL file
 *				 	specification
 *		int	mflag		Map output flag
 *		int	oflag		Output file type flag
 *		int	objflg		Linked file/library output object flag
 *		int	pflag		print linker command file flag
 *		FILE *	stderr		c_library
 *		int	uflag		Relocated listing flag
 *		int	xflag		Map file radix type flag
 *		int	wflag		Wide listing format
 *		int	zflag		Enable symbol case sensitivity
 *
 *	Functions called:
 *		VOID	addlib()	lklibr.c
 *		VOID	addpath()	lklibr.c
 *		VOID	bassav()	lkmain.c
 *		VOID	doparse()	lkmain.c
 *		int	fprintf()	c_library
 *		VOID	gblsav()	lkmain.c
 *		VOID	getfid()	lklex.c
 *		int	get()		lklex.c
 *		int	getnb()		lklex.c
 *		VOID	lkexit()	lkmain.c
 *		char *	strsto()	lksym.c
 *		int	strlen()	c_library
 *
 *	side effects:
 *		Various linker flags are updated and the linked
 *		structure lfile is created.
 */

int
parse()
{
	register int c, idx;
	register char *p;
	int sv_type;
	char fid[FILSPC+FILSPC];

	while ((c = getnb()) != 0) {
		if ( c == '-') {
			while (ctype[c=get()] & LETTER) {
				switch(c) {

				case 'c':
				case 'C':
					if (startp->f_type != 0)
						break;
					startp->f_type = F_STD;
					doparse();
					return(0);

				case 'f':
				case 'F':
					if (startp->f_type == F_LNK)
						return(0);
					unget(getnb());
					if (*ip == 0)
						usage(ER_FATAL);
					sv_type = startp->f_type;
					startp->f_idp = ip;
					startp->f_idx = fndidx(ip);
					startp->f_type = F_LNK;
					doparse();
					if (sv_type == F_STD) {
						cfp = NULL;
						sfp = NULL;
						startp->f_type = F_STD;
						filep = startp;
					}
					return(0);

				case 'i':
				case 'I':
					oflag = 1;
					break;

				case 's':
				case 'S':
					oflag = 2;
					break;

				case 'o':
				case 'O':
					objflg = 0;
					break;

				case 'v':
				case 'V':
					objflg = 1;
					break;

				case 'm':
				case 'M':
					++mflag;
					break;

				case 'u':
				case 'U':
					uflag = 1;
					break;

				case 'x':
				case 'X':
					xflag = 0;
					break;

				case 'q':
				case 'Q':
					xflag = 1;
					break;

				case 'd':
				case 'D':
					xflag = 2;
					break;

				case 'e':
				case 'E':
					return(1);

				case 'n':
				case 'N':
					pflag = 0;
					break;

				case 'p':
				case 'P':
					pflag = 1;
					break;

				case 'b':
				case 'B':
					bassav();
					return(0);

				case 'g':
				case 'G':
					gblsav();
					return(0);

				case 'k':
				case 'K':
					addpath();
					return(0);

				case 'l':
				case 'L':
					addlib();
					return(0);

				case 'w':
				case 'W':
					++wflag;
					break;

				case 'z':
				case 'Z':
					++zflag;
					break;

				default:
					fprintf(stderr,
					    "Unkown option -%c ignored\n", c);
					break;
				}
			}
		} else
		if (ctype[c] != ILL) {
			if (linkp == NULL) {
				linkp = (struct lfile *)
						new (sizeof (struct lfile));
				lfp = linkp;
			} else {
				lfp->f_flp = (struct lfile *)
						new (sizeof (struct lfile));
				lfp = lfp->f_flp;
			}
			/*
			 * Copy Path from .LNK file
			 */
			idx = startp->f_idx;
			strncpy(fid, startp->f_idp, idx);
			/*
			 * Concatenate the .REL file spec
			 */
			getfid(fid + idx, c);
			/*
			 * If .REL file spec has a path
			 * 	use it
			 * else
			 *	use path of .LNK file
			 */
			if (fndidx(fid + idx) != 0) {
				p = fid + idx;
			} else {
				p = fid;
			}
			/*
			 * Save .REL file specification
			 */
			lfp->f_idp = strsto(p);
			lfp->f_idx = fndidx(p);
			lfp->f_type = F_REL;
			lfp->f_obj = objflg;
		} else {
			fprintf(stderr, "Invalid input");
			lkexit(ER_FATAL);
		}
	}
	return(0);
}

/*)Function	VOID	doparse()
 *
 *	The function doparse() evaluates all interactive
 *	command line or file input linker directives and
 *	updates the appropriate variables.
 *
 *	local variables:
 *		none
 *
 *	global variables:
 *		FILE *	stdin		standard input
 *		FILE *	stdout		standard output
 *		lfile	*cfp		The pointer *cfp points to the
 *				 	current lfile structure
 *		FILE	*sfp		The file handle sfp points to the
 *				 	currently open file
 *		char	ib[NINPUT]	.rel file text line
 *		char	*ip		pointer into the .rel file
 *		lfile	*filep	 	The pointer *filep points to the
 *				 	beginning of a linked list of
 *				 	lfile structures.
 *		lfile	*startp		asmlnk startup file structure
 *		int	pflag		print linker command file flag
 *
 *	Functions called:
 *		int	fclose()	c_library
 *		int	fprintf()	c_library
 *		VOID	getfid()	lklex.c
 *		int	getline()	lklex.c
 *		int	parse()		lkmain.c
 *
 *	side effects:
 *		Various linker flags are updated and the linked
 *		structure lfile may be updated.
 */

VOID
doparse()
{
	cfp = NULL;
	sfp = NULL;
	filep = startp;
	while (1) {
		ip = ib;
		if (getline() == 0)
			break;
		if (pflag && cfp->f_type != F_STD)
			fprintf(stdout, "ASlink >> %s\n", ip);
		if (*ip == 0 || parse())
			break;
	}
	if(sfp != stdin) {
		fclose(sfp);
	}
	sfp = NULL;
	startp->f_idp = "";
	startp->f_idx = 0;
	startp->f_type = 0;
}

/*)Function	VOID	bassav()
 *
 *	The function bassav() creates a linked structure containing
 *	the base address strings input to the linker.
 *
 *	local variables:
 *		none
 *
 *	global variables:
 *		base	*basep		The pointer to the first
 *				 	base structure
 *		base	*bsp		Pointer to the current
 *				 	base structure
 *		char	*ip		pointer into the REL file
 *				 	text line in ib[]
 *
 *	 functions called:
 *		int	getnb()		lklex.c
 *		VOID *	new()		lksym.c
 *		int	strlen()	c_library
 *		char *	strcpy()	c_library
 *		VOID	unget()		lklex.c
 *
 *	side effects:
 *		The basep structure is created.
 */

VOID
bassav()
{
	if (basep == NULL) {
		basep = (struct base *)
			new (sizeof (struct base));
		bsp = basep;
	} else {
		bsp->b_base = (struct base *)
				new (sizeof (struct base));
		bsp = bsp->b_base;
	}
	unget(getnb());
	bsp->b_strp = (char *) new (strlen(ip)+1);
	strcpy(bsp->b_strp, ip);
}

/*)Function	VOID	setbas()
 *
 *	The function setbas() scans the base address lines in hte
 *	basep structure, evaluates the arguments, and sets beginning
 *	address of the specified areas.
 *
 *	local variables:
 *		int	v		expression value
 *		char	id[]		base id string
 *
 *	global variables:
 *		area	*ap		Pointer to the current
 *				 	area structure
 *		area	*areap		The pointer to the first
 *				 	area structure of a linked list
 *		base	*basep		The pointer to the first
 *				 	base structure
 *		base	*bsp		Pointer to the current
 *				 	base structure
 *		char	*ip		pointer into the REL file
 *				 	text line in ib[]
 *		int	lkerr		error flag
 *
 *	 functions called:
 *		addr_t	expr()		lkeval.c
 *		int	fprintf()	c_library
 *		VOID	getid()		lklex.c
 *		int	getnb()		lklex.c
 *		int	symeq()		lksym.c
 *
 *	side effects:
 *		The base address of an area is set.
 */

VOID
setbas()
{
	register int v;
	char id[NCPS];

	bsp = basep;
	while (bsp) {
		ip = bsp->b_strp;
		getid(id, -1);
		if (getnb() == '=') {
			v = expr(0);
			for (ap = areap; ap != NULL; ap = ap->a_ap) {
				if (symeq(id, ap->a_id, 0))
					break;
			}
			if (ap == NULL) {
				fprintf(stderr,
				"No definition of area %s\n", id);
				lkerr++;
			} else {
				ap->a_addr = v;
				ap->a_bset = 1;
			}
		} else {
			fprintf(stderr, "No '=' in base expression");
			lkerr++;
		}
		bsp = bsp->b_base;
	}
}

/*)Function	VOID	gblsav()
 *
 *	The function gblsav() creates a linked structure containing
 *	the global variable strings input to the linker.
 *
 *	local variable:
 *		none
 *
 *	global variables:
 *		globl	*globlp		The pointer to the first
 *				 	globl structure
 *		globl	*gsp		Pointer to the current
 *				 	globl structure
 *		char	*ip		pointer into the REL file
 *				 	text line in ib[]
 *		int	lkerr		error flag
 *
 *	functions called:
 *		int	getnb()		lklex.c
 *		VOID *	new()		lksym.c
 *		int	strlen()	c_library
 *		char *	strcpy()	c_library
 *		VOID	unget()		lklex.c
 *
 *	side effects:
 *		The globlp structure is created.
 */

VOID
gblsav()
{
	if (globlp == NULL) {
		globlp = (struct globl *)
			new (sizeof (struct globl));
		gsp = globlp;
	} else {
		gsp->g_globl = (struct globl *)
				new (sizeof (struct globl));
		gsp = gsp->g_globl;
	}
	unget(getnb());
	gsp->g_strp = (char *) new (strlen(ip)+1);
	strcpy(gsp->g_strp, ip);
}

/*)Function	VOID	setgbl()
 *
 *	The function setgbl() scans the global variable lines in the
 *	globlp structure, evaluates the arguments, and sets a variable
 *	to this value.
 *
 *	local variables:
 *		int	v		expression value
 *		char	id[]		base id string
 *		sym *	sp		pointer to a symbol structure
 *
 *	global variables:
 *		char	*ip		pointer into the REL file
 *				 	text line in ib[]
 *		globl	*globlp		The pointer to the first
 *				 	globl structure
 *		globl	*gsp		Pointer to the current
 *				 	globl structure
 *		FILE *	stderr		c_library
 *		int	lkerr		error flag
 *
 *	 functions called:
 *		addr_t	expr()		lkeval.c
 *		int	fprintf()	c_library
 *		VOID	getid()		lklex.c
 *		int	getnb()		lklex.c
 *		sym *	lkpsym()	lksym.c
 *
 *	side effects:
 *		The value of a variable is set.
 */

VOID
setgbl()
{
	register int v;
	register struct sym *sp;
	char id[NCPS];

	gsp = globlp;
	while (gsp) {
		ip = gsp->g_strp;
		getid(id, -1);
		if (getnb() == '=') {
			v = expr(0);
			sp = lkpsym(id, 0);
			if (sp == NULL) {
				fprintf(stderr,
				"No definition of symbol %s\n", id);
				lkerr++;
			} else {
				if (sp->s_type & S_DEF) {
					fprintf(stderr,
					"Redefinition of symbol %s\n", id);
					lkerr++;
					sp->s_axp = NULL;
				}
				sp->s_addr = v;
				sp->s_type |= S_DEF;
			}
		} else {
			fprintf(stderr, "No '=' in global expression");
			lkerr++;
		}
		gsp = gsp->g_globl;
	}
}

/*)Function	FILE *	afile(fn, ft, wf)
 *
 *		char *	fn		file specification string
 *		char *	ft		file type string
 *		int	wf		read(0)/write(1) flag
 *
 *	The function afile() opens a file for reading or writing.
 *		(1)	If the file type specification string ft
 *			is not NULL then a file specification is
 *			constructed with the file path\name in fn
 *			and the extension in ft.
 *		(2)	If the file type specification string ft
 *			is NULL then the file specification is
 *			constructed from fn.  If fn does not have
 *			a file type then the default .rel file
 *			type is appended to the file specification.
 *
 *	afile() returns a file handle for the opened file or aborts
 *	the assembler on an open error.
 *
 *	local variables:
 *		int	c		character value
 *		char	fb[]		constructed file specification string
 *		FILE *	fp		filehandle for opened file
 *		char *	p1		pointer to filespec string fn
 *		char *	p2		pointer to filespec string fb
 *		char *	p3		pointer to filetype string ft
 *
 *	global variables:
 *		int	lkerr		error flag
 *
 *	functions called:
 *		int	fndidx()	lkmain.c
 *		FILE *	fopen()		c_library
 *		int	fprintf()	c_library
 *
 *	side effects:
 *		File is opened for read or write.
 */

FILE *
afile(fn, ft, wf)
char *fn;
char *ft;
int wf;
{
	register char *p1, *p2, *p3;
	register int c;
	FILE *fp;
	char fb[FILSPC];

	if (strlen(fn) > (FILSPC-5)) {
		fprintf(stderr, "File Specification %s is too long.", fn);
		lkerr++;
		return(NULL);
	}

	/*
	 * Skip The Path
	 */
	strcpy(fb, fn);
	c = fndidx(fb);
	p1 = &fb[c];
	p2 = &fn[c];

	/*
	 * Skip to File Extension Seperator
	 */
	while ((c = *p2++) != 0 && c != FSEPX) {
		p1++;
	}
	*p1++ = FSEPX;

	/*
	 * Copy File Extension
	 */
	 p3 = ft;
	 if (*p3 == 0) {
		if (c == FSEPX) {
			p3 = p2;
		} else {
			p3 = "rel";
		}
	}
	while ((c = *p3++) != 0) {
		if (p1 < &fb[FILSPC-1])
			*p1++ = c;
	}
	*p1++ = 0;
	if ((fp = fopen(fb, wf?"w":"r")) == NULL) {
		fprintf(stderr, "%s: cannot %s.\n", fb, wf?"create":"open");
		lkerr++;
	}
	return (fp);
}

/*)Function	int	fndidx(str)
 *
 *		char *	str		file specification string
 *
 *	The function fndidx() scans the file specification string
 *	to find the index to the file name.  If the file
 *	specification contains a 'path' then the index will
 *	be non zero.
 *
 *	fndidx() returns the index value.
 *
 *	local variables:
 *		char *	p1		temporary pointer
 *		char *	p2		temporary pointer
 *
 *	global variables:
 *		none
 *
 *	functions called:
 *		char *	strrchr()	c_library
 *
 *	side effects:
 *		none
 */

int
fndidx(str)
char *str;
{
	register char *p1, *p2;

	/*
	 * Skip Path Delimiters
	 */
	p1 = str;
	if ((p2 = strrchr(p1,  ':')) != NULL) { p1 = p2 + 1; }
	if ((p2 = strrchr(p1,  '/')) != NULL) { p1 = p2 + 1; }
	if ((p2 = strrchr(p1, '\\')) != NULL) { p1 = p2 + 1; }

	return(p1 - str);
}

char *usetxt[] = {
	"Usage: [-Options] [-Option with arg] file [file ...]",
	"  -p   Echo commands to stdout (default)",
	"  -n   No echo of commands to stdout",
	"Alternates to Command Line Input:",
	"  -c                   ASlink >> prompt input",
	"  -f   file[.lnk]      Command File input",
	"Librarys:",
	"  -k   Library path specification, one per -k",
	"  -l   Library file specification, one per -l",
	"Relocation:",
	"  -b   area base address=expression",
	"  -g   global symbol=expression",
	"Map format:",
	"  -m   Map output generated as file[.map]",
	"  -w   Wide listing format for map file",
	"  -x   Hexidecimal (default)",
	"  -d   Decimal",
	"  -q   Octal",
	"Output:",
	"  -i   Intel Hex as file[.i--]",
	"  -s   Motorola S Record as file[.s--]",
	"  -o   Linked file/library object output enable (default)",
	"  -v   Linked file/library object output disable",
	"List:",
	"  -u   Update listing file(s) with link data as file(s)[.rst]",
	"Case Sensitivity:",
	"  -z   Enable Case Sensitivity for Symbols",
	"End:",
	"  -e   or null line terminates input",
	"",
	0
};

/*)Function	VOID	usage(n)
 *
 *		int	n		exit code
 *
 *	The function usage() outputs to the stderr device the
 *	linker name and version and a list of valid linker options.
 *
 *	local variables:
 *		char **	dp		pointer to an array of
 *					text string pointers.
 *
 *	global variables:
 *		FILE *	stderr		c_library
 *
 *	functions called:
 *		int	fprintf()	c_library
 *
 *	side effects:
 *		none
 */

VOID
usage(n)
int n;
{
	register char	**dp;

	fprintf(stderr, "\nASxxxx Linker %s\n\n", VERSION);
	for (dp = usetxt; *dp; dp++)
		fprintf(stderr, "%s\n", *dp);
	lkexit(n);
}
