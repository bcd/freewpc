/* ds8ext.c */

/*
 * Modified from i51ext.c
 * Bill McKinnon
 * w_mckinnon@conknet.com
 *
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
#include "ds8.h"

char	*cpu	= "Dallas Semiconductor [User Defined]";
int	hilo	= 1;
char	*dsft	= "asm";
