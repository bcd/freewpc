/*
 * $Header: /home/eric/src/gcc-2.95.3/gcc/config/m6809/RCS/xm-m6809.h,v 1.1 2001/12/13 02:30:08 eric Exp $
 */
/*-------------------------------------------------------------------
	FILE: xm-m6809.h
	SCCS: @(#)xm-m6809.h	1.2 SAL 1/22/92
-------------------------------------------------------------------*/
/* Configuration for GNU C-compiler for MC6809
   Copyright (C) 1989 Free Software Foundation, Inc.

 MC6809 Version by Tom Jones (jones@sal.wisc.edu)
 Space Astronomy Laboratory
 University of Wisconsin at Madison

 minor changes to adapt it to gcc-2.5.8 by Matthias Doerfel
 ( msdoerfe@informatik.uni-erlangen.de )
 also added #pragma interrupt (inspired by gcc-6811)

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* #defines that need visibility everywhere.  */
#define FALSE 0
#define TRUE 1

/* target machine dependencies.
   tm.h is a symbolic link to the actual target specific file.   */
#include "tm.h"

/* This describes the machine the compiler is hosted on.  */

#define HOST_BITS_PER_CHAR 8
#define HOST_BITS_PER_SHORT 16
#define HOST_BITS_PER_INT 32
#define HOST_BITS_PER_LONG 32

/* Arguments to use with `exit'.  */
#define SUCCESS_EXIT_CODE 0
#define FATAL_EXIT_CODE 33

/* If compiled with GNU C, use the built-in alloca */
#ifdef __GNUC__
#define alloca __builtin_alloca
#endif

