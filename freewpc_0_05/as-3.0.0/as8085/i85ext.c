/* i85ext.c */

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
#include "i8085.h"

char	*cpu	= "Intel 8085";
int	hilo	= 0;
char	*dsft	= "asm";
