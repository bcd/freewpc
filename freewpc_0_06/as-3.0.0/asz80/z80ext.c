/* z80ext.c */

/*
 * (C) Copyright 1989-2000
 * All Rights Reserved
 *
 * Alan R. Baldwin
 * 721 Berkeley St.
 * Kent, Ohio  44240
 */

#include <stdio.h>
#include <setjmp.h>
#include "asxxxx.h"
#include "z80.h"

char	*cpu	= "Zilog Z80 / Hitachi HD64180";
int	hilo	= 0;
char	*dsft	= "asm";
