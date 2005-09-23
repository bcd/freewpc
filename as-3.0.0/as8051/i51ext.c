/* i51ext.c */

/*
 * (C) Copyright 1998-2000
 * All Rights Reserved
 *
 * Alan R. Baldwin
 * 721 Berkeley St.
 * Kent, Ohio  44240
 *
 *   This Assember Ported by
 *	John L. Hartman	(JLH)
 *	jhartman@compuserve.com
 *
 */

#include <stdio.h>
#include <setjmp.h>
#include "asxxxx.h"
#include "i8051.h"

char	*cpu	= "Intel 8051";
int	hilo	= 1;
char	*dsft	= "asm";
